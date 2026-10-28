// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Style.h"
#include "TestHelper.h"

#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

using namespace slang::format;

TEST(DumpConfiguration, DefaultStyle) {
    // clang-format off
    EXPECT_EQ(dumpConfiguration(getDefaultStyle()), dedent(R"(
        ---
        AlignAfterOpenParen: Align
        AlignConsecutiveAssignments:
          Enabled: true
          AcrossComments: false
          AcrossEmptyLines: false
          AcrossParameterPortList: false
          InferFromSource: true
          MaxPadding: 2
          MinPadding: 2
        AlignConsecutiveDeclarations:
          Enabled: true
          AcrossComments: false
          AcrossEmptyLines: false
          AcrossParameterPortList: false
          InferFromSource: true
        AlignConsecutivePackedDimensions:
          Enabled: true
          AcrossComments: false
          AcrossEmptyLines: false
          AcrossParameterPortList: false
          AlignColon: false
          InferFromSource: true
          PadLeft: true
          PadRight: false
        AlignConsecutivePortConnections:
          Enabled: true
          AcrossComments: false
          AcrossEmptyLines: false
          AcrossParameterPortList: false
          InferFromSource: true
          MaxPadding: 2
          MinPadding: 2
        AlignConsecutiveTimingControls:
          Enabled: false
          AcrossComments: false
          AcrossEmptyLines: false
        AlignTrailingComments:
          Enabled: false
          AcrossComments: false
          AcrossEmptyLines: false
          AcrossParameterPortList: false
        AllowShortConstraintBlocksOnASingleLine: true
        AllowShortCoverpointsOnASingleLine: true
        BinPackArguments: true
        BinPackParameters: true
        BinPackPortConnections: false
        BreakAfterAlways: OnlyMultiline
        BreakAfterAssertionLabel: true
        BreakAfterBegin: true
        BreakAfterInitial: OnlyMultiline
        BreakBeforeAlways: Never
        BreakBeforeEnd: true
        BreakBeforeFunction: false
        BreakBeforeInitial: Never
        BreakBeforeSpecifyBlock: false
        BreakBeforeTask: false
        ColumnLimit: 100
        CompactControlStatements: true
        CompactTimeUnits: true
        ContinuationIndentWidth: 4
        EventSeparator: Preserve
        IndentCaseItem: false
        IndentWidth: 2
        InsertBeginEnd:
          Enabled: false
          AlwaysStatements: false
          ControlStatements: false
          InitialStatements: false
        InsertParens:
          Delays: false
          ExpressionEvents: false
          ImplicitEvents: false
          NamedEvents: false
        LineEnding: DeriveLF
        MaxEmptyLinesToKeep: 1
        OneLineFormatOffRegex: ""
        OneStatementPerLine: true
        PackedDimensionBounds: Preserve
        ParameterPortListIndentWidth: 4
        RangeSpacing: MatchSource
        RemoveEmptyLines: false
        SpaceAfterAlways: true
        SpaceAfterBrackets:
          PackedDimensions: true
          UnpackedDimensions: false
        SpaceAfterCaseColon: true
        SpaceAfterComma: true
        SpaceAfterParens:
          ControlStatements: true
          EventControls: true
        SpaceAfterSemicolon: true
        SpaceAroundOperators: true
        SpaceBeforeBrackets:
          PackedDimensions: true
          UnpackedDimensions: false
        SpaceBeforeCaseColon: false
        SpaceBeforeParens:
          ControlStatements: true
          ParameterList: true
          PortList: true
        SpacesBeforeTrailingComments: 2
        SpacesInBraces: false
        SpacesInBrackets: false
        SpacesInParens: false
        UnpackedDimensionBounds: Preserve
        ...
    )"));
    // clang-format on
}

TEST(DumpConfiguration, RoundTrip) {
    Style original;
    original.AlignAfterOpenParen = BracketAlignmentStyle::BlockIndent;
    original.AlignConsecutiveDeclarations.AcrossEmptyLines = true;
    original.AlignConsecutiveDeclarations.Enabled = true;
    original.AlignConsecutivePackedDimensions.AlignColon = true;
    original.AlignConsecutivePackedDimensions.InferFromSource = true;
    original.AlignConsecutivePackedDimensions.PadLeft = true;
    original.AlignConsecutivePackedDimensions.PadRight = true;
    original.AlignConsecutivePortConnections.Enabled = true;
    original.AlignConsecutivePortConnections.MaxPadding = 3;
    original.AlignConsecutivePortConnections.MinPadding = 1;
    original.AlignConsecutiveTimingControls.AcrossEmptyLines = true;
    original.AlignConsecutiveTimingControls.Enabled = true;
    original.AlignTrailingComments.AcrossEmptyLines = true;
    original.AlignTrailingComments.Enabled = true;
    original.AllowShortConstraintBlocksOnASingleLine = false;
    original.AllowShortCoverpointsOnASingleLine = false;
    original.BinPackArguments = false;
    original.BinPackParameters = false;
    original.BreakAfterAlways = BlockBreakStyle::Never;
    original.BreakAfterBegin = false;
    original.BreakAfterInitial = BlockBreakStyle::Always;
    original.BreakBeforeAlways = BlockBreakStyle::Never;
    original.BreakBeforeEnd = false;
    original.BreakBeforeFunction = false;
    original.BreakBeforeInitial = BlockBreakStyle::Never;
    original.BreakBeforeSpecifyBlock = false;
    original.BreakBeforeTask = false;
    original.CompactControlStatements = false;
    original.CompactTimeUnits = false;
    original.ContinuationIndentWidth = 4;
    original.EventSeparator = EventSeparatorStyle::Or;
    original.IndentCaseItem = true;
    original.IndentWidth = 4;
    original.InsertBeginEnd.AlwaysStatements = false;
    original.InsertBeginEnd.ControlStatements = false;
    original.InsertBeginEnd.Enabled = true;
    original.InsertBeginEnd.InitialStatements = false;
    original.InsertParens.Delays = true;
    original.InsertParens.ExpressionEvents = true;
    original.InsertParens.ImplicitEvents = true;
    original.InsertParens.NamedEvents = true;
    original.LineEnding = LineEndingStyle::LF;
    original.MaxEmptyLinesToKeep = 3;
    original.OneLineFormatOffRegex = ".*test.*";
    original.OneStatementPerLine = false;
    original.PackedDimensionBounds = DimensionBoundsStyle::LSBFirst;
    original.ParameterPortListIndentWidth = 4;
    original.RangeSpacing = RangeSpacingStyle::Never;
    original.RemoveEmptyLines = true;
    original.SpaceAfterAlways = false;
    original.SpaceAfterBrackets.PackedDimensions = false;
    original.SpaceAfterBrackets.UnpackedDimensions = true;
    original.SpaceAfterCaseColon = false;
    original.SpaceAfterComma = true;
    original.SpaceAfterParens.ControlStatements = false;
    original.SpaceAfterParens.EventControls = false;
    original.SpaceAfterSemicolon = true;
    original.SpaceAroundOperators = false;
    original.SpaceBeforeBrackets.PackedDimensions = false;
    original.SpaceBeforeBrackets.UnpackedDimensions = true;
    original.SpaceBeforeCaseColon = true;
    original.SpaceBeforeParens.ControlStatements = false;
    original.SpaceBeforeParens.ParameterList = false;
    original.SpaceBeforeParens.PortList = false;
    original.SpacesBeforeTrailingComments = 3;
    original.SpacesInBraces = true;
    original.SpacesInBrackets = true;
    original.SpacesInParens = true;
    original.UnpackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    const auto yaml = dumpConfiguration(original);
    const auto node = YAML::Load(yaml);
    Style parsed;
    parseConfiguration(node, parsed);
    EXPECT_EQ(parsed, original);
}

TEST(GetDefaultStyle, MatchesDefaultConstructed) {
    const Style style = getDefaultStyle();
    EXPECT_EQ(style, Style{});
}

TEST(GetStyle, LoadsFromCurrentDir) {
    const auto loader = makeFileLoader({{"/a/b/c/.slang-format", "{}"}});

    Style expected;
    expected.ContinuationIndentWidth = 2;
    expected.ParameterPortListIndentWidth = 2;
    EXPECT_EQ(getStyle("/a/b/c", loader), expected);
}

TEST(GetStyle, ReturnsDefaultWhenNoFile) {
    const auto loader = makeFileLoader({});
    EXPECT_EQ(getStyle("/a/b/c", loader), Style{});
}

TEST(GetStyle, StopsAtFirstMatch) {
    int callCount{0};
    auto loader = [&callCount](const std::filesystem::path& p) -> std::optional<std::string> {
        callCount++;
        if (p == std::filesystem::path("/a/b/_slang-format")) {
            return "{}";
        }

        return std::nullopt;
    };

    getStyle("/a/b/c", loader);
    EXPECT_EQ(callCount, 4);
}

TEST(GetStyle, WalksHierarchy) {
    const auto loader = makeFileLoader({{"/a/.slang-format", "{}"}});

    Style expected;
    expected.ContinuationIndentWidth = 2;
    expected.ParameterPortListIndentWidth = 2;
    EXPECT_EQ(getStyle("/a/b/c", loader), expected);
}

TEST(ParseConfiguration, AcceptsEmptyMap) {
    Style style;
    parseConfiguration(YAML::Load("{}"), style);
}

TEST(ParseConfiguration, AlignAfterOpenParen) {
    Style style;

    parseConfiguration(YAML::Load("AlignAfterOpenParen: Align"), style);
    EXPECT_EQ(style.AlignAfterOpenParen, BracketAlignmentStyle::Align);

    parseConfiguration(YAML::Load("AlignAfterOpenParen: AlwaysBreak"), style);
    EXPECT_EQ(style.AlignAfterOpenParen, BracketAlignmentStyle::AlwaysBreak);

    parseConfiguration(YAML::Load("AlignAfterOpenParen: BlockIndent"), style);
    EXPECT_EQ(style.AlignAfterOpenParen, BracketAlignmentStyle::BlockIndent);

    parseConfiguration(YAML::Load("AlignAfterOpenParen: DontAlign"), style);
    EXPECT_EQ(style.AlignAfterOpenParen, BracketAlignmentStyle::DontAlign);
}

TEST(ParseConfiguration, AlignConsecutiveAssignments) {
    Style style;

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        AlignConsecutiveAssignments:
          Enabled: false
          AcrossComments: true
          AcrossEmptyLines: true
          AcrossParameterPortList: true
          AlignColon: true
          InferFromSource: false
          MaxPadding: 4
          MinPadding: 3
          PadLeft: true
          PadRight: true
    )"));
    // clang-format on

    parseConfiguration(node, style);
    EXPECT_FALSE(style.AlignConsecutiveAssignments.Enabled);
    EXPECT_TRUE(style.AlignConsecutiveAssignments.AcrossComments);
    EXPECT_TRUE(style.AlignConsecutiveAssignments.AcrossEmptyLines);
    EXPECT_TRUE(style.AlignConsecutiveAssignments.AcrossParameterPortList);
    EXPECT_TRUE(style.AlignConsecutiveAssignments.AlignColon);
    EXPECT_FALSE(style.AlignConsecutiveAssignments.InferFromSource);
    EXPECT_EQ(style.AlignConsecutiveAssignments.MaxPadding, 4U);
    EXPECT_EQ(style.AlignConsecutiveAssignments.MinPadding, 3U);
    EXPECT_TRUE(style.AlignConsecutiveAssignments.PadLeft);
    EXPECT_TRUE(style.AlignConsecutiveAssignments.PadRight);
}

