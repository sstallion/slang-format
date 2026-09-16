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
    /// If false, disables all alignment regardless of other options.
    bool Enabled = false;

    /// If true, comments do not break alignment groups.
    bool AcrossComments = false;

    /// If true, empty lines do not break alignment groups.
    bool AcrossEmptyLines = false;

    /// If true, port list boundaries do not break alignment groups.
    bool AcrossParameterPortList = false;

    /// If true, align `:` separators in packed dimensions by right-justifying
    /// the left-side value.
    bool AlignColon = false;

    /// Maximum number of spaces any line may be padded; 0 means unlimited.
    unsigned MaxPadding = 0;

    /// If true, left-pad content within brackets (right-justify).
    bool PadLeft = false;

    /// If true, right-pad content within brackets (left-justify).
    bool PadRight = false;

    bool operator==(const AlignConsecutiveStyle&) const = default;
};

enum class BlockBreakStyle { Never, Always, OnlyMultiline };

enum class BracketAlignmentStyle { Align, AlwaysBreak, BlockIndent, DontAlign };

enum class DimensionBoundsStyle { LSBFirst, MSBFirst, Preserve };

enum class EventSeparatorStyle { Comma, Or, Preserve };

/// Controls automatic insertion of begin/end around statement bodies.
struct InsertBeginEndStyle {
    /// If false, disables all begin/end insertion regardless of other options.
    bool Enabled = false;

    /// If true, wrap bare single-statement bodies of control statements.
    bool ControlStatements = false;

    /// If true, wrap bare statement bodies of always statements.
    bool AlwaysStatements = false;

    /// If true, wrap bare statement bodies of initial statements.
    bool InitialStatements = false;

    bool operator==(const InsertBeginEndStyle&) const = default;
};

/// Controls automatic insertion of parentheses around timing constructs.
struct InsertParensStyle {
    /// If true, insert parentheses around delay expressions.
    bool Delays = false;

    /// If true, insert parentheses around event expressions.
    bool ExpressionEvents = false;

    /// If true, insert parentheses around implicit event lists.
    bool ImplicitEvents = false;

    /// If true, insert parentheses around named event expressions.
    bool NamedEvents = false;

    bool operator==(const InsertParensStyle&) const = default;
};

/// Controls whitespace before an opening parenthesis.
struct SpaceBeforeParensStyle {
    /// If true, insert a space before parentheses in control statements.
    bool ControlStatements = true;

    /// If true, insert a space before a parameter port list.
    bool ParameterList = true;

    /// If true, insert a space before a port list.
    bool PortList = true;

    bool operator==(const SpaceBeforeParensStyle&) const = default;
};

/// Controls whitespace after a closing parenthesis.
struct SpaceAfterParensStyle {
    /// If true, insert a space after parentheses in control statements.
    bool ControlStatements = true;

    /// If true, insert a space after parentheses in event controls.
    bool EventControls = true;

    bool operator==(const SpaceAfterParensStyle&) const = default;
};

/// Defines all formatting style options.
struct Style {
    /// Controls alignment of arguments after an opening parenthesis in
    /// function calls, system task calls, and constructor invocations.
    BracketAlignmentStyle AlignAfterOpenParen = BracketAlignmentStyle::Align;

    /// Controls alignment of assignment operators in consecutive declarations.
    AlignConsecutiveStyle AlignConsecutiveAssignments{.Enabled = true, .MaxPadding = 2};

    /// Controls alignment of opening parentheses in consecutive named port
    /// connections.
    AlignConsecutiveStyle AlignConsecutivePortConnections{.Enabled = true, .MaxPadding = 2};

    /// Controls alignment of trailing comments in consecutive lines.
    AlignConsecutiveStyle AlignTrailingComments;

    /// Controls alignment of signal names in consecutive declarations.
    AlignConsecutiveStyle AlignConsecutiveDeclarations;

    /// Controls alignment of packed dimensions in consecutive declarations.
    AlignConsecutiveStyle AlignConsecutivePackedDimensions;

    /// Controls alignment of signal names and assignment operators in
    /// consecutive timing controls.
    AlignConsecutiveStyle AlignConsecutiveTimingControls;

