// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "AlignClassifier.h"
#include "Align.h"
#include "AlignInference.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace slang::format;

namespace {

using Kind = LineMetadata::Kind;

constexpr auto npos = std::string_view::npos;

constexpr std::array TypeKeywords{
    std::string_view{"bit"},      std::string_view{"byte"},      std::string_view{"int"},
    std::string_view{"integer"},  std::string_view{"logic"},     std::string_view{"longint"},
    std::string_view{"real"},     std::string_view{"realtime"},  std::string_view{"reg"},
    std::string_view{"shortint"}, std::string_view{"shortreal"}, std::string_view{"string"},
    std::string_view{"time"},     std::string_view{"wire"},
};

bool isTypeKeyword(std::string_view word) {
    return std::ranges::any_of(TypeKeywords, [&](auto kw) { return kw == word; });
}

bool isDirectionKeyword(std::string_view word) {
    return word == "inout" || word == "input" || word == "output" || word == "ref";
}

bool isParameterKeyword(std::string_view word) {
    return word == "localparam" || word == "parameter";
}

bool isWordChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
}

std::string_view extractWord(std::string_view line, size_t pos) {
    auto start = pos;
    while (pos < line.size() && isWordChar(line[pos])) {
        pos++;
    }
    return line.substr(start, pos - start);
}

size_t skipDimensions(std::string_view line, size_t pos) {
    while (pos < line.size()) {
        pos = skipSpaces(line, pos);
        if (pos >= line.size() || line[pos] != '[') {
            break;
        }

        int depth = 1;
        pos++;
        while (pos < line.size() && depth > 0) {
            if (line[pos] == '[') {
                depth++;
            }
            else if (line[pos] == ']') {
                depth--;
            }
            pos++;
        }
    }
    return pos;
}

size_t skipSignedness(std::string_view line, size_t pos) {
    pos = skipSpaces(line, pos);
    auto word = extractWord(line, pos);
    if (word == "signed" || word == "unsigned") {
        pos += word.size();
    }
    return pos;
}

size_t skipTypeQualifiers(std::string_view line, size_t pos) {
    pos = skipSignedness(line, pos);
    return skipDimensions(line, pos);
}

size_t skipTimingControl(std::string_view line, size_t pos) {
    if (pos >= line.size() || line[pos] != '#') {
        return pos;
    }
    pos++;

    if (pos < line.size() && line[pos] == '(') {
        int depth = 1;
        pos++;
        while (pos < line.size() && depth > 0) {
            if (line[pos] == '(') {
                depth++;
            }
            else if (line[pos] == ')') {
                depth--;
            }
            pos++;
        }
        return pos;
    }

    auto start = pos;
    while (pos < line.size() && isWordChar(line[pos])) {
        pos++;
    }
    if (pos == start) {
        return start - 1;
    }
    return pos;
}

size_t skipDirectionType(std::string_view line, size_t pos) {
    auto nextWord = extractWord(line, pos);
    if (isTypeKeyword(nextWord)) {
        return skipTypeQualifiers(line, pos + nextWord.size());
    }

    if (nextWord == "signed" || nextWord == "unsigned") {
        return skipDimensions(line, pos + nextWord.size());
    }

    if (pos < line.size() && line[pos] == '[') {
        return skipDimensions(line, pos);
    }
    return pos;
}

size_t findEquals(std::string_view line, size_t pos) {
    while (pos < line.size()) {
        if (line[pos] == '=') {
            if (pos + 1 < line.size() && line[pos + 1] == '=') {
                pos += 2;
                continue;
            }
            if (pos > 0 && (line[pos - 1] == '!' || line[pos - 1] == '<' || line[pos - 1] == '>')) {
                pos++;
                continue;
            }
            return pos;
        }
        pos++;
    }
    return npos;
}

size_t skipAssignmentLHS(std::string_view line, size_t pos) {
    pos += extractWord(line, pos).size();
    while (pos < line.size()) {
        if (line[pos] == '.') {
            pos++;
            pos += extractWord(line, pos).size();
        }
        else if (line[pos] == '[') {
            pos = skipDimensions(line, pos);
        }
        else {
            break;
        }
    }
    return pos;
}