TEST(ParseConfiguration, AlignConsecutiveDeclarations) {
    Style style;

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        AlignConsecutiveDeclarations:
          Enabled: false
    )"));
    // clang-format on

    parseConfiguration(node, style);
    EXPECT_FALSE(style.AlignConsecutiveDeclarations.Enabled);
}

TEST(ParseConfiguration, AlignConsecutivePackedDimensions) {
    Style style;

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        AlignConsecutivePackedDimensions:
          Enabled: false
    )"));
    // clang-format on

    parseConfiguration(node, style);
    EXPECT_FALSE(style.AlignConsecutivePackedDimensions.Enabled);
}

TEST(ParseConfiguration, AlignConsecutivePortConnections) {
    Style style;

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        AlignConsecutivePortConnections:
          Enabled: false
    )"));
    // clang-format on

    parseConfiguration(node, style);
    EXPECT_FALSE(style.AlignConsecutivePortConnections.Enabled);
}

TEST(ParseConfiguration, AlignConsecutiveTimingControls) {
    Style style;

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        AlignConsecutiveTimingControls:
          Enabled: true
    )"));
    // clang-format on

    parseConfiguration(node, style);
    EXPECT_TRUE(style.AlignConsecutiveTimingControls.Enabled);
}

TEST(ParseConfiguration, AlignTrailingComments) {
    Style style;

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        AlignTrailingComments:
          Enabled: true
    )"));
    // clang-format on

    parseConfiguration(node, style);
    EXPECT_TRUE(style.AlignTrailingComments.Enabled);
}

