// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Align.h"
#include "Style.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <optional>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace slang::format;

namespace {

using Kind = LineMetadata::Kind;

constexpr auto npos = std::string_view::npos;

struct LineInfo {
    Kind kind;
    size_t indent = 0;
    size_t keywordWidth = 0;   ///< Width of direction/parameter keyword, or 0 if absent.
    size_t typeWidth = 0;      ///< Distance from indent to identifier start.
    size_t identPos = 0;       ///< Absolute position of identifier in the line.
    size_t equalsPos = npos;   ///< Position of '=' in declaration, or npos if absent.
    size_t dimPos = npos;      ///< Position of first '[' after type keyword, or npos if absent.
    unsigned depth = 0;        ///< AST nesting depth at the line's emission point.
    unsigned maxExcess = 0;    ///< Maximum source-vs-output spacing excess on this line.
    bool metadataKind = false; ///< True when kind was set from tree-walk metadata.
};

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
         c == '^' || c == ':') &&
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

struct GroupRange {
    size_t start;
    size_t end;
};

struct DeclGroupMetrics {
    size_t maxKeywordWidth = 0;
    size_t maxTypeWidth = 0;
    size_t maxIdentToEquals = 0;
    bool hasEquals = false;
};

void alignGroupDeclLine(std::string& result, std::string_view line, const LineInfo& info,
                        const DeclGroupMetrics& metrics) {
    auto indent = info.indent;
    auto identPos = info.identPos;
    auto targetPos = indent + metrics.maxTypeWidth;

    if (metrics.maxKeywordWidth > 0 && info.keywordWidth > 0) {
        auto keywordEnd = indent + info.keywordWidth;
        result.append(line.substr(0, keywordEnd));
        result.append(metrics.maxKeywordWidth - info.keywordWidth + 1, ' ');

        auto typeBodyStart = skipSpaces(line, keywordEnd);
        auto typeTextEnd = trimTrailingSpaces(line, identPos);
        if (typeBodyStart < typeTextEnd) {
            result.append(line.substr(typeBodyStart, typeTextEnd - typeBodyStart));
        }
        result.append(targetPos - (metrics.maxKeywordWidth + 1) -
                          (typeTextEnd > typeBodyStart ? typeTextEnd - typeBodyStart : 0) - indent,
                      ' ');
    }
    else {
        auto typeTextEnd = trimTrailingSpaces(line, identPos);
        result.append(line.substr(0, typeTextEnd));
        result.append(targetPos - (typeTextEnd - indent) - indent, ' ');
    }

    if (metrics.hasEquals && info.equalsPos != npos) {
        auto eqPos = info.equalsPos;
        auto preEqualsEnd = trimTrailingSpaces(line, eqPos);
        auto padding = metrics.maxIdentToEquals - (eqPos - identPos);

        result.append(line.substr(identPos, preEqualsEnd - identPos));
        result.append(padding + (eqPos - preEqualsEnd), ' ');
        result.append(line.substr(eqPos));
    }
    else {
        result.append(line.substr(identPos));
    }
}

enum class AlignPolicy { Align, FlushLeft };

/// Context passed to alignment functions for InferFromSource decisions.
struct InferContext {
    bool enabled = false;
    bool minGuarantee = false;
    unsigned maxPadding = 0;
    unsigned minPadding = 0;
    unsigned maxDisplacement = 0;
    unsigned groupMaxExcess = 0;
};

AlignPolicy inferAlignPolicy(unsigned displacement, const InferContext& ctx) {
    static constexpr unsigned minGuaranteeThreshold = 2;

    if (ctx.enabled) {
        if (ctx.minGuarantee && displacement > 0 && displacement <= minGuaranteeThreshold) {
            return AlignPolicy::Align;
        }

        if (ctx.minPadding > 0 && displacement > 0 && displacement <= ctx.minPadding) {
            return AlignPolicy::Align;
        }

        if (ctx.maxDisplacement > 0 && displacement > ctx.maxDisplacement) {
            return AlignPolicy::FlushLeft;
        }

        if (ctx.groupMaxExcess <= 2) {
            return AlignPolicy::FlushLeft;
        }

        return AlignPolicy::Align;
    }

    return (ctx.maxPadding == 0 || displacement <= ctx.maxPadding) ? AlignPolicy::Align
                                                                   : AlignPolicy::FlushLeft;
}