size_t findAssignOperator(std::string_view line, size_t pos) {
    if (pos >= line.size()) {
        return npos;
    }

    auto remaining = line.size() - pos;
    auto c = line[pos];

    if (c == '<') {
        if (remaining >= 4 && line[pos + 1] == '<' && line[pos + 2] == '<' &&
            line[pos + 3] == '=') {
            return pos;
        }

        if (remaining >= 3 && line[pos + 1] == '<' && line[pos + 2] == '=') {
            return pos;
        }

        if (remaining >= 2 && line[pos + 1] == '=') {
            return pos;
        }

        return npos;
    }

    if (c == '>') {
        if (remaining >= 4 && line[pos + 1] == '>' && line[pos + 2] == '>' &&
            line[pos + 3] == '=') {
            return pos;
        }

        if (remaining >= 3 && line[pos + 1] == '>' && line[pos + 2] == '=') {
            return pos;
        }

        return npos;
    }

    if (c == '=' && (remaining < 2 || line[pos + 1] != '=')) {
        return pos;
    }

    if ((c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '&' || c == '|' ||
         c == '^' || c == ':') &&
        remaining >= 2 && line[pos + 1] == '=') {
        return pos;
    }

    return npos;
}

bool isStatementKeyword(std::string_view word) {
    constexpr std::array keywords{
        std::string_view{"always"},      std::string_view{"always_comb"},
        std::string_view{"always_ff"},   std::string_view{"always_latch"},
        std::string_view{"begin"},       std::string_view{"case"},
        std::string_view{"casex"},       std::string_view{"casez"},
        std::string_view{"class"},       std::string_view{"deassign"},
        std::string_view{"do"},          std::string_view{"else"},
        std::string_view{"end"},         std::string_view{"endcase"},
        std::string_view{"endclass"},    std::string_view{"endfunction"},
        std::string_view{"endgenerate"}, std::string_view{"endinterface"},
        std::string_view{"endmodule"},   std::string_view{"endpackage"},
        std::string_view{"endtask"},     std::string_view{"enum"},
        std::string_view{"final"},       std::string_view{"for"},
        std::string_view{"force"},       std::string_view{"foreach"},
        std::string_view{"forever"},     std::string_view{"fork"},
        std::string_view{"function"},    std::string_view{"generate"},
        std::string_view{"if"},          std::string_view{"import"},
        std::string_view{"initial"},     std::string_view{"interface"},
        std::string_view{"join"},        std::string_view{"join_any"},
        std::string_view{"join_none"},   std::string_view{"module"},
        std::string_view{"package"},     std::string_view{"release"},
        std::string_view{"repeat"},      std::string_view{"return"},
        std::string_view{"struct"},      std::string_view{"task"},
        std::string_view{"typedef"},     std::string_view{"union"},
        std::string_view{"unique"},      std::string_view{"unique0"},
        std::string_view{"while"},
    };
    return std::ranges::any_of(keywords, [&](auto kw) { return kw == word; });
}

size_t findDimPos(std::string_view line, size_t pos, std::string_view firstWord) {
    if (isDirectionKeyword(firstWord) || isParameterKeyword(firstWord)) {
        auto nextWord = extractWord(line, pos);
        if (isTypeKeyword(nextWord)) {
            auto afterType = skipSignedness(line, pos + nextWord.size());
            auto afterSpaces = skipSpaces(line, afterType);
            if (afterSpaces < line.size() && line[afterSpaces] == '[') {
                return afterSpaces;
            }
        }
        else if (nextWord == "signed" || nextWord == "unsigned") {
            auto afterSign = skipSpaces(line, pos + nextWord.size());
            if (afterSign < line.size() && line[afterSign] == '[') {
                return afterSign;
            }
        }
        else if (pos < line.size() && line[pos] == '[') {
            return pos;
        }
    }
    else if (isTypeKeyword(firstWord)) {
        auto afterType = skipSignedness(line, pos + firstWord.size());
        auto afterSpaces = skipSpaces(line, afterType);
        if (afterSpaces < line.size() && line[afterSpaces] == '[') {
            return afterSpaces;
        }
    }
    return npos;
}

LineInfo classifyTimingControl(std::string_view line, size_t indentEnd) {
    auto afterKeyword = skipSpaces(line, indentEnd + std::string_view{"always"}.size());
    if (afterKeyword >= line.size() || line[afterKeyword] != '#') {
        return {.kind = Kind::Other, .indent = indentEnd};
    }

    auto afterDelay = skipTimingControl(line, afterKeyword);
    auto identStart = skipSpaces(line, afterDelay);
    if (identStart >= line.size() ||
        (std::isalpha(static_cast<unsigned char>(line[identStart])) == 0 &&
         line[identStart] != '_')) {
        return {.kind = Kind::Other, .indent = indentEnd};
    }

    auto afterLHS = skipAssignmentLHS(line, identStart);
    auto opPos = findAssignOperator(line, skipSpaces(line, afterLHS));
    if (opPos == npos) {
        return {.kind = Kind::Other, .indent = indentEnd};
    }

    return {.kind = Kind::TimingControl,
            .indent = indentEnd,
            .typeWidth = identStart - indentEnd,
            .identPos = identStart,
            .equalsPos = opPos,
            .dimPos = npos,
            .depth = 0};
}