TEST(ParseConfiguration, AllowShortConstraintBlocksOnASingleLine) {
    Style style;
    parseConfiguration(YAML::Load("AllowShortConstraintBlocksOnASingleLine: false"), style);
    EXPECT_FALSE(style.AllowShortConstraintBlocksOnASingleLine);
}

TEST(ParseConfiguration, AllowShortCoverpointsOnASingleLine) {
    Style style;
    parseConfiguration(YAML::Load("AllowShortCoverpointsOnASingleLine: false"), style);
    EXPECT_FALSE(style.AllowShortCoverpointsOnASingleLine);
}

TEST(ParseConfiguration, BinPackArguments) {
    Style style;
    parseConfiguration(YAML::Load("BinPackArguments: false"), style);
    EXPECT_FALSE(style.BinPackArguments);
}

TEST(ParseConfiguration, BinPackParameters) {
    Style style;
    parseConfiguration(YAML::Load("BinPackParameters: false"), style);
    EXPECT_FALSE(style.BinPackParameters);
}

TEST(ParseConfiguration, BinPackPortConnections) {
    Style style;
    parseConfiguration(YAML::Load("BinPackPortConnections: true"), style);
    EXPECT_TRUE(style.BinPackPortConnections);
}

TEST(ParseConfiguration, BreakAfterAlways) {
    Style style;

    parseConfiguration(YAML::Load("BreakAfterAlways: Always"), style);
    EXPECT_EQ(style.BreakAfterAlways, BlockBreakStyle::Always);

    parseConfiguration(YAML::Load("BreakAfterAlways: Never"), style);
    EXPECT_EQ(style.BreakAfterAlways, BlockBreakStyle::Never);

    parseConfiguration(YAML::Load("BreakAfterAlways: OnlyMultiline"), style);
    EXPECT_EQ(style.BreakAfterAlways, BlockBreakStyle::OnlyMultiline);
}

