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

TEST(GetStyle, LoadsFromCurrentDir) {
    auto loader = [](const std::filesystem::path& p) -> std::optional<std::string> {
        if (p == std::filesystem::path("/a/b/c/.slang-format")) {
            return "{}";
        }

        return std::nullopt;
    };

    EXPECT_EQ(getStyle("/a/b/c", loader), Style{});
}

TEST(GetStyle, ReturnsDefaultWhenNoFile) {
    auto loader = [](const std::filesystem::path&) -> std::optional<std::string> {
        return std::nullopt;
    };

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
    auto loader = [](const std::filesystem::path& p) -> std::optional<std::string> {
        if (p == std::filesystem::path("/a/.slang-format")) {
            return "{}";
        }

        return std::nullopt;
    };

    EXPECT_EQ(getStyle("/a/b/c", loader), Style{});
}

TEST(ParseConfiguration, AcceptsEmptyMap) {
    YAML::Node const node = YAML::Load("{}");
    Style style;
    parseConfiguration(node, style);
}

TEST(ParseConfiguration, ParsesAlignConsecutiveAssignmentsAcrossComments) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignConsecutiveAssignments:
          AcrossComments: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignConsecutiveAssignments.AcrossComments);
}

TEST(ParseConfiguration, ParsesAlignConsecutiveAssignmentsAcrossEmptyLines) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignConsecutiveAssignments:
          AcrossEmptyLines: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignConsecutiveAssignments.AcrossEmptyLines);
}

TEST(ParseConfiguration, ParsesAlignConsecutiveAssignmentsAcrossParameterPortList) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignConsecutiveAssignments:
          AcrossParameterPortList: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignConsecutiveAssignments.AcrossParameterPortList);
}

TEST(ParseConfiguration, ParsesAlignConsecutiveAssignmentsEnabled) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignConsecutiveAssignments:
          Enabled: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignConsecutiveAssignments.Enabled);
}

TEST(ParseConfiguration, ParsesAlignConsecutiveDeclarationsAcrossComments) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignConsecutiveDeclarations:
          AcrossComments: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignConsecutiveDeclarations.AcrossComments);
}

TEST(ParseConfiguration, ParsesAlignConsecutiveDeclarationsAcrossEmptyLines) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignConsecutiveDeclarations:
          AcrossEmptyLines: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignConsecutiveDeclarations.AcrossEmptyLines);
}

TEST(ParseConfiguration, ParsesAlignConsecutiveDeclarationsAcrossParameterPortList) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignConsecutiveDeclarations:
          AcrossParameterPortList: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignConsecutiveDeclarations.AcrossParameterPortList);
}

TEST(ParseConfiguration, ParsesAlignConsecutiveDeclarationsEnabled) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignConsecutiveDeclarations:
          Enabled: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignConsecutiveDeclarations.Enabled);
}

TEST(ParseConfiguration, ParsesAlignConsecutivePackedDimensionsAcrossComments) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignConsecutivePackedDimensions:
          AcrossComments: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignConsecutivePackedDimensions.AcrossComments);
}

TEST(ParseConfiguration, ParsesAlignConsecutivePackedDimensionsAcrossEmptyLines) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignConsecutivePackedDimensions:
          AcrossEmptyLines: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignConsecutivePackedDimensions.AcrossEmptyLines);
}

TEST(ParseConfiguration, ParsesAlignConsecutivePackedDimensionsAcrossParameterPortList) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignConsecutivePackedDimensions:
          AcrossParameterPortList: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignConsecutivePackedDimensions.AcrossParameterPortList);
}

TEST(ParseConfiguration, ParsesAlignConsecutivePackedDimensionsEnabled) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignConsecutivePackedDimensions:
          Enabled: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignConsecutivePackedDimensions.Enabled);
}

TEST(ParseConfiguration, ParsesAlignConsecutiveTimingControlsAcrossComments) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignConsecutiveTimingControls:
          AcrossComments: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignConsecutiveTimingControls.AcrossComments);
}

TEST(ParseConfiguration, ParsesAlignConsecutiveTimingControlsAcrossEmptyLines) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignConsecutiveTimingControls:
          AcrossEmptyLines: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignConsecutiveTimingControls.AcrossEmptyLines);
}

TEST(ParseConfiguration, ParsesAlignConsecutiveTimingControlsEnabled) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignConsecutiveTimingControls:
          Enabled: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignConsecutiveTimingControls.Enabled);
}

