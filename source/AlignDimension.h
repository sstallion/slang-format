// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include "AlignClassifier.h"
#include "AlignGroup.h"
#include "AlignInference.h"
#include "Style.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace slang::format {

/// Returns true if \p info ends the current dimension alignment group under \p alignStyle and \p
/// config.
bool shouldBreakDimGroup(const LineInfo& info, const AlignState& state,
                         const AlignConsecutiveStyle& alignStyle, const GroupBreakConfig& config);

/// Aligns packed dimensions for a group, applying InferFromSource policy to decide between
/// alignment and flush-left.
void alignGroupPackedDimensions(std::string& result, const std::vector<std::string_view>& lines,
                                const std::vector<LineInfo>& infos, GroupRange range,
                                const InferContext& ctx, const AlignConsecutiveStyle& alignStyle,
                                RangeSpacingStyle rangeSpacing);

/// Normalizes range spacing in the formatted output according to the alignment context.
std::string normalizeRangeSpacing(const std::string& output, const AlignContext& ctx,
                                  RangeSpacingStyle rangeSpacing);

} // namespace slang::format
