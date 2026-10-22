// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "ConfigFile.h"
#include "FileLoaderHelper.h"
#include "slang/format/Style.h"
#include "slang/format/styles/Default.h"
#include "slang/util/Dedent.h"

#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

using namespace slang::format;
using namespace slang::format::style;
using slang::dedent;

TEST(DumpConfig, DefaultStyle) {
    // clang-format off
    EXPECT_EQ(dumpConfig(getDefaultStyle()), dedent(R"(
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

TEST(DumpConfig, RoundTrip) {
    auto original = getDefaultStyle();
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

    const auto yaml = dumpConfig(original);
    const auto node = YAML::Load(yaml);
    auto parsed = getDefaultStyle();
    parseConfig(node, parsed);
    EXPECT_EQ(parsed, original);
}

TEST(LoadConfig, AppliesBasedOnStyle) {
    // clang-format off
    const auto loader = makeFileLoader({{"/a/b/c/.slang-format", dedent(R"(
        BasedOnStyle: Default
        IndentWidth: 4
    )")}});
    // clang-format on

    auto expected = getDefaultStyle();
    expected.IndentWidth = 4;
    expected.ContinuationIndentWidth = 4;
    expected.ParameterPortListIndentWidth = 4;
    EXPECT_EQ(loadConfig("/a/b/c", loader), expected);
}

TEST(LoadConfig, LoadsFromCurrentDir) {
    const auto loader = makeFileLoader({{"/a/b/c/.slang-format", "{}"}});

    auto expected = getDefaultStyle();
    expected.ContinuationIndentWidth = 2;
    expected.ParameterPortListIndentWidth = 2;
    EXPECT_EQ(loadConfig("/a/b/c", loader), expected);
}

TEST(LoadConfig, RejectsUnknownBasedOnStyle) {
    const auto loader = makeFileLoader({{"/a/b/c/.slang-format", "BasedOnStyle: Unknown"}});
    EXPECT_THROW(loadConfig("/a/b/c", loader), std::runtime_error);
}

TEST(LoadConfig, ReturnsDefaultWhenNoFile) {
    const auto loader = makeFileLoader({});
    EXPECT_EQ(loadConfig("/a/b/c", loader), getDefaultStyle());
}

TEST(LoadConfig, StopsAtFirstMatch) {
    int callCount{0};
    auto loader = [&callCount](const std::filesystem::path& p) -> std::optional<std::string> {
        callCount++;
        if (p == std::filesystem::path("/a/b/_slang-format")) {
            return "{}";
        }

        return std::nullopt;
    };

    loadConfig("/a/b/c", loader);
    EXPECT_EQ(callCount, 4);
}

TEST(LoadConfig, WalksHierarchy) {
    const auto loader = makeFileLoader({{"/a/.slang-format", "{}"}});

    auto expected = getDefaultStyle();
    expected.ContinuationIndentWidth = 2;
    expected.ParameterPortListIndentWidth = 2;
    EXPECT_EQ(loadConfig("/a/b/c", loader), expected);
}

TEST(ParseConfig, AcceptsEmptyMap) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("{}"), style);
}

TEST(ParseConfig, AlignAfterOpenParen) {
    auto style = getDefaultStyle();

    parseConfig(YAML::Load("AlignAfterOpenParen: Align"), style);
    EXPECT_EQ(style.AlignAfterOpenParen, BracketAlignmentStyle::Align);

    parseConfig(YAML::Load("AlignAfterOpenParen: AlwaysBreak"), style);
    EXPECT_EQ(style.AlignAfterOpenParen, BracketAlignmentStyle::AlwaysBreak);

    parseConfig(YAML::Load("AlignAfterOpenParen: BlockIndent"), style);
    EXPECT_EQ(style.AlignAfterOpenParen, BracketAlignmentStyle::BlockIndent);

    parseConfig(YAML::Load("AlignAfterOpenParen: DontAlign"), style);
    EXPECT_EQ(style.AlignAfterOpenParen, BracketAlignmentStyle::DontAlign);
}

TEST(ParseConfig, AlignConsecutiveAssignments) {
    auto style = getDefaultStyle();

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

    parseConfig(node, style);
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

TEST(ParseConfig, AlignConsecutiveDeclarations) {
    auto style = getDefaultStyle();

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        AlignConsecutiveDeclarations:
          Enabled: false
    )"));
    // clang-format on

    parseConfig(node, style);
    EXPECT_FALSE(style.AlignConsecutiveDeclarations.Enabled);
}

TEST(ParseConfig, AlignConsecutivePackedDimensions) {
    auto style = getDefaultStyle();

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        AlignConsecutivePackedDimensions:
          Enabled: false
    )"));
    // clang-format on

    parseConfig(node, style);
    EXPECT_FALSE(style.AlignConsecutivePackedDimensions.Enabled);
}

TEST(ParseConfig, AlignConsecutivePortConnections) {
    auto style = getDefaultStyle();

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        AlignConsecutivePortConnections:
          Enabled: false
    )"));
    // clang-format on

    parseConfig(node, style);
    EXPECT_FALSE(style.AlignConsecutivePortConnections.Enabled);
}

TEST(ParseConfig, AlignConsecutiveTimingControls) {
    auto style = getDefaultStyle();

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        AlignConsecutiveTimingControls:
          Enabled: true
    )"));
    // clang-format on

    parseConfig(node, style);
    EXPECT_TRUE(style.AlignConsecutiveTimingControls.Enabled);
}

TEST(ParseConfig, AlignTrailingComments) {
    auto style = getDefaultStyle();

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        AlignTrailingComments:
          Enabled: true
    )"));
    // clang-format on

    parseConfig(node, style);
    EXPECT_TRUE(style.AlignTrailingComments.Enabled);
}

