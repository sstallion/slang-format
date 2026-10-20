// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include <string>

namespace slang::format {

/// Controls alignment behavior for consecutive declarations.
struct AlignConsecutiveStyle {
    /// If false, disables all alignment regardless of other options.
    bool Enabled;

    /// If true, comments do not break alignment groups.
    bool AcrossComments;

    /// If true, empty lines do not break alignment groups.
    bool AcrossEmptyLines;

    /// If true, port list boundaries do not break alignment groups.
    bool AcrossParameterPortList;

    /// If true, align `:` separators in packed dimensions by right-justifying
    /// the left-side value.
    bool AlignColon;

    /// If true, use original source spacing to infer whether to align:
    /// align when the source was aligned, flush-left when it was not.
    bool InferFromSource;

    /// Maximum number of spaces any line may be padded; 0 means unlimited.
    unsigned MaxPadding;

    /// Minimum padding guarantee: groups with displacement at or below this
    /// threshold are always aligned when InferFromSource is true; 0 disables.
    unsigned MinPadding;

    /// If true, left-pad content within brackets (right-justify).
    bool PadLeft;

    /// If true, right-pad content within brackets (left-justify).
    bool PadRight;

    bool operator==(const AlignConsecutiveStyle&) const = default;
};

enum class BlockBreakStyle { Never, Always, OnlyMultiline };

enum class BracketAlignmentStyle { Align, AlwaysBreak, BlockIndent, DontAlign };

enum class DimensionBoundsStyle { LSBFirst, MSBFirst, Preserve };

enum class EventSeparatorStyle { Comma, Or, Preserve };

enum class LineEndingStyle { CRLF, DeriveCRLF, DeriveLF, LF };

enum class RangeSpacingStyle { Always, MatchSource, Never, Preserve };

/// Controls automatic insertion of begin/end around statement bodies.
struct InsertBeginEndStyle {
    /// If false, disables all begin/end insertion regardless of other options.
    bool Enabled;

    /// If true, wrap bare single-statement bodies of control statements.
    bool ControlStatements;

    /// If true, wrap bare statement bodies of always statements.
    bool AlwaysStatements;

    /// If true, wrap bare statement bodies of initial statements.
    bool InitialStatements;

    bool operator==(const InsertBeginEndStyle&) const = default;
};

/// Controls automatic insertion of parentheses around timing constructs.
struct InsertParensStyle {
    /// If true, insert parentheses around delay expressions.
    bool Delays;

    /// If true, insert parentheses around event expressions.
    bool ExpressionEvents;

    /// If true, insert parentheses around implicit event lists.
    bool ImplicitEvents;

    /// If true, insert parentheses around named event expressions.
    bool NamedEvents;

    bool operator==(const InsertParensStyle&) const = default;
};

/// Controls whitespace after a closing square bracket in declarations.
struct SpaceAfterBracketsStyle {
    /// If true, insert a space after packed dimension brackets.
    bool PackedDimensions;

    /// If true, insert a space after unpacked dimension brackets.
    bool UnpackedDimensions;

    bool operator==(const SpaceAfterBracketsStyle&) const = default;
};

/// Controls whitespace before an opening square bracket in declarations.
struct SpaceBeforeBracketsStyle {
    /// If true, insert a space before packed dimension brackets.
    bool PackedDimensions;

    /// If true, insert a space before unpacked dimension brackets.
    bool UnpackedDimensions;

    bool operator==(const SpaceBeforeBracketsStyle&) const = default;
};

/// Controls whitespace before an opening parenthesis.
struct SpaceBeforeParensStyle {
    /// If true, insert a space before parentheses in control statements.
    bool ControlStatements;

    /// If true, insert a space before a parameter port list.
    bool ParameterList;

    /// If true, insert a space before a port list.
    bool PortList;

    bool operator==(const SpaceBeforeParensStyle&) const = default;
};

/// Controls whitespace after a closing parenthesis.
struct SpaceAfterParensStyle {
    /// If true, insert a space after parentheses in control statements.
    bool ControlStatements;

    /// If true, insert a space after parentheses in event controls.
    bool EventControls;

    bool operator==(const SpaceAfterParensStyle&) const = default;
};

/// Defines all formatting style options.
struct Style {
    /// Controls alignment of arguments after an opening parenthesis in function calls, system task
    /// calls, and constructor invocations.
    BracketAlignmentStyle AlignAfterOpenParen;

    /// Controls alignment of assignment operators in consecutive declarations.
    AlignConsecutiveStyle AlignConsecutiveAssignments;

    /// Controls alignment of signal names in consecutive declarations.
    AlignConsecutiveStyle AlignConsecutiveDeclarations;

    /// Controls alignment of packed dimensions in consecutive declarations.
    AlignConsecutiveStyle AlignConsecutivePackedDimensions;

    /// Controls alignment of opening parentheses in consecutive named port connections.
    AlignConsecutiveStyle AlignConsecutivePortConnections;

    /// Controls alignment of signal names and assignment operators in consecutive timing controls.
    AlignConsecutiveStyle AlignConsecutiveTimingControls;