    /// If true, arguments are bin-packed when they exceed the column limit.
    /// If false, each argument is placed on its own line.
    bool BinPackArguments = true;

    /// Maximum number of consecutive empty lines to keep.
    unsigned MaxEmptyLinesToKeep = 1;

    /// If true, enforce one statement per line.
    bool OneStatementPerLine = true;

    /// If true, enables enforcement of MaxEmptyLinesToKeep.
    bool RemoveEmptyLines = false;

    /// Number of spaces for each indentation level.
    unsigned IndentWidth = 2;

    /// Number of spaces added for continuation lines relative to the
    /// enclosing statement.
    unsigned ContinuationIndentWidth = 4;

    /// Number of spaces for parameter port list items in module declarations.
    unsigned ParameterPortListIndentWidth = 4;

    /// Controls the separator used in event expressions.
    EventSeparatorStyle EventSeparator = EventSeparatorStyle::Preserve;

    /// If true, indent statements following a case label when they appear on
    /// the next line.
    bool IndentCaseItem = false;

    /// Disables formatting for any line matching this pattern.
    std::string OneLineFormatOffRegex;

    /// Controls the order of packed dimension bounds.
    DimensionBoundsStyle PackedDimensionBounds = DimensionBoundsStyle::Preserve;

    /// Normalizes whitespace after an always keyword.
    bool SpaceAfterAlways = true;

    /// Normalizes whitespace after a closing square bracket in declarations.
    bool SpaceAfterBrackets = true;

    /// Normalizes whitespace after a case item colon.
    bool SpaceAfterCaseColon = true;

    /// Normalizes whitespace around commas.
    bool SpaceAfterComma = true;

    /// Controls whitespace after a closing parenthesis.
    SpaceAfterParensStyle SpaceAfterParens;

    /// Normalizes whitespace around semicolons.
    bool SpaceAfterSemicolon = true;

    /// Normalizes whitespace before an opening square bracket in declarations.
    bool SpaceBeforeBrackets = true;

    /// Normalizes whitespace before a case item colon.
    bool SpaceBeforeCaseColon = false;

    /// Controls whitespace before an opening parenthesis.
    SpaceBeforeParensStyle SpaceBeforeParens;

    /// Normalizes whitespace around assignment, binary, and ternary operators.
    bool SpaceAroundOperators = true;

    /// Minimum number of spaces before a trailing comment.
    unsigned SpacesBeforeTrailingComments = 2;

    /// Normalizes whitespace inside curly braces.
    bool SpacesInBraces = false;

    /// Normalizes whitespace inside square brackets in declarations.
    bool SpacesInBrackets = false;

    /// Normalizes whitespace inside parentheses.
    /// space after ( and before ).
    bool SpacesInParens = false;

    /// Controls the order of unpacked dimension bounds.
    DimensionBoundsStyle UnpackedDimensionBounds = DimensionBoundsStyle::Preserve;

    /// Controls insertion of a newline between always and its body.
    BlockBreakStyle BreakAfterAlways = BlockBreakStyle::OnlyMultiline;

    /// Controls insertion of a newline between initial and its body.
    BlockBreakStyle BreakAfterInitial = BlockBreakStyle::OnlyMultiline;

    /// Controls insertion of a blank line before always and its body.
    BlockBreakStyle BreakBeforeAlways = BlockBreakStyle::Never;

    /// Controls insertion of a blank line before initial and its body.
    BlockBreakStyle BreakBeforeInitial = BlockBreakStyle::Never;

    /// If true, enforce a newline after every begin statement.
    bool BreakAfterBegin = true;

    /// If true, enforce a newline before every end statement.
    bool BreakBeforeEnd = true;

    /// If true, insert a blank line before function declarations.
    bool BreakBeforeFunction = false;

    /// If true, insert a blank line before specify blocks.
    bool BreakBeforeSpecifyBlock = false;

    /// If true, insert a blank line before task declarations.
    bool BreakBeforeTask = false;

    /// The column limit.
    unsigned ColumnLimit = 100; // NOLINT

    /// If true, compacts bare conditional bodies onto the same line when the
    /// result fits within the column limit.
    bool CompactConditionals = true;

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