unsigned computeGroupMaxExcess(const std::vector<LineMetadata>& lineMetadata,
                               const std::unordered_map<size_t, size_t>& metaByLine, size_t start,
                               size_t end) {
    unsigned maxExcess = 0;
    for (auto i = start; i < end; i++) {
        auto it = metaByLine.find(i);
        if (it == metaByLine.end()) {
            continue;
        }

        for (const auto& ts : lineMetadata[it->second].tokenSpacings) {
            if (ts.sourceSpaces > ts.outputSpaces) {
                maxExcess = std::max(maxExcess, ts.sourceSpaces - ts.outputSpaces);
            }
        }
    }
    return maxExcess;
}

size_t typeBodyWidth(const LineInfo& info) {
    auto w = info.typeWidth;
    if (info.keywordWidth > 0) {
        w -= info.keywordWidth + 1;
    }
    return w;
}

size_t typeNameWidth(const LineInfo& info) {
    auto start = info.indent + (info.keywordWidth > 0 ? info.keywordWidth + 1 : 0);
    auto end = info.dimPos != npos ? info.dimPos : info.identPos;
    return end > start ? end - start : 0;
}

struct DeclGroupSummary {
    DeclGroupMetrics metrics;
    size_t declCount = 0;
    size_t displacement = 0;
    bool hasKeywords = false;
    std::optional<size_t> groupIndent;
};

DeclGroupSummary computeDeclGroupSummary(const std::vector<LineInfo>& infos, GroupRange range) {
    DeclGroupSummary summary;
    size_t minNameWidth = npos;
    size_t maxNameWidth = 0;
    size_t maxBodyWidth = 0;
    for (size_t i = range.start; i < range.end; i++) {
        if (infos[i].kind == Kind::Declaration) {
            summary.declCount++;
            if (infos[i].keywordWidth > 0) {
                summary.hasKeywords = true;
            }
            summary.metrics.maxKeywordWidth = std::max(summary.metrics.maxKeywordWidth,
                                                       infos[i].keywordWidth);
            summary.metrics.maxTypeWidth = std::max(summary.metrics.maxTypeWidth,
                                                    infos[i].typeWidth);
            if (!summary.groupIndent) {
                summary.groupIndent = infos[i].indent;
            }
            if (infos[i].equalsPos != npos) {
                summary.metrics.hasEquals = true;
                summary.metrics.maxIdentToEquals = std::max(summary.metrics.maxIdentToEquals,
                                                            infos[i].equalsPos - infos[i].identPos);
            }

            auto nw = typeNameWidth(infos[i]);
            maxNameWidth = std::max(maxNameWidth, nw);
            minNameWidth = std::min(minNameWidth, nw);
            maxBodyWidth = std::max(maxBodyWidth, typeBodyWidth(infos[i]));
        }
        else if (infos[i].kind == Kind::Continuation) {
            summary.declCount++;
        }
    }

    if (summary.metrics.maxKeywordWidth > 0 &&
        summary.metrics.maxKeywordWidth + 1 + maxBodyWidth > summary.metrics.maxTypeWidth) {
        summary.metrics.maxKeywordWidth = 0;
    }

    summary.displacement = (minNameWidth != npos) ? maxNameWidth - minNameWidth : 0;
    return summary;
}

