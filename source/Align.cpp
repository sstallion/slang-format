// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Align.h"
#include "Style.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

using namespace slang::format;

namespace {

enum class LineKind {
    Assignment,
    Continuation,
    Declaration,
    Comment,
    Empty,
    PortListBoundary,
    TimingControl,
    Other
};

struct LineInfo {
    LineKind kind;
    size_t indent;
    size_t typeWidth; ///< Distance from indent to identifier start.
    size_t identPos;  ///< Absolute position of identifier in the line.
    size_t equalsPos; ///< Position of '=' in declaration, or npos if absent.
    size_t dimPos;    ///< Position of first '[' after type keyword, or npos if absent.
    unsigned depth;   ///< AST nesting depth at the line's emission point.
};

constexpr auto npos = std::string_view::npos;

LineInfo makeEmpty() {
    return {.kind = LineKind::Empty,
            .indent = 0,
            .typeWidth = 0,
            .identPos = 0,
            .equalsPos = npos,
            .dimPos = npos,
            .depth = 0};
}

LineInfo makeComment(size_t indent) {
    return {.kind = LineKind::Comment,
            .indent = indent,
            .typeWidth = 0,
            .identPos = 0,
            .equalsPos = npos,
            .dimPos = npos,
            .depth = 0};
}

LineInfo makePortListBoundary(size_t indent) {
    return {.kind = LineKind::PortListBoundary,
            .indent = indent,
            .typeWidth = 0,
            .identPos = 0,
            .equalsPos = npos,
            .dimPos = npos,
            .depth = 0};
}

LineInfo makeOther(size_t indent) {
    return {.kind = LineKind::Other,
            .indent = indent,
            .typeWidth = 0,
            .identPos = 0,
            .equalsPos = npos,
            .dimPos = npos,
            .depth = 0};
}

LineKind toLineKind(LineMetadata::Kind mk) {
    using MK = LineMetadata::Kind;
    switch (mk) {
        case MK::Assignment:
            return LineKind::Assignment;
        case MK::Comment:
            return LineKind::Comment;
        case MK::Continuation:
            return LineKind::Continuation;
        case MK::Declaration:
            return LineKind::Declaration;
        case MK::Empty:
            return LineKind::Empty;
        case MK::PortListBoundary:
            return LineKind::PortListBoundary;
        case MK::TimingControl:
            return LineKind::TimingControl;
        case MK::Other:
            return LineKind::Other;
    }
    return LineKind::Other;
}

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
         c == '^') &&
        remaining >= 2 && line[pos + 1] == '=') {
        return pos;
    }

    return npos;
}

/// Returns the position of the trailing comment start in \p line, or npos if no trailing comment is
/// present.
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
    auto const other = makeOther(indentEnd);

    auto afterKeyword = skipSpaces(line, indentEnd + std::string_view{"always"}.size());
    if (afterKeyword >= line.size() || line[afterKeyword] != '#') {
        return other;
    }

    auto afterDelay = skipTimingControl(line, afterKeyword);
    auto identStart = skipSpaces(line, afterDelay);
    if (identStart >= line.size() ||
        (std::isalpha(static_cast<unsigned char>(line[identStart])) == 0 &&
         line[identStart] != '_')) {
        return other;
    }

    auto afterLHS = skipAssignmentLHS(line, identStart);
    auto opPos = findAssignOperator(line, skipSpaces(line, afterLHS));
    if (opPos == npos) {
        return other;
    }

    return {.kind = LineKind::TimingControl,
            .indent = indentEnd,
            .typeWidth = identStart - indentEnd,
            .identPos = identStart,
            .equalsPos = opPos,
            .dimPos = npos,
            .depth = 0};
}

