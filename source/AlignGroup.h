// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include "Align.h"
#include "AlignClassifier.h"
#include "AlignInference.h"
#include "Style.h"

#include <cstddef>
#include <optional>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <slang/util/Function.h>

namespace slang::format {

/// Half-open range of line indices forming an alignment group.
struct GroupRange {
    size_t start;
    size_t end;
};

/// Per-pass rules for which lines join, continue, or break a group.
struct GroupBreakConfig {
    std::span<const LineMetadata::Kind> memberKinds;
    bool continuationPassThrough = true;
    bool depthScoped = false;
    bool indentAlwaysBreaks = false;
    bool portListAware = true;
};

/// Mutable state while scanning for groups.
struct AlignState {
    size_t groupStart = 0;
    bool inGroup = false;
    std::optional<size_t> groupIndent;
    std::optional<unsigned> groupDepth;
};

/// Shared inputs for all alignment passes over one formatted output.
struct AlignContext {
    const std::vector<LineMetadata>& lineMetadata;
    std::unordered_map<size_t, size_t> metaByLine; ///< Output line number to lineMetadata index.
    std::set<size_t> adjacentLines;                ///< From computePortListAdjacentLines.
    std::set<size_t> rangeExcessLines;             ///< From computeRangeExcessLines.
    unsigned continuationIndentWidth;
};

using BreakPred = bool (*)(const LineInfo&, const AlignState&, const AlignConsecutiveStyle&,
                           const GroupBreakConfig&);

using AlignGroupFn =
    slang::function_ref<void(std::string&, const std::vector<std::string_view>&,
                             const std::vector<LineInfo>&, GroupRange, const InferContext&)>;

/// Returns true if \p info ends the current group under \p alignStyle and \p config.
bool shouldBreakGroup(const LineInfo& info, const AlignState& state,
                      const AlignConsecutiveStyle& alignStyle, const GroupBreakConfig& config);

/// Splits \p output into lines, forms consecutive groups according to \p breakPred, and emits each
/// group through \p alignFn.
std::string applyAlignConsecutive(const std::string& output,
                                  const AlignConsecutiveStyle& alignStyle,
                                  const GroupBreakConfig& config, BreakPred breakPred,
                                  AlignGroupFn alignFn, const AlignContext& ctx);

} // namespace slang::format
