// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include "slang/format/Style.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace slang::format {

/// Metadata collected per output line during the formatting tree walk.
struct LineMetadata {
    enum class Kind {
        Assignment,
        Comment,
        Continuation,
        Declaration,
        DistItem,
        Empty,
        Other,
        PortConnection,
        PortListBoundary,
        TimingControl
    };

    Kind kind = Kind::Other;
    size_t lineNumber = std::string_view::npos; ///< Output line index this metadata corresponds to.
    unsigned depth = 0;

    /// Spacing data for a single inter-token gap on this line.
    struct TokenSpacing {
        unsigned sourceSpaces; ///< Whitespace characters before the token in the source.
        unsigned outputSpaces; ///< Spaces before the token in the formatted output.
    };

    std::vector<TokenSpacing> tokenSpacings;

    /// Source spacing around the colon in a range expression.
    struct RangeSpacing {
        unsigned sourceLeftSpaces;  ///< Source whitespace before the range colon.
        unsigned sourceRightSpaces; ///< Source whitespace after the range colon.
    };

    std::vector<RangeSpacing> rangeSpacings;
};

/// Applies all alignment post-processing passes to the formatted output.
std::string applyAlignment(const std::string& output, const Style& style,
                           const std::vector<LineMetadata>& lineMetadata);

} // namespace slang::format
