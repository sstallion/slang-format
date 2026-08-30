// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace slang::format {

struct Style;

/// Metadata collected per output line during the formatting tree walk.
struct LineMetadata {
    enum class Kind {
        Assignment,
        Comment,
        Continuation,
        Declaration,
        Empty,
        Other,
        PortListBoundary,
        TimingControl
    };

    Kind kind = Kind::Other;
    unsigned depth = 0;
    size_t typeWidth = 0;
    size_t identPos = std::string_view::npos;
    size_t equalsPos = std::string_view::npos;
    size_t dimPos = std::string_view::npos;
    size_t trailingCommentPos = std::string_view::npos;

    /// Per-dimension metadata for packed dimension alignment.
    struct DimInfo {
        size_t openPos;
        size_t closePos;
        size_t leftWidth;
        size_t rightWidth;
        bool hasColon;
    };

    std::vector<DimInfo> dims;
};

/// Applies all alignment post-processing passes to the formatted output.
std::string applyAlignment(const std::string& output, const Style& style,
                           const std::vector<LineMetadata>& lineMetadata);

} // namespace slang::format