void alignGroup(std::string& result, const std::vector<std::string_view>& lines,
                const std::vector<LineInfo>& infos, GroupRange range, const InferContext& ctx) {
    auto [metrics, declCount, displacement, hasKeywords,
          groupIndent] = computeDeclGroupSummary(infos, range);

    auto declCtx = ctx;
    if (!hasKeywords) {
        declCtx.minGuarantee = true;
    }
    auto policy = declCount >= 2 ? inferAlignPolicy(static_cast<unsigned>(displacement), declCtx)
                                 : AlignPolicy::FlushLeft;

    auto targetAbsCol = groupIndent.value_or(0) + metrics.maxTypeWidth;

    for (size_t i = range.start; i < range.end; i++) {
        if (infos[i].kind == Kind::Declaration && policy == AlignPolicy::Align &&
            infos[i].identPos > infos[i].indent) {
            alignGroupDeclLine(result, lines[i], infos[i], metrics);
        }
        else if (infos[i].kind == Kind::Continuation && policy == AlignPolicy::Align) {
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

struct GroupBreakConfig {
    std::span<const Kind> memberKinds;
    bool continuationPassThrough = true;
    bool depthScoped = false;
    bool indentAlwaysBreaks = false;
    bool portListAware = true;
};

bool isMemberKind(Kind kind, std::span<const Kind> memberKinds) {
    return std::ranges::find(memberKinds, kind) != memberKinds.end();
}

bool shouldBreakGroup(const LineInfo& info, const AlignState& state, bool acrossEmpty,
                      bool acrossComments, bool acrossIndent, const GroupBreakConfig& config) {
    if (info.kind == Kind::Continuation && config.continuationPassThrough) {
        return false;
    }

    if (isMemberKind(info.kind, config.memberKinds) ||
        (info.kind == Kind::Continuation && !config.continuationPassThrough)) {
        if (config.depthScoped && state.inGroup && state.groupDepth &&
            *state.groupDepth != info.depth) {
            return false;
        }

        if (config.indentAlwaysBreaks) {
            return state.inGroup && state.groupIndent && *state.groupIndent != info.indent;
        }

        return state.inGroup && !acrossIndent && state.groupIndent &&
               *state.groupIndent != info.indent;
    }

    if (info.kind == Kind::Empty) {
        return state.inGroup && !acrossEmpty;
    }

    if (info.kind == Kind::Comment) {
        return state.inGroup && !acrossComments;
    }

    if (info.kind == Kind::PortListBoundary && config.portListAware) {
        return state.inGroup && !acrossIndent;
    }

    if (config.depthScoped) {
        return state.inGroup && state.groupDepth && info.depth < *state.groupDepth;
    }

    return true;
}

bool canStartGroup(const LineInfo& info, const GroupBreakConfig& config) {
    return isMemberKind(info.kind, config.memberKinds);
}

bool shouldBreakDimGroup(const LineInfo& info, const AlignState& state, bool acrossEmpty,
                         bool acrossComments, bool acrossIndent, const GroupBreakConfig& config) {
    if ((info.kind == Kind::Declaration || info.kind == Kind::Continuation) &&
        info.dimPos == npos) {
        return false;
    }
    return shouldBreakGroup(info, state, acrossEmpty, acrossComments, acrossIndent, config);
}

struct DimContent {
    std::string_view full;
    std::string_view left;
    std::string_view right;
    bool hasColon;
    bool hasColonSpaces;
    size_t openPos;
    size_t closePos;
};

struct DimWidths {
    size_t maxFullWidth = 0;
    size_t maxLeftWidth = 0;
    size_t maxRightWidth = 0;
};

DimContent makeDimContent(std::string_view full, size_t openPos, size_t closePos) {
    auto colonPos = full.find(':');
    if (colonPos == npos) {
        return {.full = full,
                .left = full,
                .right = {},
                .hasColon = false,
                .hasColonSpaces = false,
                .openPos = openPos,
                .closePos = closePos};
    }

    auto left = full.substr(0, colonPos);
    auto right = full.substr(colonPos + 1);
    auto hasSpaces = left.ends_with(' ') || right.starts_with(' ');

    while (left.ends_with(' ')) {
        left.remove_suffix(1);
    }

    while (right.starts_with(' ')) {
        right.remove_prefix(1);
    }

    return {.full = full,
            .left = left,
            .right = right,
            .hasColon = true,
            .hasColonSpaces = hasSpaces,
            .openPos = openPos,
            .closePos = closePos};
}

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
        dims.push_back(makeDimContent(full, openPos, closePos));
    }
    return dims;
}

void emitDimContent(std::string& result, const DimContent& dim, const DimWidths& widths,
                    const AlignConsecutiveStyle& alignStyle, RangeSpacingStyle rangeSpacing) {
    auto emitColon = [&](std::string& out) {
        switch (rangeSpacing) {
            case RangeSpacingStyle::Always:
                out += " : ";
                break;
            case RangeSpacingStyle::Never:
                out += ':';
                break;
            case RangeSpacingStyle::MatchSource:
            case RangeSpacingStyle::Preserve:
                if (dim.hasColonSpaces) {
                    out += " : ";
                }
                else {
                    out += ':';
                }
                break;
        }
    };

    if (alignStyle.AlignColon && dim.hasColon) {
        result.append(widths.maxLeftWidth - dim.left.size(), ' ');
        result.append(dim.left);
        emitColon(result);

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
    else if (dim.hasColon) {
        auto emitFull = [&](std::string& out) {
            out.append(dim.left);
            emitColon(out);
            out.append(dim.right);
        };

        if (alignStyle.PadLeft) {
            std::string full;
            emitFull(full);
            result.append(widths.maxFullWidth - full.size(), ' ');
            result.append(full);
        }
        else if (alignStyle.PadRight) {
            auto start = result.size();
            emitFull(result);
            result.append(widths.maxFullWidth - (result.size() - start), ' ');
        }
        else {
            emitFull(result);
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
                       const AlignConsecutiveStyle& alignStyle, RangeSpacingStyle rangeSpacing) {
    size_t lastClose = npos;
    for (size_t j = 0; j < dims.size(); j++) {
        if (j > 0 && dims[j].openPos > lastClose + 1) {
            result.append(dims[j].openPos - lastClose - 1, ' ');
        }

        result += '[';
        emitDimContent(result, dims[j], slotWidths[j], alignStyle, rangeSpacing);
        result += ']';
        lastClose = dims[j].closePos;
    }

    auto origLastClose = dims.back().closePos;
    if (origLastClose + 1 < line.size()) {
        result.append(line.substr(origLastClose + 1));
    }
}

size_t colonWidth(const DimContent& dim, RangeSpacingStyle rangeSpacing) {
    if (!dim.hasColon) {
        return 0;
    }

    switch (rangeSpacing) {
        case RangeSpacingStyle::Always:
            return 3;
        case RangeSpacingStyle::Never:
            return 1;
        case RangeSpacingStyle::MatchSource:
        case RangeSpacingStyle::Preserve:
            return dim.hasColonSpaces ? 3 : 1;
    }
    return 1;
}

std::vector<DimWidths> computeSlotWidths(const std::vector<std::vector<DimContent>>& allDims,
                                         RangeSpacingStyle rangeSpacing) {
    size_t maxSlots = 0;
    for (const auto& dims : allDims) {
        maxSlots = std::max(maxSlots, dims.size());
    }

    std::vector<DimWidths> slotWidths(maxSlots);
    for (const auto& dims : allDims) {
        for (size_t j = 0; j < dims.size(); j++) {
            auto fullWidth = dims[j].left.size() + dims[j].right.size() +
                             colonWidth(dims[j], rangeSpacing);
            slotWidths[j].maxFullWidth = std::max(slotWidths[j].maxFullWidth, fullWidth);

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
        if (infos[i].kind == Kind::Declaration && infos[i].dimPos != npos) {
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
                          const AlignConsecutiveStyle& alignStyle, RangeSpacingStyle rangeSpacing) {
    size_t dimCount = 0;
    size_t maxDimCol = 0;
    for (size_t i = range.start; i < range.end; i++) {
        if (infos[i].kind == Kind::Declaration && infos[i].dimPos != npos) {
            dimCount++;
            maxDimCol = std::max(maxDimCol, infos[i].dimPos);
        }
        else if (infos[i].kind == Kind::Declaration) {
            maxDimCol = std::max(maxDimCol, infos[i].identPos);
        }
    }

    auto const contentAlign = alignStyle.AlignColon || alignStyle.PadLeft || alignStyle.PadRight;

    std::vector<std::vector<DimContent>> allDims;
    std::vector<DimWidths> slotWidths;
    if (contentAlign) {
        allDims = collectDimContents(lines, infos, range);
        slotWidths = computeSlotWidths(allDims, rangeSpacing);
    }

    size_t dimIdx = 0;
    for (size_t i = range.start; i < range.end; i++) {
        if (infos[i].kind == Kind::Declaration && infos[i].dimPos != npos) {
            auto line = lines[i];
            auto dp = infos[i].dimPos;

            auto preDimEnd = trimTrailingSpaces(line, dp);

            result.append(line.substr(0, preDimEnd));
            result.append(maxDimCol - preDimEnd, ' ');

            if (contentAlign && dimIdx < allDims.size() && !allDims[dimIdx].empty()) {
                emitRewrittenDims(result, line, allDims[dimIdx], slotWidths, alignStyle,
                                  rangeSpacing);
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
    return info.kind == Kind::Assignment || info.kind == Kind::Declaration ||
           info.kind == Kind::Continuation || info.kind == Kind::DistItem;
}

/// Returns true if any line in \p range (excluding the first) has a depth strictly less than
/// \p threshold, ignoring port list boundaries which are structural delimiters already handled by
/// group formation.
bool hasDepthDropBelow(const std::vector<LineInfo>& infos, GroupRange range, unsigned threshold) {
    for (auto i = range.start + 1; i < range.end; i++) {
        if (infos[i].kind == Kind::PortListBoundary) {
            continue;
        }
        if (infos[i].depth < threshold) {
            return true;
        }
    }
    return false;
}

bool hasFlatGap(const std::vector<LineInfo>& infos, GroupRange range, unsigned depth) {
    bool hasGap = false;
    for (auto i = range.start + 1; i < range.end; i++) {
        if (infos[i].kind == Kind::Other && infos[i].depth == depth) {
            hasGap = true;
        }
        else if (infos[i].depth > depth) {
            return false;
        }
    }
    return hasGap;
}

/// Assigns a sub-run index to each alignable line in the range, per depth. Consecutive assignments
/// at depth D share a sub-run unless an intervening line has depth < D or a flat gap of
/// non-alignable lines separates them at depth D.
std::vector<size_t> assignSubRuns(const std::vector<LineInfo>& infos, GroupRange range) {
    std::vector<size_t> subRun(range.end - range.start, 0);
    std::vector<size_t> lastIdxAtDepth;
    std::vector<size_t> curSubRunAtDepth;
    std::vector<unsigned> depthValues;

    for (auto i = range.start; i < range.end; i++) {
        if (!isAlignableKind(infos[i]) || infos[i].equalsPos == npos) {
            continue;
        }

        auto d = infos[i].depth;
        auto it = std::ranges::find(depthValues, d);
        size_t dIdx = 0;
        if (it == depthValues.end()) {
            dIdx = depthValues.size();
            depthValues.push_back(d);
            lastIdxAtDepth.push_back(i);
            curSubRunAtDepth.push_back(0);
        }
        else {
            dIdx = static_cast<size_t>(it - depthValues.begin());
            auto gap = GroupRange{.start = lastIdxAtDepth[dIdx], .end = i};
            if (hasDepthDropBelow(infos, gap, d) || hasFlatGap(infos, gap, d)) {
                curSubRunAtDepth[dIdx]++;
            }
            lastIdxAtDepth[dIdx] = i;
        }
        subRun[i - range.start] = curSubRunAtDepth[dIdx];
    }
    return subRun;
}

void alignGroupEquals(std::string& result, const std::vector<std::string_view>& lines,
                      const std::vector<LineInfo>& infos, GroupRange range,
                      const InferContext& ctx) {
    auto subRuns = assignSubRuns(infos, range);

    struct AlignBucket {
        unsigned depth;
        size_t subRun;
        size_t maxEqualsCol = 0;
        size_t minEqualsCol = npos;
        size_t count = 0;
        unsigned maxExcess = 0;
    };
    std::vector<AlignBucket> buckets;

    auto findBucket = [&](unsigned depth, size_t subRun) -> size_t {
        for (size_t j = 0; j < buckets.size(); j++) {
            if (buckets[j].depth == depth && buckets[j].subRun == subRun) {
                return j;
            }
        }
        buckets.push_back({.depth = depth, .subRun = subRun});
        return buckets.size() - 1;
    };

    for (auto i = range.start; i < range.end; i++) {
        if (!isAlignableKind(infos[i]) || infos[i].equalsPos == npos) {
            continue;
        }

        auto b = findBucket(infos[i].depth, subRuns[i - range.start]);
        buckets[b].count++;
        buckets[b].maxEqualsCol = std::max(buckets[b].maxEqualsCol, infos[i].equalsPos);
        buckets[b].minEqualsCol = std::min(buckets[b].minEqualsCol, infos[i].equalsPos);
        buckets[b].maxExcess = std::max(buckets[b].maxExcess, infos[i].maxExcess);
    }

    for (auto i = range.start; i < range.end; i++) {
        if (!isAlignableKind(infos[i]) || infos[i].equalsPos == npos) {
            result.append(lines[i]);
            result += '\n';
            continue;
        }

        auto b = findBucket(infos[i].depth, subRuns[i - range.start]);
        auto padding = buckets[b].maxEqualsCol - buckets[b].minEqualsCol;
        auto bucketCtx = ctx;
        bucketCtx.groupMaxExcess = buckets[b].maxExcess;
        auto policy = buckets[b].count >= 2 ? inferAlignPolicy(padding, bucketCtx)
                                            : AlignPolicy::FlushLeft;
        if (policy == AlignPolicy::Align) {
            auto line = lines[i];
            auto eqPos = infos[i].equalsPos;

            auto preEqualsEnd = trimTrailingSpaces(line, eqPos);

            result.append(line.substr(0, preEqualsEnd));
            result.append(buckets[b].maxEqualsCol - preEqualsEnd, ' ');
            result.append(line.substr(eqPos));
        }
        else {
            result.append(lines[i]);
        }
        result += '\n';
    }
}

void alignGroupPortConnections(std::string& result, const std::vector<std::string_view>& lines,
                               const std::vector<LineInfo>& infos, GroupRange range,
                               const InferContext& ctx) {
    size_t maxParenCol = 0;
    size_t minParenCol = npos;
    size_t connCount = 0;
    for (auto i = range.start; i < range.end; i++) {
        if (infos[i].kind == Kind::PortConnection && infos[i].equalsPos != npos) {
            connCount++;
            maxParenCol = std::max(maxParenCol, infos[i].equalsPos);
            minParenCol = std::min(minParenCol, infos[i].equalsPos);
        }
    }

    for (auto i = range.start; i < range.end; i++) {
        if (infos[i].kind == Kind::PortConnection && infos[i].equalsPos != npos) {
            auto padding = maxParenCol - minParenCol;
            auto policy = connCount >= 2 ? inferAlignPolicy(padding, ctx) : AlignPolicy::FlushLeft;
            if (policy == AlignPolicy::Align) {
                auto line = lines[i];
                auto parenPos = infos[i].equalsPos;
                auto preParenEnd = trimTrailingSpaces(line, parenPos);

                result.append(line.substr(0, preParenEnd));
                result.append(maxParenCol - preParenEnd, ' ');
                result.append(line.substr(parenPos));
            }
            else {
                result.append(lines[i]);
            }
        }
        else {
            result.append(lines[i]);
        }
        result += '\n';
    }
}

void alignGroupTrailingComments(std::string& result, const std::vector<std::string_view>& lines,
                                const std::vector<LineInfo>& infos, GroupRange range,
                                unsigned minSpaces) {
    size_t maxCommentCol = 0;
    size_t commentCount = 0;
    for (auto i = range.start; i < range.end; i++) {
        if (infos[i].kind == Kind::Comment || infos[i].kind == Kind::Empty) {
            continue;
        }

        auto commentPos = findTrailingComment(lines[i]);
        if (commentPos == npos) {
            continue;
        }

        commentCount++;

        auto codeEnd = trimTrailingSpaces(lines[i], commentPos);

        maxCommentCol = std::max(maxCommentCol, codeEnd + minSpaces);
    }

    for (auto i = range.start; i < range.end; i++) {
        if (infos[i].kind == Kind::Comment || infos[i].kind == Kind::Empty) {
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
            result.append(minSpaces, ' ');
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
        if (infos[i].kind != Kind::TimingControl) {
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
        if (infos[i].kind != Kind::TimingControl) {
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
                      const std::vector<LineInfo>& infos, GroupRange range,
                      const InferContext& /*ctx*/) {
    size_t count = 0;
    size_t maxTypeWidth = 0;
    for (auto i = range.start; i < range.end; i++) {
        if (infos[i].kind != Kind::TimingControl) {
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

    for (const auto& ts : meta.tokenSpacings) {
        if (ts.sourceSpaces > ts.outputSpaces) {
            info.maxExcess = std::max(info.maxExcess,
                                      static_cast<unsigned>(ts.sourceSpaces - ts.outputSpaces));
        }
    }

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

std::vector<LineInfo> classifyLines(const std::vector<std::string_view>& lines,
                                    const std::vector<LineMetadata>& lineMetadata) {
    std::unordered_map<size_t, size_t> metaByLine;
    for (size_t j = 0; j < lineMetadata.size(); j++) {
        if (lineMetadata[j].lineNumber != npos) {
            metaByLine[lineMetadata[j].lineNumber] = j;
        }
    }

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

using BreakPred = bool (*)(const LineInfo&, const AlignState&, bool, bool, bool,
                           const GroupBreakConfig&);

template<typename AlignFn>
std::string applyAlignConsecutive(const std::string& output,
                                  const AlignConsecutiveStyle& alignStyle,
                                  const GroupBreakConfig& config, BreakPred breakPred,
                                  AlignFn alignFn,
                                  const std::vector<LineMetadata>& lineMetadata = {},
                                  unsigned continuationIndentWidth = 4, bool minGuarantee = false) {
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

    std::unordered_map<size_t, size_t> metaByLine;
    for (size_t j = 0; j < lineMetadata.size(); j++) {
        if (lineMetadata[j].lineNumber != npos) {
            metaByLine[lineMetadata[j].lineNumber] = j;
        }
    }

    auto makeCtx = [&](size_t start, size_t end) {
        return InferContext{.enabled = alignStyle.InferFromSource,
                            .minGuarantee = minGuarantee,
                            .maxPadding = alignStyle.MaxPadding,
                            .minPadding = alignStyle.MinPadding,
                            .maxDisplacement = alignStyle.MaxPadding * continuationIndentWidth,
                            .groupMaxExcess = computeGroupMaxExcess(lineMetadata, metaByLine, start,
                                                                    end)};
    };

    std::string result;
    result.reserve(output.size());
    AlignState state;

    for (size_t i = 0; i < lines.size(); i++) {
        auto& info = infos[i];
        auto const breakGroup = breakPred(info, state, acrossEmpty, acrossComments, acrossIndent,
                                          config);

        if (breakGroup && state.inGroup) {
            auto ctx = makeCtx(state.groupStart, i);
            alignFn(result, lines, infos, {.start = state.groupStart, .end = i}, ctx);
            state.inGroup = false;
            state.groupIndent.reset();
            state.groupDepth.reset();
        }

        if (canStartGroup(info, config) && !state.inGroup) {
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
        auto ctx = makeCtx(state.groupStart, lines.size());
        alignFn(result, lines, infos, {.start = state.groupStart, .end = lines.size()}, ctx);
    }

    if (!result.empty() && result.back() == '\n' && (output.empty() || output.back() != '\n')) {
        result.pop_back();
    }

    return result;
}

bool hasMultiSpaceRange(const LineMetadata& meta) {
    return std::ranges::any_of(meta.rangeSpacings, [](const auto& rs) {
        return rs.sourceLeftSpaces > 1 || rs.sourceRightSpaces > 1;
    });
}

unsigned computeMetaExcess(const LineMetadata& meta) {
    unsigned excess = 0;
    for (const auto& ts : meta.tokenSpacings) {
        if (ts.sourceSpaces > ts.outputSpaces) {
            excess = std::max(excess, ts.sourceSpaces - ts.outputSpaces);
        }
    }
    return excess;
}

std::set<size_t> computePortListAdjacentLines(const std::vector<LineMetadata>& lineMetadata) {
    std::set<size_t> adjacentLines;
    unsigned groupExcess = 0;
    unsigned boundaryCount = 0;

    for (const auto& meta : lineMetadata) {
        if (meta.kind == Kind::PortListBoundary) {
            if (boundaryCount == 0 && groupExcess >= 2) {
                boundaryCount = 1;
            }
            else if (boundaryCount == 1) {
                boundaryCount = 2;
            }
            else {
                boundaryCount = 0;
            }

            groupExcess = 0;
            continue;
        }

        if (meta.kind == Kind::Empty) {
            boundaryCount = 0;
            groupExcess = 0;
            continue;
        }

        if (meta.kind == Kind::Comment) {
            continue;
        }

        if (boundaryCount >= 1) {
            if (hasMultiSpaceRange(meta)) {
                adjacentLines.insert(meta.lineNumber);
            }
            continue;
        }

        groupExcess = std::max(groupExcess, computeMetaExcess(meta));
    }

    return adjacentLines;
}

std::string normalizeBracketRange(std::string_view content) {
    auto colonPos = content.find(':');
    if (colonPos == npos) {
        return std::string{content};
    }

    auto left = content.substr(0, colonPos);
    auto right = content.substr(colonPos + 1);

    auto const hadSpaces = (colonPos > 0 && content[colonPos - 1] == ' ') ||
                           (colonPos + 1 < content.size() && content[colonPos + 1] == ' ');

    while (left.ends_with(' ')) {
        left.remove_suffix(1);
    }

    while (right.starts_with(' ')) {
        right.remove_prefix(1);
    }

    std::string result;
    result.append(left);
    result += hadSpaces ? " : " : ":";
    result.append(right);
    return result;
}

std::string normalizeLineRanges(std::string_view line) {
    std::string normalized;
    normalized.reserve(line.size());
    size_t pos = 0;
    while (pos < line.size()) {
        if (line[pos] == '[') {
            auto closePos = line.find(']', pos);
            if (closePos != npos) {
                auto content = line.substr(pos + 1, closePos - pos - 1);
                if (content.find(':') != npos) {
                    normalized += '[';
                    normalized += normalizeBracketRange(content);
                    normalized += ']';
                    pos = closePos + 1;
                    continue;
                }
            }
        }

        normalized += line[pos];
        pos++;
    }
    return normalized;
}

std::string normalizeRangeSpacing(const std::string& output,
                                  const std::vector<LineMetadata>& lineMetadata,
                                  RangeSpacingStyle rangeSpacing) {
    if (rangeSpacing != RangeSpacingStyle::MatchSource) {
        return output;
    }

    auto const adjacentLines = computePortListAdjacentLines(lineMetadata);

    std::unordered_map<size_t, size_t> metaByLine;
    for (size_t j = 0; j < lineMetadata.size(); j++) {
        if (lineMetadata[j].lineNumber != npos) {
            metaByLine[lineMetadata[j].lineNumber] = j;
        }
    }

    std::string result;
    result.reserve(output.size());

    std::string_view remaining{output};
    size_t lineNum = 0;
    while (!remaining.empty()) {
        auto nl = remaining.find('\n');
        auto line = (nl == npos) ? remaining : remaining.substr(0, nl);

        auto shouldNormalize = false;
        auto it = metaByLine.find(lineNum);
        if (it != metaByLine.end()) {
            const auto& meta = lineMetadata[it->second];
            shouldNormalize = hasMultiSpaceRange(meta) && !adjacentLines.contains(lineNum);
        }

        result.append(shouldNormalize ? normalizeLineRanges(line) : line);

        if (nl != npos) {
            result += '\n';
            remaining.remove_prefix(nl + 1);
        }
        else {
            remaining = {};
        }

        lineNum++;
    }

    return result;
}

} // namespace

namespace slang::format {

std::string applyAlignment(const std::string& output, const Style& style,
                           const std::vector<LineMetadata>& lineMetadata) {
    static constexpr std::array declKinds{Kind::Declaration};
    static constexpr std::array assignKinds{Kind::Assignment, Kind::Declaration, Kind::DistItem};
    static constexpr std::array portConnectionKinds{Kind::PortConnection};
    static constexpr std::array timingKinds{Kind::TimingControl};
    static constexpr std::array trailingKinds{Kind::Assignment, Kind::Declaration,
                                              Kind::TimingControl};

    static constexpr GroupBreakConfig declConfig{.memberKinds = declKinds};
    static constexpr GroupBreakConfig dimConfig{.memberKinds = declKinds};
    static constexpr GroupBreakConfig assignConfig{.memberKinds = assignKinds, .depthScoped = true};
    static constexpr GroupBreakConfig portConnectionConfig{.memberKinds = portConnectionKinds,
                                                           .depthScoped = true};
    static constexpr GroupBreakConfig timingConfig{.memberKinds = timingKinds,
                                                   .indentAlwaysBreaks = true,
                                                   .portListAware = false};
    static constexpr GroupBreakConfig trailingConfig{.memberKinds = trailingKinds,
                                                     .continuationPassThrough = false};

    auto normalized = (style.AlignConsecutivePackedDimensions.InferFromSource)
                          ? normalizeRangeSpacing(output, lineMetadata, style.RangeSpacing)
                          : output;

    auto dimAlignFn = [&style](std::string& r, const std::vector<std::string_view>& l,
                               const std::vector<LineInfo>& inf, GroupRange rng,
                               const InferContext& ctx) {
        size_t maxDimCol = 0;
        size_t minDimCol = npos;
        for (size_t i = rng.start; i < rng.end; i++) {
            if (inf[i].kind == Kind::Declaration && inf[i].dimPos != npos) {
                maxDimCol = std::max(maxDimCol, inf[i].dimPos);
                minDimCol = std::min(minDimCol, inf[i].dimPos);
            }
        }
        auto displacement = (minDimCol != npos) ? maxDimCol - minDimCol : size_t{0};
        auto policy = inferAlignPolicy(static_cast<unsigned>(displacement), ctx);
        if (policy == AlignPolicy::FlushLeft) {
            for (size_t i = rng.start; i < rng.end; i++) {
                r.append(l[i]);
                r += '\n';
            }
            return;
        }
        alignGroupDimensions(r, l, inf, rng, style.AlignConsecutivePackedDimensions,
                             style.RangeSpacing);
    };
    auto result = applyAlignConsecutive(normalized, style.AlignConsecutivePackedDimensions,
                                        dimConfig, shouldBreakDimGroup, dimAlignFn, lineMetadata,
                                        style.ContinuationIndentWidth);
    result = applyAlignConsecutive(result, style.AlignConsecutiveDeclarations, declConfig,
                                   shouldBreakGroup, alignGroup, lineMetadata,
                                   style.ContinuationIndentWidth);
    result = applyAlignConsecutive(result, style.AlignConsecutiveTimingControls, timingConfig,
                                   shouldBreakGroup, alignGroupTiming, lineMetadata,
                                   style.ContinuationIndentWidth);
    auto assignAlignFn = [](std::string& r, const std::vector<std::string_view>& l,
                            const std::vector<LineInfo>& inf, GroupRange rng,
                            const InferContext& ctx) { alignGroupEquals(r, l, inf, rng, ctx); };
    result = applyAlignConsecutive(result, style.AlignConsecutiveAssignments, assignConfig,
                                   shouldBreakGroup, assignAlignFn, lineMetadata,
                                   style.ContinuationIndentWidth);
    auto portConnAlignFn = [](std::string& r, const std::vector<std::string_view>& l,
                              const std::vector<LineInfo>& inf, GroupRange rng,
                              const InferContext& ctx) {
        alignGroupPortConnections(r, l, inf, rng, ctx);
    };
    result = applyAlignConsecutive(result, style.AlignConsecutivePortConnections,
                                   portConnectionConfig, shouldBreakGroup, portConnAlignFn,
                                   lineMetadata, style.ContinuationIndentWidth);
    auto trailingAlignFn = [&style](std::string& r, const std::vector<std::string_view>& l,
                                    const std::vector<LineInfo>& inf, GroupRange rng,
                                    const InferContext& /*ctx*/) {
        alignGroupTrailingComments(r, l, inf, rng, style.SpacesBeforeTrailingComments);
    };
    result = applyAlignConsecutive(result, style.AlignTrailingComments, trailingConfig,
                                   shouldBreakGroup, trailingAlignFn, lineMetadata,
                                   style.ContinuationIndentWidth);

    return result;
}

} // namespace slang::format