TEST(ParseConfig, AllowShortConstraintBlocksOnASingleLine) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("AllowShortConstraintBlocksOnASingleLine: false"), style);
    EXPECT_FALSE(style.AllowShortConstraintBlocksOnASingleLine);
}

TEST(ParseConfig, AllowShortCoverpointsOnASingleLine) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("AllowShortCoverpointsOnASingleLine: false"), style);
    EXPECT_FALSE(style.AllowShortCoverpointsOnASingleLine);
}

TEST(ParseConfig, BasedOnStyle) {
    auto style = getDefaultStyle();
    style.IndentWidth = 4;
    parseConfig(YAML::Load("BasedOnStyle: Default"), style);

    auto expected = getDefaultStyle();
    expected.ContinuationIndentWidth = 2;
    expected.ParameterPortListIndentWidth = 2;
    EXPECT_EQ(style, expected);
}

TEST(ParseConfig, BasedOnStyleAppliesBeforeOtherKeys) {
    auto style = getDefaultStyle();

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        IndentWidth: 4
        BasedOnStyle: Default
    )"));
    // clang-format on

    parseConfig(node, style);
    EXPECT_EQ(style.IndentWidth, 4U);
    EXPECT_EQ(style.ContinuationIndentWidth, 4U);
    EXPECT_EQ(style.ParameterPortListIndentWidth, 4U);
}

TEST(ParseConfig, BasedOnStyleIsCaseInsensitive) {
    auto style = getDefaultStyle();
    style.IndentWidth = 4;
    parseConfig(YAML::Load("BasedOnStyle: default"), style);
    EXPECT_EQ(style.IndentWidth, 2U);

    style.IndentWidth = 4;
    parseConfig(YAML::Load("BasedOnStyle: DEFAULT"), style);
    EXPECT_EQ(style.IndentWidth, 2U);
}

TEST(ParseConfig, BinPackArguments) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("BinPackArguments: false"), style);
    EXPECT_FALSE(style.BinPackArguments);
}

TEST(ParseConfig, BinPackParameters) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("BinPackParameters: false"), style);
    EXPECT_FALSE(style.BinPackParameters);
}

