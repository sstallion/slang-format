// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include "Align.h"

#include <cstddef>
#include <set>
#include <unordered_map>
#include <vector>

namespace slang::format {

enum class AlignPolicy { Align, FlushLeft };

/// Inputs to the InferFromSource alignment decision for one group.
struct InferContext {
    bool enabled = false;         ///< Whether InferFromSource logic is active.
    bool minGuarantee = false;    ///< When true, small displacements always align.
    unsigned maxPadding = 0;      ///< Maximum column padding before flushing left.
    unsigned minPadding = 0;      ///< Minimum displacement that always aligns when enabled.
    unsigned maxDisplacement = 0; ///< Maximum displacement before flushing left when enabled.
    unsigned groupMaxExcess = 0;  ///< Maximum source-vs-output spacing excess in the group.
};

/// Decides whether a group with the given column @p displacement is aligned or flushed left.
AlignPolicy inferAlignPolicy(unsigned displacement, const InferContext& ctx);

/// Returns the maximum source-vs-output spacing excess recorded for @p meta.
unsigned computeMetaExcess(const LineMetadata& meta);

/// Returns true if @p meta records a range colon with more than one space on either side.
bool hasMultiSpaceRange(const LineMetadata& meta);

/// Returns the maximum spacing excess across output lines [@p start, @p end), including range
/// excess for lines in @p rangeExcessLines.
unsigned computeGroupMaxExcess(const std::vector<LineMetadata>& lineMetadata,
                               const std::unordered_map<size_t, size_t>& metaByLine,
                               const std::set<size_t>& rangeExcessLines, size_t start, size_t end);

/// Returns the output lines adjacent to a parameter port list group with alignment evidence.
std::set<size_t> computePortListAdjacentLines(const std::vector<LineMetadata>& lineMetadata);

/// Returns the port list lines whose range excess contributes to InferFromSource decisions.
std::set<size_t> computeRangeExcessLines(const std::vector<LineMetadata>& lineMetadata,
                                         const std::set<size_t>& adjacentLines);

} // namespace slang::format