TEST(ParseConfiguration, BreakAfterAssertionLabel) {
    Style style;
    parseConfiguration(YAML::Load("BreakAfterAssertionLabel: false"), style);
    EXPECT_FALSE(style.BreakAfterAssertionLabel);
}

TEST(ParseConfiguration, BreakAfterBegin) {
    Style style;
    parseConfiguration(YAML::Load("BreakAfterBegin: false"), style);
    EXPECT_FALSE(style.BreakAfterBegin);
}

TEST(ParseConfiguration, BreakAfterInitial) {
    Style style;

    parseConfiguration(YAML::Load("BreakAfterInitial: Always"), style);
    EXPECT_EQ(style.BreakAfterInitial, BlockBreakStyle::Always);

    parseConfiguration(YAML::Load("BreakAfterInitial: Never"), style);
    EXPECT_EQ(style.BreakAfterInitial, BlockBreakStyle::Never);

    parseConfiguration(YAML::Load("BreakAfterInitial: OnlyMultiline"), style);
    EXPECT_EQ(style.BreakAfterInitial, BlockBreakStyle::OnlyMultiline);
}

TEST(ParseConfiguration, BreakBeforeAlways) {
    Style style;

    parseConfiguration(YAML::Load("BreakBeforeAlways: Always"), style);
    EXPECT_EQ(style.BreakBeforeAlways, BlockBreakStyle::Always);

    parseConfiguration(YAML::Load("BreakBeforeAlways: Never"), style);
    EXPECT_EQ(style.BreakBeforeAlways, BlockBreakStyle::Never);

    parseConfiguration(YAML::Load("BreakBeforeAlways: OnlyMultiline"), style);
    EXPECT_EQ(style.BreakBeforeAlways, BlockBreakStyle::OnlyMultiline);
}

