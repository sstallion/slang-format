// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include "Align.h"

#include <cstddef>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace slang::format {

/// Classification of a single output line used by the alignment passes.
struct LineInfo {
    LineMetadata::Kind kind = LineMetadata::Kind::Other;
    size_t indent = 0;       ///< Column of the first non-space character.
    size_t keywordWidth = 0; ///< Width of direction/parameter keyword, or 0 if absent.
    size_t typeWidth = 0;    ///< Distance from indent to identifier start.
    size_t identPos = 0;     ///< Absolute position of the identifier.
    size_t equalsPos =
        std::string_view::npos; ///< Position of the alignment operator, or npos if absent.
    size_t dimPos =
        std::string_view::npos; ///< Position of the first packed dimension, or npos if absent.
    unsigned depth = 0;         ///< AST nesting depth at the line's emission point.
    unsigned maxExcess = 0;     ///< Maximum source-vs-output spacing excess on this line.
    bool metadataKind = false;  ///< True when kind was set from tree-walk metadata.
};

/// Returns the position after any run of spaces starting at @p pos.
size_t skipSpaces(std::string_view line, size_t pos);

/// Returns the position before any run of spaces ending at @p pos.
size_t trimTrailingSpaces(std::string_view line, size_t pos);

/// Returns the position of a trailing comment in @p line, or npos if none is present.
size_t findTrailingComment(std::string_view line);

/// Classifies every line of formatted output, combining text scanning with tree-walk metadata.
std::vector<LineInfo> classifyLines(const std::vector<std::string_view>& lines,
                                    const std::vector<LineMetadata>& lineMetadata,
                                    const std::unordered_map<size_t, size_t>& metaByLine);

} // namespace slang::format