LineInfo classifyLine(std::string_view line, bool formatOff) {
    if (line.empty() || line.find_first_not_of(' ') == std::string_view::npos) {
        return makeEmpty();
    }

    auto indentEnd = line.find_first_not_of(' ');
    auto content = line.substr(indentEnd);

    if (content.starts_with("//") || content.starts_with("/*")) {
        return makeComment(indentEnd);
    }

    if (content == ") (" || content == ");" || content == ")" || content == "#(") {
        return makePortListBoundary(indentEnd);
    }

    if (formatOff) {
        return makeOther(indentEnd);
    }

    auto firstWord = extractWord(content, 0);
    if (firstWord.empty()) {
        return makeOther(indentEnd);
    }

    if (firstWord == "always") {
        return classifyTimingControl(line, indentEnd);
    }

    size_t pos = indentEnd;
    size_t dimPos = npos;
    if (isDirectionKeyword(firstWord) || isParameterKeyword(firstWord)) {
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
            return makeOther(indentEnd);
        }

        if (identStart >= line.size() ||
            (std::isalpha(static_cast<unsigned char>(line[identStart])) == 0 &&
             line[identStart] != '_')) {
            return makeOther(indentEnd);
        }

        auto afterLHS = skipAssignmentLHS(line, identStart);
        auto opPos = findAssignOperator(line, skipSpaces(line, afterLHS));
        if (opPos == npos) {
            return makeOther(indentEnd);
        }

        return {.kind = LineKind::Assignment,
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
        return makeOther(indentEnd);
    }

    auto eqPos = findEquals(line, pos);
    return {.kind = LineKind::Declaration,
            .indent = indentEnd,
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

struct GroupRange {
    size_t start;
    size_t end;
};

void alignGroup(std::string& result, const std::vector<std::string_view>& lines,
                const std::vector<LineInfo>& infos, GroupRange range) {
    size_t declCount = 0;
    size_t maxTypeWidth = 0;
    std::optional<size_t> groupIndent;
    for (size_t i = range.start; i < range.end; i++) {
        if (infos[i].kind == LineKind::Declaration) {
            declCount++;
            maxTypeWidth = std::max(maxTypeWidth, infos[i].typeWidth);
            if (!groupIndent) {
                groupIndent = infos[i].indent;
            }
        }
        else if (infos[i].kind == LineKind::Continuation) {
            declCount++;
        }
    }

    auto targetAbsCol = groupIndent.value_or(0) + maxTypeWidth;

    for (size_t i = range.start; i < range.end; i++) {
        if (infos[i].kind == LineKind::Declaration && declCount >= 2) {
            auto line = lines[i];
            auto indent = infos[i].indent;
            auto identPos = infos[i].identPos;
            auto targetPos = indent + maxTypeWidth;

            auto typeTextEnd = trimTrailingSpaces(line, identPos);

            result.append(line.substr(0, typeTextEnd));
            result.append(targetPos - (typeTextEnd - indent) - indent, ' ');
            result.append(line.substr(identPos));
        }
        else if (infos[i].kind == LineKind::Continuation && declCount >= 2) {
            auto line = lines[i];
            auto indent = infos[i].indent;

            if (targetAbsCol > indent) {
                result.append(line.substr(0, indent));
                result.append(targetAbsCol - indent, ' ');
                result.append(line.substr(infos[i].identPos));
            }
            else {
                result.append(line);
            }
        }
        else {
            result.append(lines[i]);
        }
        result += '\n';
    }
}

struct AlignState {
    size_t groupStart = 0;
    bool inGroup = false;
    std::optional<size_t> groupIndent;
    std::optional<unsigned> groupDepth;
};

bool shouldBreakGroup(const LineInfo& info, const AlignState& state, bool acrossEmpty,
                      bool acrossComments, bool acrossIndent) {
    if (info.kind == LineKind::Continuation) {
        return false;
    }

    if (info.kind == LineKind::Declaration) {
        return state.inGroup && !acrossIndent && state.groupIndent &&
               *state.groupIndent != info.indent;
    }

    if (info.kind == LineKind::Empty) {
        return state.inGroup && !acrossEmpty;
    }

    if (info.kind == LineKind::Comment) {
        return state.inGroup && !acrossComments;
    }

    if (info.kind == LineKind::PortListBoundary) {
        return state.inGroup && !acrossIndent;
    }

    return true;
}

struct DimContent {
    std::string_view full;
    std::string_view left;
    std::string_view right;
    bool hasColon;
    size_t openPos;
    size_t closePos;
};

struct DimWidths {
    size_t maxFullWidth = 0;
    size_t maxLeftWidth = 0;
    size_t maxRightWidth = 0;
};

std::vector<DimContent> parseDimContents(std::string_view line, size_t pos) {
    std::vector<DimContent> dims;
    while (pos < line.size()) {
        pos = skipSpaces(line, pos);
        if (pos >= line.size() || line[pos] != '[') {
            break;
        }

        auto openPos = pos;
        pos++;
        auto depth = 1;
        auto contentStart = pos;
        while (pos < line.size() && depth > 0) {
            if (line[pos] == '[') {
                depth++;
            }
            else if (line[pos] == ']') {
                depth--;
            }

            if (depth > 0) {
                pos++;
            }
        }

        auto contentEnd = pos;
        auto closePos = pos;
        if (pos < line.size()) {
            pos++;
        }

        auto full = line.substr(contentStart, contentEnd - contentStart);
        auto colonPos = full.find(':');
        if (colonPos != npos) {
            dims.push_back({.full = full,
                            .left = full.substr(0, colonPos),
                            .right = full.substr(colonPos + 1),
                            .hasColon = true,
                            .openPos = openPos,
                            .closePos = closePos});
        }
        else {
            dims.push_back({.full = full,
                            .left = full,
                            .right = {},
                            .hasColon = false,
                            .openPos = openPos,
                            .closePos = closePos});
        }
    }
    return dims;
}

void emitDimContent(std::string& result, const DimContent& dim, const DimWidths& widths,
                    const AlignConsecutiveStyle& alignStyle) {
    if (alignStyle.AlignColon && dim.hasColon) {
        result.append(widths.maxLeftWidth - dim.left.size(), ' ');
        result.append(dim.left);
        result += ':';

        if (alignStyle.PadLeft) {
            result.append(widths.maxRightWidth - dim.right.size(), ' ');
            result.append(dim.right);
        }
        else if (alignStyle.PadRight) {
            result.append(dim.right);
            result.append(widths.maxRightWidth - dim.right.size(), ' ');
        }
        else {
            result.append(dim.right);
        }
    }
    else if (alignStyle.PadLeft) {
        result.append(widths.maxFullWidth - dim.full.size(), ' ');
        result.append(dim.full);
    }
    else if (alignStyle.PadRight) {
        result.append(dim.full);
        result.append(widths.maxFullWidth - dim.full.size(), ' ');
    }
    else {
        result.append(dim.full);
    }
}

void emitRewrittenDims(std::string& result, std::string_view line,
                       const std::vector<DimContent>& dims,
                       const std::vector<DimWidths>& slotWidths,
                       const AlignConsecutiveStyle& alignStyle) {
    size_t lastClose = npos;
    for (size_t j = 0; j < dims.size(); j++) {
        if (j > 0 && dims[j].openPos > lastClose + 1) {
            result.append(dims[j].openPos - lastClose - 1, ' ');
        }

        result += '[';
        emitDimContent(result, dims[j], slotWidths[j], alignStyle);
        result += ']';
        lastClose = dims[j].closePos;
    }

    auto origLastClose = dims.back().closePos;
    if (origLastClose + 1 < line.size()) {
        result.append(line.substr(origLastClose + 1));
    }
}

std::vector<DimWidths> computeSlotWidths(const std::vector<std::vector<DimContent>>& allDims) {
    size_t maxSlots = 0;
    for (const auto& dims : allDims) {
        maxSlots = std::max(maxSlots, dims.size());
    }

    std::vector<DimWidths> slotWidths(maxSlots);
    for (const auto& dims : allDims) {
        for (size_t j = 0; j < dims.size(); j++) {
            slotWidths[j].maxFullWidth = std::max(slotWidths[j].maxFullWidth, dims[j].full.size());

            if (dims[j].hasColon) {
                slotWidths[j].maxLeftWidth = std::max(slotWidths[j].maxLeftWidth,
                                                      dims[j].left.size());
                slotWidths[j].maxRightWidth = std::max(slotWidths[j].maxRightWidth,
                                                       dims[j].right.size());
            }
        }
    }
    return slotWidths;
}

std::vector<std::vector<DimContent>> collectDimContents(const std::vector<std::string_view>& lines,
                                                        const std::vector<LineInfo>& infos,
                                                        GroupRange range) {
    std::vector<std::vector<DimContent>> allDims;
    for (size_t i = range.start; i < range.end; i++) {
        if (infos[i].kind == LineKind::Declaration && infos[i].dimPos != npos) {
            allDims.push_back(parseDimContents(lines[i], infos[i].dimPos));
        }
        else {
            allDims.emplace_back();
        }
    }
    return allDims;
}

void alignGroupDimensions(std::string& result, const std::vector<std::string_view>& lines,
                          const std::vector<LineInfo>& infos, GroupRange range,
                          const AlignConsecutiveStyle& alignStyle) {
    size_t dimCount = 0;
    size_t maxDimCol = 0;
    for (size_t i = range.start; i < range.end; i++) {
        if (infos[i].kind == LineKind::Declaration && infos[i].dimPos != npos) {
            dimCount++;
            maxDimCol = std::max(maxDimCol, infos[i].dimPos);
        }
        else if (infos[i].kind == LineKind::Declaration) {
            maxDimCol = std::max(maxDimCol, infos[i].identPos);
        }
    }

    auto const contentAlign = alignStyle.AlignColon || alignStyle.PadLeft || alignStyle.PadRight;

    std::vector<std::vector<DimContent>> allDims;
    std::vector<DimWidths> slotWidths;
    if (contentAlign && dimCount >= 2) {
        allDims = collectDimContents(lines, infos, range);
        slotWidths = computeSlotWidths(allDims);
    }

    size_t dimIdx = 0;
    for (size_t i = range.start; i < range.end; i++) {
        if (infos[i].kind == LineKind::Declaration && infos[i].dimPos != npos && dimCount >= 2) {
            auto line = lines[i];
            auto dp = infos[i].dimPos;

            auto preDimEnd = trimTrailingSpaces(line, dp);

            result.append(line.substr(0, preDimEnd));
            result.append(maxDimCol - preDimEnd, ' ');

            if (contentAlign && dimIdx < allDims.size() && !allDims[dimIdx].empty()) {
                emitRewrittenDims(result, line, allDims[dimIdx], slotWidths, alignStyle);
            }
            else {
                result.append(line.substr(dp));
            }

            dimIdx++;
        }
        else {
            result.append(lines[i]);
            if (contentAlign) {
                dimIdx++;
            }
        }
        result += '\n';
    }
}

bool isAlignableKind(const LineInfo& info) {
    return info.kind == LineKind::Assignment || info.kind == LineKind::Declaration ||
           info.kind == LineKind::Continuation;
}

void alignGroupEquals(std::string& result, const std::vector<std::string_view>& lines,
                      const std::vector<LineInfo>& infos, GroupRange range) {
    // Collect unique depths present among alignable lines in this group.
    std::vector<unsigned> depths;
    for (auto i = range.start; i < range.end; i++) {
        if (isAlignableKind(infos[i]) && infos[i].equalsPos != npos) {
            if (std::ranges::find(depths, infos[i].depth) == depths.end()) {
                depths.push_back(infos[i].depth);
            }
        }
    }

    // Compute the max equals column for each depth independently.
    std::vector<size_t> maxEqualsCols(depths.size(), 0);
    std::vector<size_t> equalsCounts(depths.size(), 0);
    for (auto i = range.start; i < range.end; i++) {
        if (!isAlignableKind(infos[i]) || infos[i].equalsPos == npos) {
            continue;
        }

        auto it = std::ranges::find(depths, infos[i].depth);
        if (it != depths.end()) {
            auto idx = static_cast<size_t>(it - depths.begin());
            equalsCounts[idx]++;
            maxEqualsCols[idx] = std::max(maxEqualsCols[idx], infos[i].equalsPos);
        }
    }

    for (auto i = range.start; i < range.end; i++) {
        if (!isAlignableKind(infos[i]) || infos[i].equalsPos == npos) {
            result.append(lines[i]);
            result += '\n';
            continue;
        }

        auto it = std::ranges::find(depths, infos[i].depth);
        auto idx = static_cast<size_t>(it - depths.begin());
        if (equalsCounts[idx] >= 2) {
            auto line = lines[i];
            auto eqPos = infos[i].equalsPos;

            auto preEqualsEnd = trimTrailingSpaces(line, eqPos);

            result.append(line.substr(0, preEqualsEnd));
            result.append(maxEqualsCols[idx] - preEqualsEnd, ' ');
            result.append(line.substr(eqPos));
        }
        else {
            result.append(lines[i]);
        }
        result += '\n';
    }
}

bool shouldBreakDimGroup(const LineInfo& info, const AlignState& state, bool acrossEmpty,
                         bool acrossComments, bool acrossIndent) {
    if ((info.kind == LineKind::Declaration || info.kind == LineKind::Continuation) &&
        info.dimPos == npos) {
        return false;
    }
    return shouldBreakGroup(info, state, acrossEmpty, acrossComments, acrossIndent);
}

bool canStartDeclGroup(const LineInfo& info) {
    return info.kind == LineKind::Declaration;
}

bool canStartDimGroup(const LineInfo& info) {
    return info.kind == LineKind::Declaration;
}

bool shouldBreakAssignGroup(const LineInfo& info, const AlignState& state, bool acrossEmpty,
                            bool acrossComments, bool acrossIndent) {
    if (info.kind == LineKind::Continuation) {
        return false;
    }

    if (info.kind == LineKind::Assignment || info.kind == LineKind::Declaration) {
        if (state.inGroup && state.groupDepth && *state.groupDepth != info.depth) {
            return false;
        }
        return state.inGroup && !acrossIndent && state.groupIndent &&
               *state.groupIndent != info.indent;
    }

    if (info.kind == LineKind::Empty) {
        return state.inGroup && !acrossEmpty;
    }

    if (info.kind == LineKind::Comment) {
        return state.inGroup && !acrossComments;
    }

    if (info.kind == LineKind::PortListBoundary) {
        return state.inGroup && !acrossIndent;
    }

    return state.inGroup && state.groupDepth && info.depth < *state.groupDepth;
}

bool canStartAssignGroup(const LineInfo& info) {
    return info.kind == LineKind::Assignment || info.kind == LineKind::Declaration;
}

bool shouldBreakTimingGroup(const LineInfo& info, const AlignState& state, bool acrossEmpty,
                            bool acrossComments, bool /*acrossIndent*/) {
    if (info.kind == LineKind::TimingControl) {
        return state.inGroup && state.groupIndent && *state.groupIndent != info.indent;
    }

    if (info.kind == LineKind::Empty) {
        return state.inGroup && !acrossEmpty;
    }

    if (info.kind == LineKind::Comment) {
        return state.inGroup && !acrossComments;
    }

    return true;
}

bool canStartTimingGroup(const LineInfo& info) {
    return info.kind == LineKind::TimingControl;
}

bool shouldBreakTrailingCommentGroup(const LineInfo& info, const AlignState& state,
                                     bool acrossEmpty, bool acrossComments, bool acrossIndent) {
    if (info.kind == LineKind::Assignment || info.kind == LineKind::Declaration ||
        info.kind == LineKind::TimingControl || info.kind == LineKind::Continuation) {
        return state.inGroup && !acrossIndent && state.groupIndent &&
               *state.groupIndent != info.indent;
    }

    if (info.kind == LineKind::Empty) {
        return state.inGroup && !acrossEmpty;
    }

    if (info.kind == LineKind::Comment) {
        return state.inGroup && !acrossComments;
    }

    if (info.kind == LineKind::PortListBoundary) {
        return state.inGroup && !acrossIndent;
    }

    return true;
}

bool canStartTrailingCommentGroup(const LineInfo& info) {
    return info.kind == LineKind::Assignment || info.kind == LineKind::Declaration ||
           info.kind == LineKind::TimingControl;
}

void alignGroupTrailingComments(std::string& result, const std::vector<std::string_view>& lines,
                                const std::vector<LineInfo>& infos, GroupRange range) {
    size_t maxCommentCol = 0;
    size_t commentCount = 0;
    for (auto i = range.start; i < range.end; i++) {
        if (infos[i].kind == LineKind::Comment || infos[i].kind == LineKind::Empty) {
            continue;
        }

        auto commentPos = findTrailingComment(lines[i]);
        if (commentPos == npos) {
            continue;
        }

        commentCount++;

        auto codeEnd = trimTrailingSpaces(lines[i], commentPos);

        maxCommentCol = std::max(maxCommentCol, codeEnd + 1);
    }

    for (auto i = range.start; i < range.end; i++) {
        if (infos[i].kind == LineKind::Comment || infos[i].kind == LineKind::Empty) {
            result.append(lines[i]);
            result += '\n';
            continue;
        }

        auto commentPos = findTrailingComment(lines[i]);
        if (commentPos == npos || commentCount < 2) {
            result.append(lines[i]);
            result += '\n';
            continue;
        }

        auto codeEnd = trimTrailingSpaces(lines[i], commentPos);

        result.append(lines[i].substr(0, codeEnd));
        if (maxCommentCol > codeEnd) {
            result.append(maxCommentCol - codeEnd, ' ');
        }
        else {
            result += ' ';
        }
        result.append(lines[i].substr(commentPos));
        result += '\n';
    }
}

std::string padTimingIdent(std::string_view line, const LineInfo& info, size_t maxTypeWidth) {
    auto targetPos = info.indent + maxTypeWidth;
    auto typeTextEnd = trimTrailingSpaces(line, info.identPos);

    std::string p;
    p.append(line.substr(0, typeTextEnd));
    p.append(targetPos - (typeTextEnd - info.indent) - info.indent, ' ');
    p.append(line.substr(info.identPos));
    return p;
}

void alignTimingIdentsAndEquals(std::string& result, const std::vector<std::string_view>& lines,
                                const std::vector<LineInfo>& infos, GroupRange range,
                                size_t maxTypeWidth) {
    std::vector<std::string> padded;
    padded.reserve(range.end - range.start);
    size_t maxEqualsCol = 0;
    for (auto i = range.start; i < range.end; i++) {
        if (infos[i].kind != LineKind::TimingControl) {
            padded.emplace_back(lines[i]);
            continue;
        }

        padded.push_back(padTimingIdent(lines[i], infos[i], maxTypeWidth));
        auto shift = static_cast<ptrdiff_t>(infos[i].indent + maxTypeWidth) -
                     static_cast<ptrdiff_t>(infos[i].identPos);
        auto adjustedEquals = static_cast<size_t>(static_cast<ptrdiff_t>(infos[i].equalsPos) +
                                                  shift);
        maxEqualsCol = std::max(maxEqualsCol, adjustedEquals);
    }

    size_t idx = 0;
    for (auto i = range.start; i < range.end; i++, idx++) {
        if (infos[i].kind != LineKind::TimingControl) {
            result.append(padded[idx]);
            result += '\n';
            continue;
        }

        auto& p = padded[idx];
        auto shift = static_cast<ptrdiff_t>(infos[i].indent + maxTypeWidth) -
                     static_cast<ptrdiff_t>(infos[i].identPos);
        auto eqPos = static_cast<size_t>(static_cast<ptrdiff_t>(infos[i].equalsPos) + shift);
        auto preEqualsEnd = trimTrailingSpaces(std::string_view(p), eqPos);

        result.append(std::string_view(p).substr(0, preEqualsEnd));
        result.append(maxEqualsCol - preEqualsEnd, ' ');
        result.append(std::string_view(p).substr(eqPos));
        result += '\n';
    }
}

void alignGroupTiming(std::string& result, const std::vector<std::string_view>& lines,
                      const std::vector<LineInfo>& infos, GroupRange range) {
    size_t count = 0;
    size_t maxTypeWidth = 0;
    for (auto i = range.start; i < range.end; i++) {
        if (infos[i].kind != LineKind::TimingControl) {
            continue;
        }
        count++;
        maxTypeWidth = std::max(maxTypeWidth, infos[i].typeWidth);
    }

    if (count < 2) {
        for (auto i = range.start; i < range.end; i++) {
            result.append(lines[i]);
            result += '\n';
        }
        return;
    }

    alignTimingIdentsAndEquals(result, lines, infos, range, maxTypeWidth);
}

bool followsDeclaration(const std::vector<LineInfo>& infos, size_t i) {
    for (auto j = i; j > 0; j--) {
        auto prevKind = infos[j - 1].kind;
        if (prevKind == LineKind::Assignment || prevKind == LineKind::Declaration ||
            prevKind == LineKind::Continuation) {
            return true;
        }

        if (prevKind == LineKind::Empty || prevKind == LineKind::Comment) {
            continue;
        }

        break;
    }
    return false;
}

void reclassifyContinuationLines(const std::vector<std::string_view>& lines,
                                 std::vector<LineInfo>& infos) {
    for (size_t i = 1; i < infos.size(); i++) {
        if (infos[i].kind != LineKind::Other && infos[i].kind != LineKind::Assignment) {
            continue;
        }

        if (!followsDeclaration(infos, i) || !isContinuationLine(lines[i])) {
            continue;
        }

        auto indentEnd = lines[i].find_first_not_of(' ');
        infos[i] = {.kind = LineKind::Continuation,
                    .indent = indentEnd,
                    .typeWidth = 0,
                    .identPos = indentEnd,
                    .equalsPos = findEquals(lines[i], indentEnd),
                    .dimPos = npos,
                    .depth = infos[i].depth};
    }
}

std::vector<LineInfo> classifyLines(const std::vector<std::string_view>& lines,
                                    const std::vector<LineMetadata>& lineMetadata) {
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
        if (i < lineMetadata.size()) {
            info.depth = lineMetadata[i].depth;

            auto metaKind = toLineKind(lineMetadata[i].kind);
            if (!formatOff && metaKind != LineKind::Other) {
                info.kind = metaKind;
            }
        }
        infos.push_back(info);
    }

    reclassifyContinuationLines(lines, infos);
    return infos;
}

template<typename BreakPred, typename StartPred, typename AlignFn>
std::string applyAlignConsecutive(const std::string& output,
                                  const AlignConsecutiveStyle& alignStyle, BreakPred shouldBreak,
                                  StartPred canStart, AlignFn alignFn,
                                  const std::vector<LineMetadata>& lineMetadata = {}) {
    if (!alignStyle.Enabled) {
        return output;
    }

    auto const acrossEmpty = alignStyle.AcrossEmptyLines;
    auto const acrossComments = alignStyle.AcrossComments;
    auto const acrossIndent = alignStyle.AcrossParameterPortList;

    std::vector<std::string_view> lines;
    std::string_view remaining{output};
    while (!remaining.empty()) {
        auto nl = remaining.find('\n');
        if (nl == std::string_view::npos) {
            lines.push_back(remaining);
            remaining = {};
        }
        else {
            lines.push_back(remaining.substr(0, nl));
            remaining.remove_prefix(nl + 1);
        }
    }

    auto infos = classifyLines(lines, lineMetadata);

    std::string result;
    result.reserve(output.size());
    AlignState state;

    for (size_t i = 0; i < lines.size(); i++) {
        auto& info = infos[i];
        auto const breakGroup = shouldBreak(info, state, acrossEmpty, acrossComments, acrossIndent);

        if (breakGroup && state.inGroup) {
            alignFn(result, lines, infos, {.start = state.groupStart, .end = i});
            state.inGroup = false;
            state.groupIndent.reset();
            state.groupDepth.reset();
        }

        if (canStart(info) && !state.inGroup) {
            state.groupStart = i;
            state.inGroup = true;
            state.groupIndent = info.indent;
            state.groupDepth = info.depth;
        }
        else if (!state.inGroup) {
            result.append(lines[i]);
            result += '\n';
        }
    }

    if (state.inGroup) {
        alignFn(result, lines, infos, {.start = state.groupStart, .end = lines.size()});
    }

    if (!result.empty() && result.back() == '\n' && (output.empty() || output.back() != '\n')) {
        result.pop_back();
    }

    return result;
}

} // namespace