TEST(ParseConfiguration, ParsesAlignTrailingCommentsAcrossComments) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignTrailingComments:
          AcrossComments: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignTrailingComments.AcrossComments);
}

TEST(ParseConfiguration, ParsesAlignTrailingCommentsAcrossEmptyLines) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignTrailingComments:
          AcrossEmptyLines: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignTrailingComments.AcrossEmptyLines);
}

TEST(ParseConfiguration, ParsesAlignTrailingCommentsAcrossParameterPortList) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignTrailingComments:
          AcrossParameterPortList: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignTrailingComments.AcrossParameterPortList);
}

TEST(ParseConfiguration, ParsesAlignTrailingCommentsEnabled) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignTrailingComments:
          Enabled: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignTrailingComments.Enabled);
}

TEST(ParseConfiguration, ParsesAlignConsecutiveStyleAlignColon) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignConsecutivePackedDimensions:
          AlignColon: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignConsecutivePackedDimensions.AlignColon);
}

TEST(ParseConfiguration, ParsesAlignConsecutiveStylePadLeft) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignConsecutivePackedDimensions:
          PadLeft: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignConsecutivePackedDimensions.PadLeft);
}

TEST(ParseConfiguration, ParsesAlignConsecutiveStylePadRight) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        AlignConsecutivePackedDimensions:
          PadRight: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.AlignConsecutivePackedDimensions.PadRight);
}

TEST(ParseConfiguration, ContinuationIndentWidthDefaultsToIndentWidth) {
    YAML::Node const node = YAML::Load("IndentWidth: 6");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.IndentWidth, 6U);
    EXPECT_EQ(style.ContinuationIndentWidth, 6U);
}

TEST(ParseConfiguration, ParameterPortListIndentWidthDefaultsChain) {
    YAML::Node const node = YAML::Load("IndentWidth: 6");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.ContinuationIndentWidth, 6U);
    EXPECT_EQ(style.ParameterPortListIndentWidth, 6U);
}

TEST(ParseConfiguration, ParameterPortListIndentWidthDefaultsToContinuationIndentWidth) {
    YAML::Node const node = YAML::Load("ContinuationIndentWidth: 4");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.ContinuationIndentWidth, 4U);
    EXPECT_EQ(style.ParameterPortListIndentWidth, 4U);
}

TEST(ParseConfiguration, ParsesBreakAfterAlways) {
    YAML::Node const node = YAML::Load("BreakAfterAlways: Always");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.BreakAfterAlways, BlockBreakStyle::Always);
}

TEST(ParseConfiguration, ParsesBreakAfterBegin) {
    YAML::Node const node = YAML::Load("BreakAfterBegin: false");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.BreakAfterBegin, false);
}

TEST(ParseConfiguration, ParsesBreakAfterInitial) {
    YAML::Node const node = YAML::Load("BreakAfterInitial: Always");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.BreakAfterInitial, BlockBreakStyle::Always);
}

TEST(ParseConfiguration, ParsesBreakBeforeAlways) {
    YAML::Node const node = YAML::Load("BreakBeforeAlways: Always");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.BreakBeforeAlways, BlockBreakStyle::Always);
}

TEST(ParseConfiguration, ParsesBreakBeforeInitial) {
    YAML::Node const node = YAML::Load("BreakBeforeInitial: Always");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.BreakBeforeInitial, BlockBreakStyle::Always);
}

TEST(ParseConfiguration, ParsesBreakBeforeEnd) {
    YAML::Node const node = YAML::Load("BreakBeforeEnd: false");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.BreakBeforeEnd, false);
}

TEST(ParseConfiguration, ParsesBreakBeforeFunction) {
    YAML::Node const node = YAML::Load("BreakBeforeFunction: false");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.BreakBeforeFunction, false);
}

TEST(ParseConfiguration, ParsesBreakBeforeSpecifyBlock) {
    YAML::Node const node = YAML::Load("BreakBeforeSpecifyBlock: false");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.BreakBeforeSpecifyBlock, false);
}

TEST(ParseConfiguration, ParsesBreakBeforeTask) {
    YAML::Node const node = YAML::Load("BreakBeforeTask: false");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.BreakBeforeTask, false);
}

TEST(ParseConfiguration, ParsesContinuationIndentWidth) {
    YAML::Node const node = YAML::Load("ContinuationIndentWidth: 4");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.ContinuationIndentWidth, 4U);
}