TEST(ParseConfig, BinPackPortConnections) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("BinPackPortConnections: true"), style);
    EXPECT_TRUE(style.BinPackPortConnections);
}

TEST(ParseConfig, BreakAfterAlways) {
    auto style = getDefaultStyle();

    parseConfig(YAML::Load("BreakAfterAlways: Always"), style);
    EXPECT_EQ(style.BreakAfterAlways, BlockBreakStyle::Always);

    parseConfig(YAML::Load("BreakAfterAlways: Never"), style);
    EXPECT_EQ(style.BreakAfterAlways, BlockBreakStyle::Never);

    parseConfig(YAML::Load("BreakAfterAlways: OnlyMultiline"), style);
    EXPECT_EQ(style.BreakAfterAlways, BlockBreakStyle::OnlyMultiline);
}

TEST(ParseConfig, BreakAfterAssertionLabel) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("BreakAfterAssertionLabel: false"), style);
    EXPECT_FALSE(style.BreakAfterAssertionLabel);
}

TEST(ParseConfig, BreakAfterBegin) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("BreakAfterBegin: false"), style);
    EXPECT_FALSE(style.BreakAfterBegin);
}

TEST(ParseConfig, BreakAfterInitial) {
    auto style = getDefaultStyle();

    parseConfig(YAML::Load("BreakAfterInitial: Always"), style);
    EXPECT_EQ(style.BreakAfterInitial, BlockBreakStyle::Always);

    parseConfig(YAML::Load("BreakAfterInitial: Never"), style);
    EXPECT_EQ(style.BreakAfterInitial, BlockBreakStyle::Never);

    parseConfig(YAML::Load("BreakAfterInitial: OnlyMultiline"), style);
    EXPECT_EQ(style.BreakAfterInitial, BlockBreakStyle::OnlyMultiline);
}

TEST(ParseConfig, BreakBeforeAlways) {
    auto style = getDefaultStyle();

    parseConfig(YAML::Load("BreakBeforeAlways: Always"), style);
    EXPECT_EQ(style.BreakBeforeAlways, BlockBreakStyle::Always);

    parseConfig(YAML::Load("BreakBeforeAlways: Never"), style);
    EXPECT_EQ(style.BreakBeforeAlways, BlockBreakStyle::Never);

    parseConfig(YAML::Load("BreakBeforeAlways: OnlyMultiline"), style);
    EXPECT_EQ(style.BreakBeforeAlways, BlockBreakStyle::OnlyMultiline);
}

TEST(ParseConfig, BreakBeforeEnd) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("BreakBeforeEnd: false"), style);
    EXPECT_FALSE(style.BreakBeforeEnd);
}

TEST(ParseConfig, BreakBeforeFunction) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("BreakBeforeFunction: true"), style);
    EXPECT_TRUE(style.BreakBeforeFunction);
}

TEST(ParseConfig, BreakBeforeInitial) {
    auto style = getDefaultStyle();

    parseConfig(YAML::Load("BreakBeforeInitial: Always"), style);
    EXPECT_EQ(style.BreakBeforeInitial, BlockBreakStyle::Always);

    parseConfig(YAML::Load("BreakBeforeInitial: Never"), style);
    EXPECT_EQ(style.BreakBeforeInitial, BlockBreakStyle::Never);

    parseConfig(YAML::Load("BreakBeforeInitial: OnlyMultiline"), style);
    EXPECT_EQ(style.BreakBeforeInitial, BlockBreakStyle::OnlyMultiline);
}

TEST(ParseConfig, BreakBeforeSpecifyBlock) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("BreakBeforeSpecifyBlock: true"), style);
    EXPECT_TRUE(style.BreakBeforeSpecifyBlock);
}

TEST(ParseConfig, BreakBeforeTask) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("BreakBeforeTask: true"), style);
    EXPECT_TRUE(style.BreakBeforeTask);
}

