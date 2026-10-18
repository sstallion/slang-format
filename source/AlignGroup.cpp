// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "AlignGroup.h"
#include "Align.h"
#include "AlignClassifier.h"
#include "AlignInference.h"
#include "Style.h"

#include <algorithm>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <vector>

using namespace slang::format;

namespace {

using Kind = LineMetadata::Kind;

bool isMemberKind(Kind kind, std::span<const Kind> memberKinds) {
    return std::ranges::find(memberKinds, kind) != memberKinds.end();
}

bool canStartGroup(const LineInfo& info, const GroupBreakConfig& config) {
    return isMemberKind(info.kind, config.memberKinds);
}

} // namespace

namespace slang::format {

bool shouldBreakGroup(const LineInfo& info, const AlignState& state,
                      const AlignConsecutiveStyle& alignStyle, const GroupBreakConfig& config) {
    const auto acrossEmpty = alignStyle.AcrossEmptyLines;
    const auto acrossComments = alignStyle.AcrossComments;
    const auto acrossIndent = alignStyle.AcrossParameterPortList;

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

std::string applyAlignConsecutive(const std::string& output,
                                  const AlignConsecutiveStyle& alignStyle,
                                  const GroupBreakConfig& config, BreakPred breakPred,
                                  AlignGroupFn alignFn, const AlignContext& ctx) {
    if (!alignStyle.Enabled) {
        return output;
    }

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

    auto infos = classifyLines(lines, ctx.lineMetadata, ctx.metaByLine);

    auto makeCtx = [&](size_t start, size_t end) {
        return InferContext{.enabled = alignStyle.InferFromSource,
                            .maxPadding = alignStyle.MaxPadding,
                            .minPadding = alignStyle.MinPadding,
                            .maxDisplacement = alignStyle.MaxPadding * ctx.continuationIndentWidth,
                            .groupMaxExcess =
                                computeGroupMaxExcess(ctx.lineMetadata, ctx.metaByLine,
                                                      ctx.rangeExcessLines, start, end)};
    };

    std::string result;
    result.reserve(output.size());
    AlignState state;

    for (size_t i = 0; i < lines.size(); i++) {
        auto& info = infos[i];
        const auto breakGroup = breakPred(info, state, alignStyle, config);

        if (breakGroup && state.inGroup) {
            auto ictx = makeCtx(state.groupStart, i);
            alignFn(result, lines, infos, {.start = state.groupStart, .end = i}, ictx);
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
        auto ictx = makeCtx(state.groupStart, lines.size());
        alignFn(result, lines, infos, {.start = state.groupStart, .end = lines.size()}, ictx);
    }

    if (!result.empty() && result.back() == '\n' && (output.empty() || output.back() != '\n')) {
        result.pop_back();
    }

    return result;
}

} // namespace slang::format
