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

TEST(Style, DefaultStyle) {
    Style const style = getDefaultStyle();
    EXPECT_EQ(style, Style{});
}
