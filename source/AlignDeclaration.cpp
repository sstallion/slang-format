// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "AlignDeclaration.h"
#include "Align.h"
#include "AlignClassifier.h"
#include "AlignGroup.h"
#include "AlignInference.h"

#include <algorithm>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using namespace slang::format;

namespace {

using Kind = LineMetadata::Kind;

constexpr auto npos = std::string_view::npos;

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

        if (info.dimPos != npos && info.dimPos > typeBodyStart) {
            auto nameEnd = trimTrailingSpaces(line, info.dimPos);
            auto nameWidth = nameEnd > typeBodyStart ? nameEnd - typeBodyStart : size_t{0};
            auto dimEnd = trimTrailingSpaces(line, identPos);
            auto dimWidth = dimEnd - info.dimPos;

            if (nameWidth > 0) {
                result.append(line.substr(typeBodyStart, nameWidth));
            }

            auto bodyTarget = targetPos - indent - metrics.maxKeywordWidth - 1;
            result.append(bodyTarget - nameWidth - dimWidth - 1, ' ');
            result.append(line.substr(info.dimPos, dimWidth));
            result += ' ';
        }
        else {
            if (typeBodyStart < typeTextEnd) {
                result.append(line.substr(typeBodyStart, typeTextEnd - typeBodyStart));
            }
            result.append(targetPos - (metrics.maxKeywordWidth + 1) -
                              (typeTextEnd > typeBodyStart ? typeTextEnd - typeBodyStart : 0) -
                              indent,
                          ' ');
        }
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

DeclGroupSummary computeDeclGroupSummary(const std::vector<std::string_view>& lines,
                                         const std::vector<LineInfo>& infos, GroupRange range) {
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

            auto bw = typeBodyWidth(infos[i]);
            if (infos[i].dimPos != npos && infos[i].keywordWidth > 0) {
                auto typeBodyStart = infos[i].indent + infos[i].keywordWidth + 1;
                auto nameEnd = trimTrailingSpaces(lines[i], infos[i].dimPos);
                auto nameWidth = nameEnd > typeBodyStart ? nameEnd - typeBodyStart : size_t{0};
                auto dimWidth = infos[i].identPos - infos[i].dimPos;
                bw = nameWidth + 1 + dimWidth;
            }
            maxBodyWidth = std::max(maxBodyWidth, bw);
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

void padTimingIdent(std::string& p, std::string_view line, const LineInfo& info,
                    size_t maxTypeWidth) {
    auto targetPos = info.indent + maxTypeWidth;
    auto typeTextEnd = trimTrailingSpaces(line, info.identPos);

    p.append(line.substr(0, typeTextEnd));
    p.append(targetPos - (typeTextEnd - info.indent) - info.indent, ' ');
    p.append(line.substr(info.identPos));
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

        std::string p;
        padTimingIdent(p, lines[i], infos[i], maxTypeWidth);
        auto shift = static_cast<ptrdiff_t>(infos[i].indent + maxTypeWidth) -
                     static_cast<ptrdiff_t>(infos[i].identPos);
        auto adjustedEquals = static_cast<size_t>(static_cast<ptrdiff_t>(infos[i].equalsPos) +
                                                  shift);
        maxEqualsCol = std::max(maxEqualsCol, adjustedEquals);
        padded.push_back(std::move(p));
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

} // namespace

namespace slang::format {

void alignGroupDeclarations(std::string& result, const std::vector<std::string_view>& lines,
                            const std::vector<LineInfo>& infos, GroupRange range,
                            const InferContext& ctx) {
    auto [metrics, declCount, displacement, hasKeywords,
          groupIndent] = computeDeclGroupSummary(lines, infos, range);

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

void alignGroupTimingControls(std::string& result, const std::vector<std::string_view>& lines,
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

} // namespace slang::format
