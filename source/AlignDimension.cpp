// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "AlignDimension.h"
#include "Align.h"
#include "AlignClassifier.h"
#include "AlignGroup.h"
#include "AlignInference.h"
#include "slang/format/Style.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace slang::format;

namespace {

using Kind = LineMetadata::Kind;

constexpr auto npos = std::string_view::npos;

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
    size_t maxDimCol = 0;
    for (size_t i = range.start; i < range.end; i++) {
        if (infos[i].kind == Kind::Declaration && infos[i].dimPos != npos) {
            maxDimCol = std::max(maxDimCol, infos[i].dimPos);
        }
        else if (infos[i].kind == Kind::Declaration) {
            maxDimCol = std::max(maxDimCol, infos[i].identPos);
        }
    }

    const auto contentAlign = alignStyle.AlignColon || alignStyle.PadLeft || alignStyle.PadRight;

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

std::string normalizeBracketRange(std::string_view content) {
    auto colonPos = content.find(':');
    if (colonPos == npos) {
        return std::string{content};
    }

    auto left = content.substr(0, colonPos);
    auto right = content.substr(colonPos + 1);

    const auto hadSpaces = (colonPos > 0 && content[colonPos - 1] == ' ') ||
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

} // namespace

namespace slang::format {

bool shouldBreakDimGroup(const LineInfo& info, const AlignState& state,
                         const AlignConsecutiveStyle& alignStyle, const GroupBreakConfig& config) {
    if ((info.kind == LineMetadata::Kind::Declaration ||
         info.kind == LineMetadata::Kind::Continuation) &&
        info.dimPos == std::string_view::npos) {
        return false;
    }
    return shouldBreakGroup(info, state, alignStyle, config);
}

void alignGroupPackedDimensions(std::string& result, const std::vector<std::string_view>& lines,
                                const std::vector<LineInfo>& infos, GroupRange range,
                                const InferContext& ctx, const AlignConsecutiveStyle& alignStyle,
                                RangeSpacingStyle rangeSpacing) {
    size_t maxDimCol = 0;
    size_t minDimCol = npos;
    for (size_t i = range.start; i < range.end; i++) {
        if (infos[i].kind == LineMetadata::Kind::Declaration && infos[i].dimPos != npos) {
            maxDimCol = std::max(maxDimCol, infos[i].dimPos);
            minDimCol = std::min(minDimCol, infos[i].dimPos);
        }
    }
    auto displacement = (minDimCol != npos) ? maxDimCol - minDimCol : size_t{0};
    auto policy = inferAlignPolicy(static_cast<unsigned>(displacement), ctx);
    if (policy == AlignPolicy::FlushLeft) {
        for (size_t i = range.start; i < range.end; i++) {
            result.append(lines[i]);
            result += '\n';
        }
        return;
    }
    alignGroupDimensions(result, lines, infos, range, alignStyle, rangeSpacing);
}

std::string normalizeRangeSpacing(const std::string& output, const AlignContext& ctx,
                                  RangeSpacingStyle rangeSpacing) {
    if (rangeSpacing != RangeSpacingStyle::MatchSource) {
        return output;
    }

    std::string result;
    result.reserve(output.size());

    std::string_view remaining{output};
    size_t lineNum = 0;
    while (!remaining.empty()) {
        auto nl = remaining.find('\n');
        auto line = (nl == npos) ? remaining : remaining.substr(0, nl);

        auto shouldNormalize = false;
        auto it = ctx.metaByLine.find(lineNum);
        if (it != ctx.metaByLine.end()) {
            const auto& meta = ctx.lineMetadata[it->second];
            shouldNormalize = hasMultiSpaceRange(meta) && !ctx.adjacentLines.contains(lineNum);
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

} // namespace slang::format
