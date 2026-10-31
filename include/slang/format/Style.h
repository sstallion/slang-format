// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

//! @file Style.h
//! @brief Formatting style options and types.

#pragma once

#include "slang/format/Export.h"

#include <string>

namespace slang::format {

/// Controls alignment behavior for consecutive declarations.
struct SLANG_FORMAT_EXPORT AlignConsecutiveStyle {
    /// If false, disables alignment regardless of other options.
    bool Enabled;

    /// If true, comments do not break alignment groups.
    bool AcrossComments;

    /// If true, empty lines do not break alignment groups.
    bool AcrossEmptyLines;

    /// If true, port list boundaries do not break alignment groups.
    bool AcrossParameterPortList;

    /// If true, use original source spacing to infer whether to align.
    bool InferFromSource;

    /// Maximum number of spaces any line may be padded; 0 means unlimited.
    unsigned MaxPadding;

    /// Minimum number of spaces any line may be padded; 0 means disabled.
    unsigned MinPadding;

    /// If true, align `:` separators in packed dimensions.
    bool AlignColon;

    /// If true, left-pad content within brackets.
    bool PadLeft;

    /// If true, right-pad content within brackets.
    bool PadRight;

    bool operator==(const AlignConsecutiveStyle&) const = default;
};

/// Defines break insertion behavior for procedural blocks.
enum class BlockBreakStyle { Never, Always, OnlyMultiline };

/// Defines alignment behavior after an opening parenthesis.
enum class ParenAlignmentStyle { Align, AlwaysBreak, BlockIndent, DontAlign };

/// Defines the ordering of dimension bounds.
enum class DimensionBoundsStyle { LSBFirst, MSBFirst, Preserve };

/// Defines the separator style for event expressions.
enum class EventSeparatorStyle { Comma, Or, Preserve };

/// Defines line ending characters in formatted output.
enum class LineEndingStyle { CRLF, DeriveCRLF, DeriveLF, LF };

/// Defines spacing behavior around range operators.
enum class RangeSpacingStyle { Always, MatchSource, Never, Preserve };

/// Controls automatic insertion of begin/end around statement bodies.
struct SLANG_FORMAT_EXPORT InsertBeginEndStyle {
    /// If false, disables begin/end insertion regardless of other options.
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
struct SLANG_FORMAT_EXPORT InsertParensStyle {
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

/// Controls whitespace after a closing bracket in declarations.
struct SLANG_FORMAT_EXPORT SpaceAfterBracketsStyle {
    /// If true, insert a space after packed dimension brackets.
    bool PackedDimensions;

    /// If true, insert a space after unpacked dimension brackets.
    bool UnpackedDimensions;

    bool operator==(const SpaceAfterBracketsStyle&) const = default;
};

/// Controls whitespace before an opening bracket in declarations.
struct SLANG_FORMAT_EXPORT SpaceBeforeBracketsStyle {
    /// If true, insert a space before packed dimension brackets.
    bool PackedDimensions;

    /// If true, insert a space before unpacked dimension brackets.
    bool UnpackedDimensions;

    bool operator==(const SpaceBeforeBracketsStyle&) const = default;
};

/// Controls whitespace before an opening parenthesis.
struct SLANG_FORMAT_EXPORT SpaceBeforeParensStyle {
    /// If true, insert a space before parentheses in control statements.
    bool ControlStatements;

    /// If true, insert a space before a parameter port list.
    bool ParameterList;

    /// If true, insert a space before a port list.
    bool PortList;

    bool operator==(const SpaceBeforeParensStyle&) const = default;
};

/// Controls whitespace after a closing parenthesis.
struct SLANG_FORMAT_EXPORT SpaceAfterParensStyle {
    /// If true, insert a space after parentheses in control statements.
    bool ControlStatements;

    /// If true, insert a space after parentheses in event controls.
    bool EventControls;

    bool operator==(const SpaceAfterParensStyle&) const = default;
};

/// Defines all formatting style options.
struct SLANG_FORMAT_EXPORT Style {
    /// Controls alignment of arguments after an opening parenthesis.
    ParenAlignmentStyle AlignAfterOpenParen;

    /// Controls alignment of assignment operators in consecutive declarations.
    AlignConsecutiveStyle AlignConsecutiveAssignments;

    /// Controls alignment of signal names in consecutive declarations.
    AlignConsecutiveStyle AlignConsecutiveDeclarations;

    /// Controls alignment of packed dimensions in consecutive declarations.
    AlignConsecutiveStyle AlignConsecutivePackedDimensions;