size_t findPortConnectionParen(std::string_view line) {
    auto indentEnd = line.find_first_not_of(' ');
    if (indentEnd == npos || line[indentEnd] != '.') {
        return npos;
    }

    auto nameStart = indentEnd + 1;
    if (nameStart >= line.size() ||
        (std::isalpha(static_cast<unsigned char>(line[nameStart])) == 0 &&
         line[nameStart] != '_')) {
        return npos;
    }

    auto pos = nameStart;
    while (pos < line.size() &&
           (std::isalnum(static_cast<unsigned char>(line[pos])) != 0 || line[pos] == '_')) {
        pos++;
    }

    auto parenPos = skipSpaces(line, pos);
    if (parenPos >= line.size() || line[parenPos] != '(') {
        return npos;
    }

    return parenPos;
}

LineInfo classifyLine(std::string_view line, bool formatOff) {
    if (line.empty() || line.find_first_not_of(' ') == std::string_view::npos) {
        return {.kind = Kind::Empty};
    }

    auto indentEnd = line.find_first_not_of(' ');
    auto content = line.substr(indentEnd);

    if (content.starts_with("//") || content.starts_with("/*")) {
        return {.kind = Kind::Comment, .indent = indentEnd};
    }

    if (content == ") (" || content == ");" || content == ")" || content == "#(") {
        return {.kind = Kind::PortListBoundary, .indent = indentEnd};
    }

    if (formatOff) {
        return {.kind = Kind::Other, .indent = indentEnd};
    }

    auto firstWord = extractWord(content, 0);
    if (firstWord.empty()) {
        return {.kind = Kind::Other, .indent = indentEnd};
    }

    if (firstWord == "always") {
        return classifyTimingControl(line, indentEnd);
    }

    size_t pos = indentEnd;
    size_t dimPos = npos;
    size_t keywordWidth = 0;
    if (isDirectionKeyword(firstWord) || isParameterKeyword(firstWord)) {
        keywordWidth = firstWord.size();
        pos = skipSpaces(line, pos + firstWord.size());
        dimPos = findDimPos(line, pos, firstWord);
        pos = skipDirectionType(line, pos);
    }
    else if (isTypeKeyword(firstWord)) {
        dimPos = findDimPos(line, indentEnd, firstWord);
        pos = skipTypeQualifiers(line, pos + firstWord.size());
    }
    else {
        auto identStart = indentEnd;
        if (firstWord == "assign") {
            identStart = skipSpaces(line, indentEnd + firstWord.size());
        }
        else if (isStatementKeyword(firstWord)) {
            return {.kind = Kind::Other, .indent = indentEnd};
        }

        if (identStart >= line.size() ||
            (std::isalpha(static_cast<unsigned char>(line[identStart])) == 0 &&
             line[identStart] != '_')) {
            return {.kind = Kind::Other, .indent = indentEnd};
        }

        auto afterLHS = skipAssignmentLHS(line, identStart);
        auto opPos = findAssignOperator(line, skipSpaces(line, afterLHS));
        if (opPos == npos) {
            return {.kind = Kind::Other, .indent = indentEnd};
        }

        return {.kind = Kind::Assignment,
                .indent = indentEnd,
                .typeWidth = 0,
                .identPos = identStart,
                .equalsPos = opPos,
                .dimPos = npos,
                .depth = 0};
    }

    pos = skipSpaces(line, pos);
    if (pos >= line.size() ||
        (std::isalpha(static_cast<unsigned char>(line[pos])) == 0 && line[pos] != '_')) {
        return {.kind = Kind::Other, .indent = indentEnd};
    }

    auto eqPos = findEquals(line, pos);
    return {.kind = Kind::Declaration,
            .indent = indentEnd,
            .keywordWidth = keywordWidth,
            .typeWidth = pos - indentEnd,
            .identPos = pos,
            .equalsPos = eqPos,
            .dimPos = dimPos,
            .depth = 0};
}

bool isContinuationLine(std::string_view line) {
    auto indentEnd = line.find_first_not_of(' ');
    if (indentEnd == npos) {
        return false;
    }

    if (std::isalpha(static_cast<unsigned char>(line[indentEnd])) == 0 && line[indentEnd] != '_') {
        return false;
    }

    auto word = extractWord(line, indentEnd);
    auto afterWord = skipSpaces(line, indentEnd + word.size());
    if (afterWord >= line.size() || line[afterWord] != '=') {
        return false;
    }

    if (afterWord + 1 < line.size() && line[afterWord + 1] == '=') {
        return false;
    }

    auto lastNonSpace = line.find_last_not_of(' ');
    return lastNonSpace != npos && (line[lastNonSpace] == ',' || line[lastNonSpace] == ';');
}