TEST(ParseConfiguration, ParsesEventSeparatorComma) {
    YAML::Node const node = YAML::Load("EventSeparator: Comma");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.EventSeparator, EventSeparatorStyle::Comma);
}

TEST(ParseConfiguration, ParsesEventSeparatorOr) {
    YAML::Node const node = YAML::Load("EventSeparator: Or");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.EventSeparator, EventSeparatorStyle::Or);
}

TEST(ParseConfiguration, ParsesEventSeparatorPreserve) {
    YAML::Node const node = YAML::Load("EventSeparator: Preserve");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.EventSeparator, EventSeparatorStyle::Preserve);
}

TEST(ParseConfiguration, ParsesIndentCaseItem) {
    YAML::Node const node = YAML::Load("IndentCaseItem: false");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.IndentCaseItem, false);
}

TEST(ParseConfiguration, ParsesIndentWidth) {
    YAML::Node const node = YAML::Load("IndentWidth: 4");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.IndentWidth, 4U);
}

TEST(ParseConfiguration, ParsesInsertBeginEndAlwaysStatements) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        InsertBeginEnd:
          AlwaysStatements: false
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_FALSE(style.InsertBeginEnd.AlwaysStatements);
}

TEST(ParseConfiguration, ParsesInsertBeginEndControlStatements) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        InsertBeginEnd:
          ControlStatements: false
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_FALSE(style.InsertBeginEnd.ControlStatements);
}

TEST(ParseConfiguration, ParsesInsertBeginEndEnabled) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        InsertBeginEnd:
          Enabled: false
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_FALSE(style.InsertBeginEnd.Enabled);
}

TEST(ParseConfiguration, ParsesInsertBeginEndInitialStatements) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        InsertBeginEnd:
          InitialStatements: false
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_FALSE(style.InsertBeginEnd.InitialStatements);
}

TEST(ParseConfiguration, ParsesInsertParensDelays) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        InsertParens:
          Delays: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.InsertParens.Delays);
}

TEST(ParseConfiguration, ParsesInsertParensExpressionEvents) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        InsertParens:
          ExpressionEvents: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.InsertParens.ExpressionEvents);
}

TEST(ParseConfiguration, ParsesInsertParensImplicitEvents) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        InsertParens:
          ImplicitEvents: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.InsertParens.ImplicitEvents);
}

TEST(ParseConfiguration, ParsesInsertParensNamedEvents) {
    Style style;

    // clang-format off
    YAML::Node const node = YAML::Load(dedent(R"(
        InsertParens:
          NamedEvents: true
    )"));
    // clang-format on

    ASSERT_NO_THROW(parseConfiguration(node, style));
    EXPECT_TRUE(style.InsertParens.NamedEvents);
}

TEST(ParseConfiguration, ParsesMaxEmptyLinesToKeep) {
    YAML::Node const node = YAML::Load("MaxEmptyLinesToKeep: 2");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.MaxEmptyLinesToKeep, 2U);
}

TEST(ParseConfiguration, ParsesOneLineFormatOffRegex) {
    YAML::Node const node = YAML::Load("OneLineFormatOffRegex: '.*foo.*'");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.OneLineFormatOffRegex, ".*foo.*");
}

TEST(ParseConfiguration, ParsesPackedDimensionBoundsLSBFirst) {
    YAML::Node const node = YAML::Load("PackedDimensionBounds: LSBFirst");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.PackedDimensionBounds, DimensionBoundsStyle::LSBFirst);
}

TEST(ParseConfiguration, ParsesPackedDimensionBoundsMSBFirst) {
    YAML::Node const node = YAML::Load("PackedDimensionBounds: MSBFirst");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.PackedDimensionBounds, DimensionBoundsStyle::MSBFirst);
}

TEST(ParseConfiguration, ParsesPackedDimensionBoundsPreserve) {
    YAML::Node const node = YAML::Load("PackedDimensionBounds: Preserve");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.PackedDimensionBounds, DimensionBoundsStyle::Preserve);
}

TEST(ParseConfiguration, ParsesParameterPortListIndentWidth) {
    YAML::Node const node = YAML::Load("ParameterPortListIndentWidth: 4");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.ParameterPortListIndentWidth, 4U);
}

TEST(ParseConfiguration, ParsesUnpackedDimensionBoundsLSBFirst) {
    YAML::Node const node = YAML::Load("UnpackedDimensionBounds: LSBFirst");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.UnpackedDimensionBounds, DimensionBoundsStyle::LSBFirst);
}