TEST(ParseConfig, ColumnLimit) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("ColumnLimit: 80"), style);
    EXPECT_EQ(style.ColumnLimit, 80U);
}

TEST(ParseConfig, CompactControlStatements) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("CompactControlStatements: false"), style);
    EXPECT_FALSE(style.CompactControlStatements);
}

TEST(ParseConfig, CompactTimeUnits) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("CompactTimeUnits: false"), style);
    EXPECT_FALSE(style.CompactTimeUnits);
}

TEST(ParseConfig, ContinuationIndentWidth) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("ContinuationIndentWidth: 6"), style);
    EXPECT_EQ(style.ContinuationIndentWidth, 6U);
}

TEST(ParseConfig, ContinuationIndentWidthDefaultsToIndentWidth) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("IndentWidth: 6"), style);
    EXPECT_EQ(style.IndentWidth, 6U);
    EXPECT_EQ(style.ContinuationIndentWidth, 6U);
    EXPECT_EQ(style.ParameterPortListIndentWidth, 6U);
}

TEST(ParseConfig, EventSeparator) {
    auto style = getDefaultStyle();

    parseConfig(YAML::Load("EventSeparator: Comma"), style);
    EXPECT_EQ(style.EventSeparator, EventSeparatorStyle::Comma);

    parseConfig(YAML::Load("EventSeparator: Or"), style);
    EXPECT_EQ(style.EventSeparator, EventSeparatorStyle::Or);

    parseConfig(YAML::Load("EventSeparator: Preserve"), style);
    EXPECT_EQ(style.EventSeparator, EventSeparatorStyle::Preserve);
}

TEST(ParseConfig, IndentCaseItem) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("IndentCaseItem: true"), style);
    EXPECT_TRUE(style.IndentCaseItem);
}

TEST(ParseConfig, IndentWidth) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("IndentWidth: 4"), style);
    EXPECT_EQ(style.IndentWidth, 4U);
}

TEST(ParseConfig, InsertBeginEnd) {
    auto style = getDefaultStyle();

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        InsertBeginEnd:
          Enabled: true
          AlwaysStatements: true
          ControlStatements: true
          InitialStatements: true
    )"));
    // clang-format on

    parseConfig(node, style);
    EXPECT_TRUE(style.InsertBeginEnd.Enabled);
    EXPECT_TRUE(style.InsertBeginEnd.AlwaysStatements);
    EXPECT_TRUE(style.InsertBeginEnd.ControlStatements);
    EXPECT_TRUE(style.InsertBeginEnd.InitialStatements);
}

TEST(ParseConfig, InsertParens) {
    auto style = getDefaultStyle();

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        InsertParens:
          Delays: true
          ExpressionEvents: true
          ImplicitEvents: true
          NamedEvents: true
    )"));
    // clang-format on

    parseConfig(node, style);
    EXPECT_TRUE(style.InsertParens.Delays);
    EXPECT_TRUE(style.InsertParens.ExpressionEvents);
    EXPECT_TRUE(style.InsertParens.ImplicitEvents);
    EXPECT_TRUE(style.InsertParens.NamedEvents);
}

TEST(ParseConfig, LineEnding) {
    auto style = getDefaultStyle();

    parseConfig(YAML::Load("LineEnding: CRLF"), style);
    EXPECT_EQ(style.LineEnding, LineEndingStyle::CRLF);

    parseConfig(YAML::Load("LineEnding: DeriveCRLF"), style);
    EXPECT_EQ(style.LineEnding, LineEndingStyle::DeriveCRLF);

    parseConfig(YAML::Load("LineEnding: DeriveLF"), style);
    EXPECT_EQ(style.LineEnding, LineEndingStyle::DeriveLF);

    parseConfig(YAML::Load("LineEnding: LF"), style);
    EXPECT_EQ(style.LineEnding, LineEndingStyle::LF);
}

