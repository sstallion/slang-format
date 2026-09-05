// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include "FileLoader.h"

#include <filesystem>
#include <string>

#include <yaml-cpp/yaml.h>

namespace slang::format {

/// Controls alignment behavior for consecutive declarations.
struct AlignConsecutiveStyle {
    /// If true, comments do not break alignment groups.
    bool AcrossComments = false;

    /// If true, empty lines do not break alignment groups.
    bool AcrossEmptyLines = false;

    /// If true, port list boundaries do not break alignment groups.
    bool AcrossParameterPortList = false;

    /// If true, align `:` separators in packed dimensions by right-justifying the left-side value.
    bool AlignColon = false;

    /// If false, disables all alignment regardless of other options.
    bool Enabled = false;

    /// If true, left-pad content within brackets (right-justify).
    bool PadLeft = false;

    /// If true, right-pad content within brackets (left-justify).
    bool PadRight = false;

    bool operator==(const AlignConsecutiveStyle&) const = default;
};

enum class BlockBreakStyle { Never, Always, OnlyMultiline };

enum class DimensionBoundsStyle { LSBFirst, MSBFirst, Preserve };

enum class EventSeparatorStyle { Comma, Or, Preserve };

/// Controls automatic insertion of begin/end around statement bodies.
struct InsertBeginEndStyle {
    /// If false, disables all begin/end insertion regardless of other options.
    bool Enabled = false;

    /// If true, wrap bare single-statement bodies of control statements.
    bool ControlStatements = true;

    /// If true, wrap bare statement bodies of always statements.
    bool AlwaysStatements = true;

    /// If true, wrap bare statement bodies of initial statements.
    bool InitialStatements = true;

    bool operator==(const InsertBeginEndStyle&) const = default;
};

/// Controls automatic insertion of parentheses around timing constructs.
struct InsertParensStyle {
    /// If true, insert parentheses around delay expressions (e.g. `#15` to `#(15)`).
    bool Delays = false;

    /// If true, insert parentheses around event expressions (e.g. `@posedge clk` to `@(posedge
    /// clk)`).
    bool ExpressionEvents = false;

    /// If true, insert parentheses around implicit event lists (e.g. `@*` to `@(*)`).
    bool ImplicitEvents = false;

    /// If true, insert parentheses around named event expressions (e.g. `@signal` to `@(signal)`).
    bool NamedEvents = false;

    bool operator==(const InsertParensStyle&) const = default;
};

/// Defines all formatting style options.
struct Style {
    /// Controls alignment of assignment operators in consecutive declarations.
    AlignConsecutiveStyle AlignConsecutiveAssignments;

    /// Controls alignment of trailing comments in consecutive lines.
    AlignConsecutiveStyle AlignTrailingComments;

    /// Controls alignment of signal names in consecutive declarations.
    AlignConsecutiveStyle AlignConsecutiveDeclarations;

    /// Controls alignment of packed dimensions in consecutive declarations.
    AlignConsecutiveStyle AlignConsecutivePackedDimensions;

    /// Controls alignment of signal names and assignment operators in consecutive timing controls.
    AlignConsecutiveStyle AlignConsecutiveTimingControls;

    /// Maximum number of consecutive empty lines to keep.
    unsigned MaxEmptyLinesToKeep = 1;

    /// Number of spaces for each indentation level.
    unsigned IndentWidth = 2;

    /// Number of spaces added for continuation lines relative to the
    /// enclosing statement.
    unsigned ContinuationIndentWidth = 2;

    /// Number of spaces for parameter port list items in module declarations.
    unsigned ParameterPortListIndentWidth = 2;

    /// Controls the separator used in event expressions.
    EventSeparatorStyle EventSeparator = EventSeparatorStyle::Preserve;

    /// If true, indent statements following a case label when they appear on
    /// the next line.
    bool IndentCaseItem = true;

    /// Disables formatting for any line matching this pattern.
    std::string OneLineFormatOffRegex;

    /// Controls the order of packed dimension bounds.
    DimensionBoundsStyle PackedDimensionBounds = DimensionBoundsStyle::Preserve;

    /// If true, normalizes whitespace around commas to a single trailing space.
    bool SpaceAfterComma = false;

    /// Controls the order of unpacked dimension bounds.
    DimensionBoundsStyle UnpackedDimensionBounds = DimensionBoundsStyle::Preserve;

    /// Controls insertion of a newline between always and its body.
    BlockBreakStyle BreakAfterAlways = BlockBreakStyle::OnlyMultiline;

    /// Controls insertion of a newline between initial and its body.
    BlockBreakStyle BreakAfterInitial = BlockBreakStyle::OnlyMultiline;

    /// Controls insertion of a blank line before always and its body.
    BlockBreakStyle BreakBeforeAlways = BlockBreakStyle::OnlyMultiline;

    /// Controls insertion of a blank line before initial and its body.
    BlockBreakStyle BreakBeforeInitial = BlockBreakStyle::OnlyMultiline;

    /// If true, enforce a newline after every begin statement.
    bool BreakAfterBegin = true;

    /// If true, enforce a newline before every end statement.
    bool BreakBeforeEnd = true;

    /// If true, insert a blank line before function declarations.
    bool BreakBeforeFunction = true;

    /// If true, insert a blank line before specify blocks.
    bool BreakBeforeSpecifyBlock = true;

    /// If true, insert a blank line before task declarations.
    bool BreakBeforeTask = true;

    /// Controls insertion of begin/end around bare statements.
    InsertBeginEndStyle InsertBeginEnd;

    /// Controls insertion of parentheses around timing constructs.
    InsertParensStyle InsertParens;

    bool operator==(const Style&) const = default;
};

/// Returns the default style.
Style getDefaultStyle();

/// Serializes \p style to a YAML document string.
std::string dumpConfiguration(const Style& style);

/// Parses a YAML node into \p style. Throws std::runtime_error on error.
void parseConfiguration(const YAML::Node& node, Style& style);

/// Searches for a configuration file starting from \p searchDir, walking up
/// the directory hierarchy to the root. Returns the default style if no
/// configuration file is found.
Style getStyle(const std::filesystem::path& searchDir, FileLoader loader = {});

} // namespace slang::format
