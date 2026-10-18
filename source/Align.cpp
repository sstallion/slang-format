// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Align.h"
#include "AlignAssignment.h"
#include "AlignClassifier.h"
#include "AlignDeclaration.h"
#include "AlignDimension.h"
#include "AlignGroup.h"
#include "AlignInference.h"
#include "AlignTrailingComment.h"
#include "Style.h"

#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace slang::format;

namespace {

using Kind = LineMetadata::Kind;

constexpr auto npos = std::string_view::npos;

std::unordered_map<size_t, size_t> buildMetaByLine(const std::vector<LineMetadata>& lineMetadata) {
    std::unordered_map<size_t, size_t> metaByLine;
    for (size_t j = 0; j < lineMetadata.size(); j++) {
        if (lineMetadata[j].lineNumber != npos) {
            metaByLine[lineMetadata[j].lineNumber] = j;
        }
    }
    return metaByLine;
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

    const auto adjacentLines = computePortListAdjacentLines(lineMetadata);
    const AlignContext ctx{.lineMetadata = lineMetadata,
                           .metaByLine = buildMetaByLine(lineMetadata),
                           .adjacentLines = adjacentLines,
                           .rangeExcessLines = computeRangeExcessLines(lineMetadata, adjacentLines),
                           .continuationIndentWidth = style.ContinuationIndentWidth};

    auto normalized = (style.AlignConsecutivePackedDimensions.InferFromSource)
                          ? normalizeRangeSpacing(output, ctx, style.RangeSpacing)
                          : output;

    auto dimAlignFn = [&style](std::string& r, const std::vector<std::string_view>& l,
                               const std::vector<LineInfo>& inf, GroupRange rng,
                               const InferContext& ictx) {
        alignGroupPackedDimensions(r, l, inf, rng, ictx, style.AlignConsecutivePackedDimensions,
                                   style.RangeSpacing);
    };
    auto result = applyAlignConsecutive(normalized, style.AlignConsecutivePackedDimensions,
                                        dimConfig, shouldBreakDimGroup, dimAlignFn, ctx);
    result = applyAlignConsecutive(result, style.AlignConsecutiveDeclarations, declConfig,
                                   shouldBreakGroup, alignGroupDeclarations, ctx);
    result = applyAlignConsecutive(result, style.AlignConsecutiveTimingControls, timingConfig,
                                   shouldBreakGroup, alignGroupTimingControls, ctx);
    result = applyAlignConsecutive(result, style.AlignConsecutiveAssignments, assignConfig,
                                   shouldBreakGroup, alignGroupAssignments, ctx);
    result = applyAlignConsecutive(result, style.AlignConsecutivePortConnections,
                                   portConnectionConfig, shouldBreakGroup,
                                   alignGroupPortConnections, ctx);
    auto trailingAlignFn = [&style](std::string& r, const std::vector<std::string_view>& l,
                                    const std::vector<LineInfo>& inf, GroupRange rng,
                                    const InferContext& /*ictx*/) {
        alignGroupTrailingComments(r, l, inf, rng, style.SpacesBeforeTrailingComments);
    };
    result = applyAlignConsecutive(result, style.AlignTrailingComments, trailingConfig,
                                   shouldBreakGroup, trailingAlignFn, ctx);

    return result;
}

} // namespace slang::format