TEST(ParseConfig, MaxEmptyLinesToKeep) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("MaxEmptyLinesToKeep: 2"), style);
    EXPECT_EQ(style.MaxEmptyLinesToKeep, 2U);
}

TEST(ParseConfig, OneLineFormatOffRegex) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("OneLineFormatOffRegex: '.*foo.*'"), style);
    EXPECT_EQ(style.OneLineFormatOffRegex, ".*foo.*");
}

TEST(ParseConfig, OneStatementPerLine) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("OneStatementPerLine: false"), style);
    EXPECT_FALSE(style.OneStatementPerLine);
}

TEST(ParseConfig, PackedDimensionBounds) {
    auto style = getDefaultStyle();

    parseConfig(YAML::Load("PackedDimensionBounds: LSBFirst"), style);
    EXPECT_EQ(style.PackedDimensionBounds, DimensionBoundsStyle::LSBFirst);

    parseConfig(YAML::Load("PackedDimensionBounds: MSBFirst"), style);
    EXPECT_EQ(style.PackedDimensionBounds, DimensionBoundsStyle::MSBFirst);

    parseConfig(YAML::Load("PackedDimensionBounds: Preserve"), style);
    EXPECT_EQ(style.PackedDimensionBounds, DimensionBoundsStyle::Preserve);
}

TEST(ParseConfig, ParameterPortListIndentWidth) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("ParameterPortListIndentWidth: 2"), style);
    EXPECT_EQ(style.ParameterPortListIndentWidth, 2U);
}

TEST(ParseConfig, ParameterPortListIndentWidthDefaultsToContinuationIndentWidth) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("ContinuationIndentWidth: 4"), style);
    EXPECT_EQ(style.ContinuationIndentWidth, 4U);
    EXPECT_EQ(style.ParameterPortListIndentWidth, 4U);
}

TEST(ParseConfig, RangeSpacing) {
    auto style = getDefaultStyle();

    parseConfig(YAML::Load("RangeSpacing: Always"), style);
    EXPECT_EQ(style.RangeSpacing, RangeSpacingStyle::Always);

    parseConfig(YAML::Load("RangeSpacing: MatchSource"), style);
    EXPECT_EQ(style.RangeSpacing, RangeSpacingStyle::MatchSource);

    parseConfig(YAML::Load("RangeSpacing: Never"), style);
    EXPECT_EQ(style.RangeSpacing, RangeSpacingStyle::Never);

    parseConfig(YAML::Load("RangeSpacing: Preserve"), style);
    EXPECT_EQ(style.RangeSpacing, RangeSpacingStyle::Preserve);
}

TEST(ParseConfig, RejectsScalar) {
    auto style = getDefaultStyle();
    EXPECT_THROW(parseConfig(YAML::Load("42"), style), std::runtime_error);
}

TEST(ParseConfig, RejectsSequence) {
    auto style = getDefaultStyle();
    EXPECT_THROW(parseConfig(YAML::Load("[1, 2, 3]"), style), std::runtime_error);
}

TEST(ParseConfig, RejectsUnknownBasedOnStyle) {
    auto style = getDefaultStyle();
    EXPECT_THROW(parseConfig(YAML::Load("BasedOnStyle: Unknown"), style), std::runtime_error);
}

TEST(ParseConfig, RemoveEmptyLines) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("RemoveEmptyLines: true"), style);
    EXPECT_TRUE(style.RemoveEmptyLines);
}

TEST(ParseConfig, SpaceAfterAlways) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("SpaceAfterAlways: false"), style);
    EXPECT_FALSE(style.SpaceAfterAlways);
}

TEST(ParseConfig, SpaceAfterBrackets) {
    auto style = getDefaultStyle();

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        SpaceAfterBrackets:
          PackedDimensions: false
          UnpackedDimensions: true
    )"));
    // clang-format on

    parseConfig(node, style);
    EXPECT_FALSE(style.SpaceAfterBrackets.PackedDimensions);
    EXPECT_TRUE(style.SpaceAfterBrackets.UnpackedDimensions);
}