    /// Controls alignment of trailing comments in consecutive lines.
    AlignConsecutiveStyle AlignTrailingComments;

    /// If true, constraint blocks with a single item are joined onto one line.
    bool AllowShortConstraintBlocksOnASingleLine;

    /// If true, coverpoint and cover cross bodies are joined onto one line when they fit.
    bool AllowShortCoverpointsOnASingleLine;

    /// If true, arguments are bin-packed when they exceed the column limit. If false, each argument
    /// is placed on its own line.
    bool BinPackArguments;

    /// If true, function and task port parameters are bin-packed. If false, each parameter is
    /// placed on its own line.
    bool BinPackParameters;

    /// If true, instance port connections are bin-packed. If false, each port connection is placed
    /// on its own line.
    bool BinPackPortConnections;

    /// Controls insertion of a newline between always and its body.
    BlockBreakStyle BreakAfterAlways;

    /// If true, inserts a newline after assertion labels on concurrent assertions and immediate
    /// assertions with else clauses.
    bool BreakAfterAssertionLabel;

    /// If true, enforce a newline after every begin statement.
    bool BreakAfterBegin;

    /// Controls insertion of a newline between initial and its body.
    BlockBreakStyle BreakAfterInitial;

    /// Controls insertion of a blank line before always and its body.
    BlockBreakStyle BreakBeforeAlways;

    /// If true, enforce a newline before every end statement.
    bool BreakBeforeEnd;

    /// If true, insert a blank line before function declarations.
    bool BreakBeforeFunction;

    /// Controls insertion of a blank line before initial and its body.
    BlockBreakStyle BreakBeforeInitial;

    /// If true, insert a blank line before specify blocks.
    bool BreakBeforeSpecifyBlock;

    /// If true, insert a blank line before task declarations.
    bool BreakBeforeTask;

    /// The column limit.
    unsigned ColumnLimit;

    /// If true, compacts bare control statement bodies onto the same line when the result fits
    /// within the column limit.
    bool CompactControlStatements;

    /// If true, keeps consecutive timeunit and timeprecision declarations on the same line.
    bool CompactTimeUnits;

    /// Number of spaces added for continuation lines relative to the enclosing statement.
    unsigned ContinuationIndentWidth;

    /// Controls the separator used in event expressions.
    EventSeparatorStyle EventSeparator;

    /// If true, indent statements following a case label when they appear on the next line.
    bool IndentCaseItem;

    /// Number of spaces for each indentation level.
    unsigned IndentWidth;

    /// Controls insertion of begin/end around bare statements.
    InsertBeginEndStyle InsertBeginEnd;

    /// Controls insertion of parentheses around timing constructs.
    InsertParensStyle InsertParens;

    /// Controls line ending characters in formatted output.
    LineEndingStyle LineEnding;

    /// Maximum number of consecutive empty lines to keep.
    unsigned MaxEmptyLinesToKeep;

    /// Disables formatting for any line matching this pattern.
    std::string OneLineFormatOffRegex;

    /// If true, enforce one statement per line.
    bool OneStatementPerLine;

    /// Controls the order of packed dimension bounds.
    DimensionBoundsStyle PackedDimensionBounds;

    /// Number of spaces for parameter port list items in module declarations.
    unsigned ParameterPortListIndentWidth;

    /// Controls spacing around `:` in range expressions and declared dimensions.
    RangeSpacingStyle RangeSpacing;

    /// If true, enables enforcement of MaxEmptyLinesToKeep.
    bool RemoveEmptyLines;

    /// Normalizes whitespace after an always keyword.
    bool SpaceAfterAlways;

    /// Controls whitespace after a closing square bracket in declarations.
    SpaceAfterBracketsStyle SpaceAfterBrackets;

    /// Normalizes whitespace after a case item colon.
    bool SpaceAfterCaseColon;

    /// Normalizes whitespace around commas.
    bool SpaceAfterComma;

    /// Controls whitespace after a closing parenthesis.
    SpaceAfterParensStyle SpaceAfterParens;

    /// Normalizes whitespace around semicolons.
    bool SpaceAfterSemicolon;

    /// Normalizes whitespace around assignment, binary, and ternary operators.
    bool SpaceAroundOperators;

    /// Controls whitespace before an opening square bracket in declarations.
    SpaceBeforeBracketsStyle SpaceBeforeBrackets;

    /// Normalizes whitespace before a case item colon.
    bool SpaceBeforeCaseColon;

    /// Controls whitespace before an opening parenthesis.
    SpaceBeforeParensStyle SpaceBeforeParens;

    /// Minimum number of spaces before a trailing comment.
    unsigned SpacesBeforeTrailingComments;

    /// Normalizes whitespace inside curly braces.
    bool SpacesInBraces;

    /// Normalizes whitespace inside square brackets in declarations.
    bool SpacesInBrackets;

    /// Normalizes whitespace inside parentheses (space after `(` and before `)`).
    bool SpacesInParens;

    /// Controls the order of unpacked dimension bounds.
    DimensionBoundsStyle UnpackedDimensionBounds;

    bool operator==(const Style&) const = default;
};

} // namespace slang::format