TEST(ParseConfiguration, ParsesUnpackedDimensionBoundsMSBFirst) {
    YAML::Node const node = YAML::Load("UnpackedDimensionBounds: MSBFirst");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.UnpackedDimensionBounds, DimensionBoundsStyle::MSBFirst);
}

TEST(ParseConfiguration, ParsesUnpackedDimensionBoundsPreserve) {
    YAML::Node const node = YAML::Load("UnpackedDimensionBounds: Preserve");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.UnpackedDimensionBounds, DimensionBoundsStyle::Preserve);
}

TEST(ParseConfiguration, RejectsScalar) {
    YAML::Node const node = YAML::Load("42");
    Style style;
    EXPECT_THROW(parseConfiguration(node, style), std::runtime_error);
}

TEST(ParseConfiguration, RejectsSequence) {
    YAML::Node const node = YAML::Load("[1, 2, 3]");
    Style style;
    EXPECT_THROW(parseConfiguration(node, style), std::runtime_error);
}

TEST(DumpConfiguration, DefaultStyle) {
    auto const result = dumpConfiguration(getDefaultStyle());
    EXPECT_NE(result.find("---"), std::string::npos);
    EXPECT_NE(result.find("..."), std::string::npos);
    EXPECT_NE(result.find("AlignConsecutiveAssignments:"), std::string::npos);
    EXPECT_NE(result.find("AlignTrailingComments:"), std::string::npos);
    EXPECT_NE(result.find("AlignConsecutiveDeclarations:"), std::string::npos);
    EXPECT_NE(result.find("Enabled: false"), std::string::npos);
    EXPECT_NE(result.find("BreakAfterAlways: OnlyMultiline"), std::string::npos);
    EXPECT_NE(result.find("BreakAfterBegin: true"), std::string::npos);
    EXPECT_NE(result.find("BreakAfterInitial: OnlyMultiline"), std::string::npos);
    EXPECT_NE(result.find("BreakBeforeAlways: OnlyMultiline"), std::string::npos);
    EXPECT_NE(result.find("BreakBeforeInitial: OnlyMultiline"), std::string::npos);
    EXPECT_NE(result.find("BreakBeforeEnd: true"), std::string::npos);
    EXPECT_NE(result.find("BreakBeforeFunction: true"), std::string::npos);
    EXPECT_NE(result.find("BreakBeforeSpecifyBlock: true"), std::string::npos);
    EXPECT_NE(result.find("BreakBeforeTask: true"), std::string::npos);
    EXPECT_NE(result.find("ContinuationIndentWidth: 2"), std::string::npos);
    EXPECT_NE(result.find("EventSeparator: Preserve"), std::string::npos);
    EXPECT_NE(result.find("IndentCaseItem: true"), std::string::npos);
    EXPECT_NE(result.find("IndentWidth: 2"), std::string::npos);
    EXPECT_NE(result.find("MaxEmptyLinesToKeep: 1"), std::string::npos);
    EXPECT_NE(result.find("OneLineFormatOffRegex: \"\""), std::string::npos);
    EXPECT_NE(result.find("PackedDimensionBounds: Preserve"), std::string::npos);
    EXPECT_NE(result.find("ParameterPortListIndentWidth: 2"), std::string::npos);
    EXPECT_NE(result.find("UnpackedDimensionBounds: Preserve"), std::string::npos);
    EXPECT_NE(result.find("Enabled: false"), std::string::npos);
    EXPECT_NE(result.find("AlignColon: false"), std::string::npos);
    EXPECT_NE(result.find("PadLeft: false"), std::string::npos);
    EXPECT_NE(result.find("PadRight: false"), std::string::npos);
    EXPECT_NE(result.find("ControlStatements: true"), std::string::npos);
    EXPECT_NE(result.find("AlwaysStatements: true"), std::string::npos);
    EXPECT_NE(result.find("InitialStatements: true"), std::string::npos);
    EXPECT_NE(result.find("InsertParens:"), std::string::npos);
    EXPECT_NE(result.find("Delays: false"), std::string::npos);
    EXPECT_NE(result.find("ExpressionEvents: false"), std::string::npos);
    EXPECT_NE(result.find("ImplicitEvents: false"), std::string::npos);
    EXPECT_NE(result.find("NamedEvents: false"), std::string::npos);
}

