// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "AlignAssignment.h"
#include "Align.h"
#include "AlignClassifier.h"
#include "AlignGroup.h"
#include "AlignInference.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

using namespace slang::format;

namespace {

using Kind = LineMetadata::Kind;

constexpr auto npos = std::string_view::npos;

bool isAlignableKind(const LineInfo& info) {
    return info.kind == Kind::Assignment || info.kind == Kind::Declaration ||
           info.kind == Kind::Continuation || info.kind == Kind::DistItem;
}

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

struct AlignBucket {
    unsigned depth;
    size_t subRun;
    size_t maxEqualsCol = 0;
    size_t minEqualsCol = npos;
    size_t count = 0;
    unsigned maxExcess = 0;
};

} // namespace

namespace slang::format {

void alignGroupAssignments(std::string& result, const std::vector<std::string_view>& lines,
                           const std::vector<LineInfo>& infos, GroupRange range,
                           const InferContext& ctx) {
    auto subRuns = assignSubRuns(infos, range);

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
        auto policy = buckets[b].count >= 2
                          ? inferAlignPolicy(static_cast<unsigned>(padding), bucketCtx)
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
        if (infos[i].kind == LineMetadata::Kind::PortConnection && infos[i].equalsPos != npos) {
            connCount++;
            maxParenCol = std::max(maxParenCol, infos[i].equalsPos);
            minParenCol = std::min(minParenCol, infos[i].equalsPos);
        }
    }

    for (auto i = range.start; i < range.end; i++) {
        if (infos[i].kind == LineMetadata::Kind::PortConnection && infos[i].equalsPos != npos) {
            auto padding = maxParenCol - minParenCol;
            auto policy = connCount >= 2 ? inferAlignPolicy(static_cast<unsigned>(padding), ctx)
                                         : AlignPolicy::FlushLeft;
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

} // namespace slang::format
