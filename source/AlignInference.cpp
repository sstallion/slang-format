// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "AlignInference.h"
#include "Align.h"

#include <algorithm>
#include <cstddef>
#include <set>
#include <unordered_map>
#include <vector>

using namespace slang::format;

namespace {

using Kind = LineMetadata::Kind;

} // namespace

namespace slang::format {

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

unsigned computeMetaExcess(const LineMetadata& meta) {
    unsigned excess = 0;
    for (const auto& ts : meta.tokenSpacings) {
        if (ts.sourceSpaces > ts.outputSpaces) {
            excess = std::max(excess, ts.sourceSpaces - ts.outputSpaces);
        }
    }
    return excess;
}

bool hasMultiSpaceRange(const LineMetadata& meta) {
    return std::ranges::any_of(meta.rangeSpacings, [](const auto& rs) {
        return rs.sourceLeftSpaces > 1 || rs.sourceRightSpaces > 1;
    });
}

unsigned computeGroupMaxExcess(const std::vector<LineMetadata>& lineMetadata,
                               const std::unordered_map<size_t, size_t>& metaByLine,
                               const std::set<size_t>& rangeExcessLines, size_t start, size_t end) {
    unsigned maxExcess = 0;
    for (auto i = start; i < end; i++) {
        auto it = metaByLine.find(i);
        if (it == metaByLine.end()) {
            continue;
        }

        const auto& meta = lineMetadata[it->second];
        for (const auto& ts : meta.tokenSpacings) {
            if (ts.sourceSpaces > ts.outputSpaces) {
                maxExcess = std::max(maxExcess, ts.sourceSpaces - ts.outputSpaces);
            }
        }

        if (rangeExcessLines.contains(i)) {
            for (const auto& rs : meta.rangeSpacings) {
                if (rs.sourceLeftSpaces > 1) {
                    maxExcess = std::max(maxExcess, rs.sourceLeftSpaces - 1);
                }
                if (rs.sourceRightSpaces > 1) {
                    maxExcess = std::max(maxExcess, rs.sourceRightSpaces - 1);
                }
            }
        }
    }
    return maxExcess;
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

std::set<size_t> computeRangeExcessLines(const std::vector<LineMetadata>& lineMetadata,
                                         const std::set<size_t>& adjacentLines) {
    std::set<size_t> rangeExcessLines;
    bool inPortList = false;

    for (const auto& meta : lineMetadata) {
        if (meta.kind == Kind::PortListBoundary) {
            inPortList = true;
            continue;
        }

        if (meta.kind == Kind::Empty) {
            inPortList = false;
            continue;
        }

        if (meta.kind == Kind::Comment) {
            continue;
        }

        if (inPortList && !adjacentLines.contains(meta.lineNumber)) {
            rangeExcessLines.insert(meta.lineNumber);
        }
    }

    return rangeExcessLines;
}

} // namespace slang::format