TEST(DumpConfiguration, NonDefaultValues) {
    Style style;
    style.IndentWidth = 4;
    style.BreakAfterAlways = BlockBreakStyle::Always;
    style.BreakBeforeAlways = BlockBreakStyle::Always;
    style.BreakBeforeInitial = BlockBreakStyle::Always;
    style.BreakBeforeFunction = false;
    style.BreakBeforeSpecifyBlock = false;
    style.BreakBeforeTask = false;
    style.EventSeparator = EventSeparatorStyle::Comma;
    style.InsertBeginEnd.Enabled = true;
    style.InsertParens.Delays = true;
    style.InsertParens.ExpressionEvents = true;
    style.InsertParens.ImplicitEvents = true;
    style.InsertParens.NamedEvents = true;
    style.PackedDimensionBounds = DimensionBoundsStyle::MSBFirst;
    style.UnpackedDimensionBounds = DimensionBoundsStyle::LSBFirst;

    auto const result = dumpConfiguration(style);
    EXPECT_NE(result.find("IndentWidth: 4"), std::string::npos);
    EXPECT_NE(result.find("BreakAfterAlways: Always"), std::string::npos);
    EXPECT_NE(result.find("BreakBeforeAlways: Always"), std::string::npos);
    EXPECT_NE(result.find("BreakBeforeFunction: false"), std::string::npos);
    EXPECT_NE(result.find("BreakBeforeInitial: Always"), std::string::npos);
    EXPECT_NE(result.find("BreakBeforeSpecifyBlock: false"), std::string::npos);
    EXPECT_NE(result.find("BreakBeforeTask: false"), std::string::npos);
    EXPECT_NE(result.find("EventSeparator: Comma"), std::string::npos);
    EXPECT_NE(result.find("Enabled: true"), std::string::npos);
    EXPECT_NE(result.find("PackedDimensionBounds: MSBFirst"), std::string::npos);
    EXPECT_NE(result.find("UnpackedDimensionBounds: LSBFirst"), std::string::npos);
}

TEST(DumpConfiguration, RoundTrip) {
    Style original;
    original.AlignConsecutiveDeclarations.Enabled = true;
    original.AlignConsecutiveDeclarations.AcrossEmptyLines = true;
    original.AlignConsecutivePackedDimensions.AlignColon = true;
    original.AlignConsecutivePackedDimensions.PadLeft = true;
    original.AlignConsecutivePackedDimensions.PadRight = true;
    original.AlignConsecutiveTimingControls.Enabled = true;
    original.AlignConsecutiveTimingControls.AcrossEmptyLines = true;
    original.AlignTrailingComments.Enabled = true;
    original.AlignTrailingComments.AcrossEmptyLines = true;
    original.IndentWidth = 4;
    original.ContinuationIndentWidth = 4;
    original.ParameterPortListIndentWidth = 4;
    original.MaxEmptyLinesToKeep = 3;
    original.IndentCaseItem = false;
    original.BreakAfterAlways = BlockBreakStyle::Never;
    original.BreakAfterInitial = BlockBreakStyle::Always;
    original.BreakBeforeAlways = BlockBreakStyle::Never;
    original.BreakBeforeInitial = BlockBreakStyle::Never;
    original.BreakAfterBegin = false;
    original.BreakBeforeEnd = false;
    original.BreakBeforeFunction = false;
    original.BreakBeforeSpecifyBlock = false;
    original.BreakBeforeTask = false;
    original.EventSeparator = EventSeparatorStyle::Or;
    original.OneLineFormatOffRegex = ".*test.*";
    original.PackedDimensionBounds = DimensionBoundsStyle::LSBFirst;
    original.UnpackedDimensionBounds = DimensionBoundsStyle::MSBFirst;
    original.InsertBeginEnd.Enabled = true;
    original.InsertBeginEnd.ControlStatements = false;
    original.InsertBeginEnd.AlwaysStatements = false;
    original.InsertBeginEnd.InitialStatements = false;
    original.InsertParens.Delays = true;
    original.InsertParens.ExpressionEvents = true;
    original.InsertParens.ImplicitEvents = true;
    original.InsertParens.NamedEvents = true;

    auto const yaml = dumpConfiguration(original);
    YAML::Node const node = YAML::Load(yaml);
    Style parsed;
    parseConfiguration(node, parsed);
    EXPECT_EQ(parsed, original);
}

TEST(Style, DefaultStyle) {
    Style const style = getDefaultStyle();
    EXPECT_EQ(style, Style{});
}