TEST(ParseConfiguration, BreakBeforeEnd) {
    Style style;
    parseConfiguration(YAML::Load("BreakBeforeEnd: false"), style);
    EXPECT_FALSE(style.BreakBeforeEnd);
}

TEST(ParseConfiguration, BreakBeforeFunction) {
    Style style;
    parseConfiguration(YAML::Load("BreakBeforeFunction: true"), style);
    EXPECT_TRUE(style.BreakBeforeFunction);
}

TEST(ParseConfiguration, BreakBeforeInitial) {
    Style style;

    parseConfiguration(YAML::Load("BreakBeforeInitial: Always"), style);
    EXPECT_EQ(style.BreakBeforeInitial, BlockBreakStyle::Always);

    parseConfiguration(YAML::Load("BreakBeforeInitial: Never"), style);
    EXPECT_EQ(style.BreakBeforeInitial, BlockBreakStyle::Never);

    parseConfiguration(YAML::Load("BreakBeforeInitial: OnlyMultiline"), style);
    EXPECT_EQ(style.BreakBeforeInitial, BlockBreakStyle::OnlyMultiline);
}

TEST(ParseConfiguration, BreakBeforeSpecifyBlock) {
    Style style;
    parseConfiguration(YAML::Load("BreakBeforeSpecifyBlock: true"), style);
    EXPECT_TRUE(style.BreakBeforeSpecifyBlock);
}

TEST(ParseConfiguration, BreakBeforeTask) {
    Style style;
    parseConfiguration(YAML::Load("BreakBeforeTask: true"), style);
    EXPECT_TRUE(style.BreakBeforeTask);
}

TEST(ParseConfiguration, ColumnLimit) {
    Style style;
    parseConfiguration(YAML::Load("ColumnLimit: 80"), style);
    EXPECT_EQ(style.ColumnLimit, 80U);
}

TEST(ParseConfiguration, CompactControlStatements) {
    Style style;
    parseConfiguration(YAML::Load("CompactControlStatements: false"), style);
    EXPECT_FALSE(style.CompactControlStatements);
}

TEST(ParseConfiguration, CompactTimeUnits) {
    Style style;
    parseConfiguration(YAML::Load("CompactTimeUnits: false"), style);
    EXPECT_FALSE(style.CompactTimeUnits);
}

TEST(ParseConfiguration, ContinuationIndentWidth) {
    Style style;
    parseConfiguration(YAML::Load("ContinuationIndentWidth: 6"), style);
    EXPECT_EQ(style.ContinuationIndentWidth, 6U);
}

