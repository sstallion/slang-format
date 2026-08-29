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

TEST(ParseConfiguration, ParsesAlignConsecutiveDeclarations) {
    auto parse = [](const char* yaml) {
        YAML::Node const node = YAML::Load(yaml);
        Style style;
        parseConfiguration(node, style);
        return style.AlignConsecutiveDeclarations;
    };

    EXPECT_EQ(parse("AlignConsecutiveDeclarations: AcrossComments"),
              AlignConsecutiveDeclarationsStyle::AcrossComments);
    EXPECT_EQ(parse("AlignConsecutiveDeclarations: AcrossEmptyLines"),
              AlignConsecutiveDeclarationsStyle::AcrossEmptyLines);
    EXPECT_EQ(parse("AlignConsecutiveDeclarations: AcrossEmptyLinesAndComments"),
              AlignConsecutiveDeclarationsStyle::AcrossEmptyLinesAndComments);
    EXPECT_EQ(parse("AlignConsecutiveDeclarations: AcrossParameterPortList"),
              AlignConsecutiveDeclarationsStyle::AcrossParameterPortList);
    EXPECT_EQ(parse("AlignConsecutiveDeclarations: Consecutive"),
              AlignConsecutiveDeclarationsStyle::Consecutive);
    EXPECT_EQ(parse("AlignConsecutiveDeclarations: None"), AlignConsecutiveDeclarationsStyle::None);
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
    EXPECT_EQ(style.BreakAfterAlways, BreakAfterBlockStyle::Always);
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
    EXPECT_EQ(style.BreakAfterInitial, BreakAfterBlockStyle::Always);
}

TEST(ParseConfiguration, ParsesBreakBeforeEnd) {
    YAML::Node const node = YAML::Load("BreakBeforeEnd: false");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.BreakBeforeEnd, false);
}

TEST(ParseConfiguration, ParsesContinuationIndentWidth) {
    YAML::Node const node = YAML::Load("ContinuationIndentWidth: 4");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.ContinuationIndentWidth, 4U);
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

TEST(ParseConfiguration, ParsesParameterPortListIndentWidth) {
    YAML::Node const node = YAML::Load("ParameterPortListIndentWidth: 4");
    Style style;
    parseConfiguration(node, style);
    EXPECT_EQ(style.ParameterPortListIndentWidth, 4U);
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
    EXPECT_NE(result.find("AlignConsecutiveDeclarations: Consecutive"), std::string::npos);
    EXPECT_NE(result.find("BreakAfterAlways: OnlyMultiline"), std::string::npos);
    EXPECT_NE(result.find("BreakAfterBegin: true"), std::string::npos);
    EXPECT_NE(result.find("BreakAfterInitial: OnlyMultiline"), std::string::npos);
    EXPECT_NE(result.find("BreakBeforeEnd: true"), std::string::npos);
    EXPECT_NE(result.find("ContinuationIndentWidth: 2"), std::string::npos);
    EXPECT_NE(result.find("IndentCaseItem: true"), std::string::npos);
    EXPECT_NE(result.find("IndentWidth: 2"), std::string::npos);
    EXPECT_NE(result.find("MaxEmptyLinesToKeep: 1"), std::string::npos);
    EXPECT_NE(result.find("OneLineFormatOffRegex: \"\""), std::string::npos);
    EXPECT_NE(result.find("ParameterPortListIndentWidth: 2"), std::string::npos);
    EXPECT_NE(result.find("Enabled: false"), std::string::npos);
    EXPECT_NE(result.find("ControlStatements: true"), std::string::npos);
    EXPECT_NE(result.find("AlwaysStatements: true"), std::string::npos);
    EXPECT_NE(result.find("InitialStatements: true"), std::string::npos);
}

TEST(DumpConfiguration, NonDefaultValues) {
    Style style;
    style.IndentWidth = 4;
    style.BreakAfterAlways = BreakAfterBlockStyle::Always;
    style.InsertBeginEnd.Enabled = true;

    auto const result = dumpConfiguration(style);
    EXPECT_NE(result.find("IndentWidth: 4"), std::string::npos);
    EXPECT_NE(result.find("BreakAfterAlways: Always"), std::string::npos);
    EXPECT_NE(result.find("Enabled: true"), std::string::npos);
}

TEST(DumpConfiguration, RoundTrip) {
    Style original;
    original.AlignConsecutiveDeclarations = AlignConsecutiveDeclarationsStyle::AcrossEmptyLines;
    original.IndentWidth = 4;
    original.ContinuationIndentWidth = 4;
    original.ParameterPortListIndentWidth = 4;
    original.MaxEmptyLinesToKeep = 3;
    original.IndentCaseItem = false;
    original.BreakAfterAlways = BreakAfterBlockStyle::Never;
    original.BreakAfterInitial = BreakAfterBlockStyle::Always;
    original.BreakAfterBegin = false;
    original.BreakBeforeEnd = false;
    original.OneLineFormatOffRegex = ".*test.*";
    original.InsertBeginEnd.Enabled = true;
    original.InsertBeginEnd.ControlStatements = false;
    original.InsertBeginEnd.AlwaysStatements = false;
    original.InsertBeginEnd.InitialStatements = false;

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