TEST(ParseConfig, SpaceAfterCaseColon) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("SpaceAfterCaseColon: false"), style);
    EXPECT_FALSE(style.SpaceAfterCaseColon);
}

TEST(ParseConfig, SpaceAfterComma) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("SpaceAfterComma: false"), style);
    EXPECT_FALSE(style.SpaceAfterComma);
}

TEST(ParseConfig, SpaceAfterParens) {
    auto style = getDefaultStyle();

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        SpaceAfterParens:
          ControlStatements: false
          EventControls: false
    )"));
    // clang-format on

    parseConfig(node, style);
    EXPECT_FALSE(style.SpaceAfterParens.ControlStatements);
    EXPECT_FALSE(style.SpaceAfterParens.EventControls);
}

TEST(ParseConfig, SpaceAfterSemicolon) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("SpaceAfterSemicolon: false"), style);
    EXPECT_FALSE(style.SpaceAfterSemicolon);
}

TEST(ParseConfig, SpaceAroundOperators) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("SpaceAroundOperators: false"), style);
    EXPECT_FALSE(style.SpaceAroundOperators);
}

TEST(ParseConfig, SpaceBeforeBrackets) {
    auto style = getDefaultStyle();

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        SpaceBeforeBrackets:
          PackedDimensions: false
          UnpackedDimensions: true
    )"));
    // clang-format on

    parseConfig(node, style);
    EXPECT_FALSE(style.SpaceBeforeBrackets.PackedDimensions);
    EXPECT_TRUE(style.SpaceBeforeBrackets.UnpackedDimensions);
}

TEST(ParseConfig, SpaceBeforeCaseColon) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("SpaceBeforeCaseColon: true"), style);
    EXPECT_TRUE(style.SpaceBeforeCaseColon);
}

TEST(ParseConfig, SpaceBeforeParens) {
    auto style = getDefaultStyle();

    // clang-format off
    const auto node = YAML::Load(dedent(R"(
        SpaceBeforeParens:
          ControlStatements: false
          ParameterList: false
          PortList: false
    )"));
    // clang-format on

    parseConfig(node, style);
    EXPECT_FALSE(style.SpaceBeforeParens.ControlStatements);
    EXPECT_FALSE(style.SpaceBeforeParens.ParameterList);
    EXPECT_FALSE(style.SpaceBeforeParens.PortList);
}

TEST(ParseConfig, SpacesBeforeTrailingComments) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("SpacesBeforeTrailingComments: 4"), style);
    EXPECT_EQ(style.SpacesBeforeTrailingComments, 4U);
}

TEST(ParseConfig, SpacesInBraces) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("SpacesInBraces: true"), style);
    EXPECT_TRUE(style.SpacesInBraces);
}

TEST(ParseConfig, SpacesInBrackets) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("SpacesInBrackets: true"), style);
    EXPECT_TRUE(style.SpacesInBrackets);
}

TEST(ParseConfig, SpacesInParens) {
    auto style = getDefaultStyle();
    parseConfig(YAML::Load("SpacesInParens: true"), style);
    EXPECT_TRUE(style.SpacesInParens);
}

TEST(ParseConfig, UnpackedDimensionBounds) {
    auto style = getDefaultStyle();

    parseConfig(YAML::Load("UnpackedDimensionBounds: LSBFirst"), style);
    EXPECT_EQ(style.UnpackedDimensionBounds, DimensionBoundsStyle::LSBFirst);

    parseConfig(YAML::Load("UnpackedDimensionBounds: MSBFirst"), style);
    EXPECT_EQ(style.UnpackedDimensionBounds, DimensionBoundsStyle::MSBFirst);

    parseConfig(YAML::Load("UnpackedDimensionBounds: Preserve"), style);
    EXPECT_EQ(style.UnpackedDimensionBounds, DimensionBoundsStyle::Preserve);
}