TEST(ParseConfiguration, ContinuationIndentWidthDefaultsToIndentWidth) {
    Style style;
    parseConfiguration(YAML::Load("IndentWidth: 6"), style);
    EXPECT_EQ(style.IndentWidth, 6U);
    EXPECT_EQ(style.ContinuationIndentWidth, 6U);
    EXPECT_EQ(style.ParameterPortListIndentWidth, 6U);
}

TEST(ParseConfiguration, EventSeparator) {
    Style style;

    parseConfiguration(YAML::Load("EventSeparator: Comma"), style);
    EXPECT_EQ(style.EventSeparator, EventSeparatorStyle::Comma);

    parseConfiguration(YAML::Load("EventSeparator: Or"), style);
    EXPECT_EQ(style.EventSeparator, EventSeparatorStyle::Or);

    parseConfiguration(YAML::Load("EventSeparator: Preserve"), style);
    EXPECT_EQ(style.EventSeparator, EventSeparatorStyle::Preserve);
}

TEST(ParseConfiguration, IndentCaseItem) {
    Style style;
    parseConfiguration(YAML::Load("IndentCaseItem: true"), style);
    EXPECT_TRUE(style.IndentCaseItem);
}

TEST(ParseConfiguration, IndentWidth) {
    Style style;
    parseConfiguration(YAML::Load("IndentWidth: 4"), style);
    EXPECT_EQ(style.IndentWidth, 4U);
}

TEST(ParseConfiguration, InsertBeginEnd) {
    Style style;

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        InsertBeginEnd:
          Enabled: true
          AlwaysStatements: true
          ControlStatements: true
          InitialStatements: true
    )"));
    // clang-format on

    parseConfiguration(node, style);
    EXPECT_TRUE(style.InsertBeginEnd.Enabled);
    EXPECT_TRUE(style.InsertBeginEnd.AlwaysStatements);
    EXPECT_TRUE(style.InsertBeginEnd.ControlStatements);
    EXPECT_TRUE(style.InsertBeginEnd.InitialStatements);
}

TEST(ParseConfiguration, InsertParens) {
    Style style;

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        InsertParens:
          Delays: true
          ExpressionEvents: true
          ImplicitEvents: true
          NamedEvents: true
    )"));
    // clang-format on

    parseConfiguration(node, style);
    EXPECT_TRUE(style.InsertParens.Delays);
    EXPECT_TRUE(style.InsertParens.ExpressionEvents);
    EXPECT_TRUE(style.InsertParens.ImplicitEvents);
    EXPECT_TRUE(style.InsertParens.NamedEvents);
}

TEST(ParseConfiguration, LineEnding) {
    Style style;

    parseConfiguration(YAML::Load("LineEnding: CRLF"), style);
    EXPECT_EQ(style.LineEnding, LineEndingStyle::CRLF);

    parseConfiguration(YAML::Load("LineEnding: DeriveCRLF"), style);
    EXPECT_EQ(style.LineEnding, LineEndingStyle::DeriveCRLF);

    parseConfiguration(YAML::Load("LineEnding: DeriveLF"), style);
    EXPECT_EQ(style.LineEnding, LineEndingStyle::DeriveLF);

    parseConfiguration(YAML::Load("LineEnding: LF"), style);
    EXPECT_EQ(style.LineEnding, LineEndingStyle::LF);
}

TEST(ParseConfiguration, MaxEmptyLinesToKeep) {
    Style style;
    parseConfiguration(YAML::Load("MaxEmptyLinesToKeep: 2"), style);
    EXPECT_EQ(style.MaxEmptyLinesToKeep, 2U);
}

TEST(ParseConfiguration, OneLineFormatOffRegex) {
    Style style;
    parseConfiguration(YAML::Load("OneLineFormatOffRegex: '.*foo.*'"), style);
    EXPECT_EQ(style.OneLineFormatOffRegex, ".*foo.*");
}