namespace slang::format {

std::string applyAlignment(const std::string& output, const Style& style,
                           const std::vector<LineMetadata>& lineMetadata) {
    auto dimAlignFn = [&style](std::string& r, const std::vector<std::string_view>& l,
                               const std::vector<LineInfo>& inf, GroupRange rng) {
        alignGroupDimensions(r, l, inf, rng, style.AlignConsecutivePackedDimensions);
    };
    auto result = applyAlignConsecutive(output, style.AlignConsecutivePackedDimensions,
                                        shouldBreakDimGroup, canStartDimGroup, dimAlignFn,
                                        lineMetadata);
    result = applyAlignConsecutive(result, style.AlignConsecutiveDeclarations, shouldBreakGroup,
                                   canStartDeclGroup, alignGroup, lineMetadata);
    result = applyAlignConsecutive(result, style.AlignConsecutiveTimingControls,
                                   shouldBreakTimingGroup, canStartTimingGroup, alignGroupTiming,
                                   lineMetadata);
    result = applyAlignConsecutive(result, style.AlignConsecutiveAssignments,
                                   shouldBreakAssignGroup, canStartAssignGroup, alignGroupEquals,
                                   lineMetadata);
    result = applyAlignConsecutive(result, style.AlignTrailingComments,
                                   shouldBreakTrailingCommentGroup, canStartTrailingCommentGroup,
                                   alignGroupTrailingComments, lineMetadata);
    return result;
}

} // namespace slang::format