    /// Controls alignment of consecutive named port connections.
    AlignConsecutiveStyle AlignConsecutivePortConnections;

    /// Controls alignment of consecutive timing controls.
    AlignConsecutiveStyle AlignConsecutiveTimingControls;

    /// Controls alignment of trailing comments.
    AlignConsecutiveStyle AlignTrailingComments;

    /// If true, constraint blocks are joined onto one line when possible.
    bool AllowShortConstraintBlocksOnASingleLine;

    /// If true, coverpoints are joined onto one line when possible.
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

    /// If true, inserts a newline after assertion labels.
    bool BreakAfterAssertionLabel;

    /// If true, inserts a newline after begin statements.
    bool BreakAfterBegin;

    /// Controls insertion of a newline after initial statements.
    BlockBreakStyle BreakAfterInitial;

    /// Controls insertion of a newline before always statements.
    BlockBreakStyle BreakBeforeAlways;

    /// If true, insert a newline before end statements.
    bool BreakBeforeEnd;

    /// If true, insert a newline before function declarations.
    bool BreakBeforeFunction;

    /// Controls insertion of a newline before initial blocks.
    BlockBreakStyle BreakBeforeInitial;

    /// If true, insert a newline before specify blocks.
    bool BreakBeforeSpecifyBlock;

    /// If true, insert a newline before task declarations.
    bool BreakBeforeTask;

    /// The column limit.
    unsigned ColumnLimit;

    /// If true, compacts control statements onto the same line when possible.
    bool CompactControlStatements;

    /// If true, compacts time units onto the same line when possible.
    bool CompactTimeUnits;

    /// Number of spaces indented for continuation lines.
    unsigned ContinuationIndentWidth;

    /// Controls the separator used in event expressions.
    EventSeparatorStyle EventSeparator;

    /// If true, indent statements after a case label.
    bool IndentCaseItem;

    /// Number of spaces for each indentation level.
    unsigned IndentWidth;

    /// Controls insertion of begin/end statements.
    InsertBeginEndStyle InsertBeginEnd;

    /// Controls insertion of parentheses.
    InsertParensStyle InsertParens;

    /// Controls line ending characters.
    LineEndingStyle LineEnding;

    /// Maximum number of consecutive empty lines to keep.
    unsigned MaxEmptyLinesToKeep;

    /// Disables formatting for lines matching this pattern.
    std::string OneLineFormatOffRegex;

    /// If true, enforce one statement per line.
    bool OneStatementPerLine;

    /// Controls the order of packed dimension bounds.
    DimensionBoundsStyle PackedDimensionBounds;

    /// Number of spaces for parameter port list items in module declarations.
    unsigned ParameterPortListIndentWidth;

    /// Controls spacing around `:` in range expressions.
    RangeSpacingStyle RangeSpacing;

    /// If true, enables enforcement of @ref Style.MaxEmptyLinesToKeep.
    bool RemoveEmptyLines;

    /// Inserts a whitespace after an always keyword.
    bool SpaceAfterAlways;

    /// Controls whitespace after a closing bracket in declarations.
    SpaceAfterBracketsStyle SpaceAfterBrackets;

    /// Inserts a whitespace after case item colons.
    bool SpaceAfterCaseColon;

    /// Inserts whitespace after commas.
    bool SpaceAfterComma;

    /// Controls whitespace after closing parentheses.
    SpaceAfterParensStyle SpaceAfterParens;

    /// Inserts whitespace after semicolons.
    bool SpaceAfterSemicolon;

    /// Inserts whitespace around assignment, binary, and ternary operators.
    bool SpaceAroundOperators;

    /// Controls whitespace before an opening square bracket in declarations.
    SpaceBeforeBracketsStyle SpaceBeforeBrackets;

    /// Inserts whitespace before case item colons.
    bool SpaceBeforeCaseColon;

    /// Controls whitespace before opening parentheses.
    SpaceBeforeParensStyle SpaceBeforeParens;

    /// Number of spaces before a trailing comment.
    unsigned SpacesBeforeTrailingComments;

    /// Normalizes whitespace inside braces.
    bool SpacesInBraces;

    /// Normalizes whitespace inside brackets.
    bool SpacesInBrackets;

    /// Normalizes whitespace inside parentheses.
    bool SpacesInParens;

    /// Controls the order of unpacked dimension bounds.
    DimensionBoundsStyle UnpackedDimensionBounds;

    bool operator==(const Style&) const = default;
};

} // namespace slang::format