TEST(ParseConfiguration, OneStatementPerLine) {
    Style style;
    parseConfiguration(YAML::Load("OneStatementPerLine: false"), style);
    EXPECT_FALSE(style.OneStatementPerLine);
}

TEST(ParseConfiguration, PackedDimensionBounds) {
    Style style;

    parseConfiguration(YAML::Load("PackedDimensionBounds: LSBFirst"), style);
    EXPECT_EQ(style.PackedDimensionBounds, DimensionBoundsStyle::LSBFirst);

    parseConfiguration(YAML::Load("PackedDimensionBounds: MSBFirst"), style);
    EXPECT_EQ(style.PackedDimensionBounds, DimensionBoundsStyle::MSBFirst);

    parseConfiguration(YAML::Load("PackedDimensionBounds: Preserve"), style);
    EXPECT_EQ(style.PackedDimensionBounds, DimensionBoundsStyle::Preserve);
}

TEST(ParseConfiguration, ParameterPortListIndentWidth) {
    Style style;
    parseConfiguration(YAML::Load("ParameterPortListIndentWidth: 2"), style);
    EXPECT_EQ(style.ParameterPortListIndentWidth, 2U);
}

TEST(ParseConfiguration, ParameterPortListIndentWidthDefaultsToContinuationIndentWidth) {
    Style style;
    parseConfiguration(YAML::Load("ContinuationIndentWidth: 4"), style);
    EXPECT_EQ(style.ContinuationIndentWidth, 4U);
    EXPECT_EQ(style.ParameterPortListIndentWidth, 4U);
}

TEST(ParseConfiguration, RangeSpacing) {
    Style style;

    parseConfiguration(YAML::Load("RangeSpacing: Always"), style);
    EXPECT_EQ(style.RangeSpacing, RangeSpacingStyle::Always);

    parseConfiguration(YAML::Load("RangeSpacing: MatchSource"), style);
    EXPECT_EQ(style.RangeSpacing, RangeSpacingStyle::MatchSource);

    parseConfiguration(YAML::Load("RangeSpacing: Never"), style);
    EXPECT_EQ(style.RangeSpacing, RangeSpacingStyle::Never);

    parseConfiguration(YAML::Load("RangeSpacing: Preserve"), style);
    EXPECT_EQ(style.RangeSpacing, RangeSpacingStyle::Preserve);
}

TEST(ParseConfiguration, RejectsScalar) {
    Style style;
    EXPECT_THROW(parseConfiguration(YAML::Load("42"), style), std::runtime_error);
}

TEST(ParseConfiguration, RejectsSequence) {
    Style style;
    EXPECT_THROW(parseConfiguration(YAML::Load("[1, 2, 3]"), style), std::runtime_error);
}

TEST(ParseConfiguration, RemoveEmptyLines) {
    Style style;
    parseConfiguration(YAML::Load("RemoveEmptyLines: true"), style);
    EXPECT_TRUE(style.RemoveEmptyLines);
}

TEST(ParseConfiguration, SpaceAfterAlways) {
    Style style;
    parseConfiguration(YAML::Load("SpaceAfterAlways: false"), style);
    EXPECT_FALSE(style.SpaceAfterAlways);
}

TEST(ParseConfiguration, SpaceAfterBrackets) {
    Style style;

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        SpaceAfterBrackets:
          PackedDimensions: false
          UnpackedDimensions: true
    )"));
    // clang-format on

    parseConfiguration(node, style);
    EXPECT_FALSE(style.SpaceAfterBrackets.PackedDimensions);
    EXPECT_TRUE(style.SpaceAfterBrackets.UnpackedDimensions);
}

TEST(ParseConfiguration, SpaceAfterCaseColon) {
    Style style;
    parseConfiguration(YAML::Load("SpaceAfterCaseColon: false"), style);
    EXPECT_FALSE(style.SpaceAfterCaseColon);
}