bool followsDeclaration(const std::vector<LineInfo>& infos, size_t i) {
    for (auto j = i; j > 0; j--) {
        auto prevKind = infos[j - 1].kind;
        if (prevKind == Kind::Assignment || prevKind == Kind::Declaration ||
            prevKind == Kind::Continuation) {
            return true;
        }

        if (prevKind == Kind::Empty || prevKind == Kind::Comment) {
            continue;
        }

        break;
    }
    return false;
}

void reclassifyContinuationLines(const std::vector<std::string_view>& lines,
                                 std::vector<LineInfo>& infos) {
    for (size_t i = 1; i < infos.size(); i++) {
        if (infos[i].kind != Kind::Other && infos[i].kind != Kind::Assignment) {
            continue;
        }

        if (infos[i].metadataKind) {
            continue;
        }

        if (!followsDeclaration(infos, i) || !isContinuationLine(lines[i])) {
            continue;
        }

        auto indentEnd = lines[i].find_first_not_of(' ');
        infos[i] = {.kind = Kind::Continuation,
                    .indent = indentEnd,
                    .typeWidth = 0,
                    .identPos = indentEnd,
                    .equalsPos = findEquals(lines[i], indentEnd),
                    .dimPos = npos,
                    .depth = infos[i].depth,
                    .maxExcess = infos[i].maxExcess,
                    .metadataKind = infos[i].metadataKind};
    }
}

size_t findDistOperator(std::string_view line) {
    for (size_t j = 0; j + 1 < line.size(); j++) {
        if (line[j] == ':' && line[j + 1] == '=') {
            return j;
        }
    }
    return npos;
}

void reclassifyAsDeclaration(std::string_view line, LineInfo& info) {
    auto fw = extractWord(line, info.indent);
    if (fw.empty()) {
        return;
    }

    auto afterType = skipSpaces(line, info.indent + fw.size());
    if (afterType < line.size() &&
        (std::isalpha(static_cast<unsigned char>(line[afterType])) != 0 ||
         line[afterType] == '_')) {
        info.typeWidth = afterType - info.indent;
        info.identPos = afterType;
    }
}

void applyLineMetadata(std::string_view line, LineInfo& info, const LineMetadata& meta,
                       bool formatOff) {
    info.depth = meta.depth;
    info.maxExcess = computeMetaExcess(meta);

    if (!formatOff && meta.kind != Kind::Other) {
        if (meta.kind == Kind::Declaration && info.kind == Kind::Other) {
            reclassifyAsDeclaration(line, info);
        }
        info.kind = meta.kind;
        info.metadataKind = true;
    }

    if (info.kind == Kind::PortConnection) {
        info.equalsPos = findPortConnectionParen(line);
    }
    else if (info.kind == Kind::DistItem && info.equalsPos == npos) {
        info.equalsPos = findDistOperator(line);
    }
}

} // namespace

namespace slang::format {

size_t skipSpaces(std::string_view line, size_t pos) {
    while (pos < line.size() && line[pos] == ' ') {
        pos++;
    }
    return pos;
}

size_t trimTrailingSpaces(std::string_view line, size_t pos) {
    while (pos > 0 && line[pos - 1] == ' ') {
        pos--;
    }
    return pos;
}

size_t findTrailingComment(std::string_view line) {
    auto inString = false;
    for (size_t i = 0; i < line.size(); i++) {
        if (line[i] == '"' && (i == 0 || line[i - 1] != '\\')) {
            inString = !inString;
            continue;
        }

        if (inString) {
            continue;
        }

        if (line[i] == '/' && i + 1 < line.size()) {
            if (line[i + 1] == '/' || line[i + 1] == '*') {
                return i;
            }
        }
    }
    return std::string_view::npos;
}

std::vector<LineInfo> classifyLines(const std::vector<std::string_view>& lines,
                                    const std::vector<LineMetadata>& lineMetadata,
                                    const std::unordered_map<size_t, size_t>& metaByLine) {
    std::vector<LineInfo> infos;
    infos.reserve(lines.size());
    auto formatOff = false;
    for (size_t i = 0; i < lines.size(); i++) {
        if (lines[i].find("slang-format off") != std::string_view::npos) {
            formatOff = true;
        }
        else if (lines[i].find("slang-format on") != std::string_view::npos) {
            formatOff = false;
        }

        auto info = classifyLine(lines[i], formatOff);
        auto it = metaByLine.find(i);
        if (it != metaByLine.end()) {
            applyLineMetadata(lines[i], info, lineMetadata[it->second], formatOff);
        }
        infos.push_back(info);
    }

    reclassifyContinuationLines(lines, infos);
    return infos;
}

} // namespace slang::format