TEST(ParseConfiguration, SpaceAfterComma) {
    Style style;
    parseConfiguration(YAML::Load("SpaceAfterComma: false"), style);
    EXPECT_FALSE(style.SpaceAfterComma);
}

TEST(ParseConfiguration, SpaceAfterParens) {
    Style style;

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        SpaceAfterParens:
          ControlStatements: false
          EventControls: false
    )"));
    // clang-format on

    parseConfiguration(node, style);
    EXPECT_FALSE(style.SpaceAfterParens.ControlStatements);
    EXPECT_FALSE(style.SpaceAfterParens.EventControls);
}

TEST(ParseConfiguration, SpaceAfterSemicolon) {
    Style style;
    parseConfiguration(YAML::Load("SpaceAfterSemicolon: false"), style);
    EXPECT_FALSE(style.SpaceAfterSemicolon);
}

TEST(ParseConfiguration, SpaceAroundOperators) {
    Style style;
    parseConfiguration(YAML::Load("SpaceAroundOperators: false"), style);
    EXPECT_FALSE(style.SpaceAroundOperators);
}

TEST(ParseConfiguration, SpaceBeforeBrackets) {
    Style style;

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        SpaceBeforeBrackets:
          PackedDimensions: false
          UnpackedDimensions: true
    )"));
    // clang-format on

    parseConfiguration(node, style);
    EXPECT_FALSE(style.SpaceBeforeBrackets.PackedDimensions);
    EXPECT_TRUE(style.SpaceBeforeBrackets.UnpackedDimensions);
}

TEST(ParseConfiguration, SpaceBeforeCaseColon) {
    Style style;
    parseConfiguration(YAML::Load("SpaceBeforeCaseColon: true"), style);
    EXPECT_TRUE(style.SpaceBeforeCaseColon);
}

TEST(ParseConfiguration, SpaceBeforeParens) {
    Style style;

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        SpaceBeforeParens:
          ControlStatements: false
          ParameterList: false
          PortList: false
    )"));
    // clang-format on

    parseConfiguration(node, style);
    EXPECT_FALSE(style.SpaceBeforeParens.ControlStatements);
    EXPECT_FALSE(style.SpaceBeforeParens.ParameterList);
    EXPECT_FALSE(style.SpaceBeforeParens.PortList);
}

TEST(ParseConfiguration, SpacesBeforeTrailingComments) {
    Style style;
    parseConfiguration(YAML::Load("SpacesBeforeTrailingComments: 4"), style);
    EXPECT_EQ(style.SpacesBeforeTrailingComments, 4U);
}

TEST(ParseConfiguration, SpacesInBraces) {
    Style style;
    parseConfiguration(YAML::Load("SpacesInBraces: true"), style);
    EXPECT_TRUE(style.SpacesInBraces);
}

TEST(ParseConfiguration, SpacesInBrackets) {
    Style style;
    parseConfiguration(YAML::Load("SpacesInBrackets: true"), style);
    EXPECT_TRUE(style.SpacesInBrackets);
}

TEST(ParseConfiguration, SpacesInParens) {
    Style style;
    parseConfiguration(YAML::Load("SpacesInParens: true"), style);
    EXPECT_TRUE(style.SpacesInParens);
}

TEST(ParseConfiguration, UnpackedDimensionBounds) {
    Style style;

    parseConfiguration(YAML::Load("UnpackedDimensionBounds: LSBFirst"), style);
    EXPECT_EQ(style.UnpackedDimensionBounds, DimensionBoundsStyle::LSBFirst);

    parseConfiguration(YAML::Load("UnpackedDimensionBounds: MSBFirst"), style);
    EXPECT_EQ(style.UnpackedDimensionBounds, DimensionBoundsStyle::MSBFirst);

    parseConfiguration(YAML::Load("UnpackedDimensionBounds: Preserve"), style);
    EXPECT_EQ(style.UnpackedDimensionBounds, DimensionBoundsStyle::Preserve);
}
