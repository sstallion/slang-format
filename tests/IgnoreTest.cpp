// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Ignore.h"
#include "TestHelper.h"

#include <filesystem>
#include <optional>
#include <string>

#include <gtest/gtest.h>

using namespace slang::format;

TEST(IsIgnored, FindsIgnoreInAncestorDir) {
    const auto loader = makeFileLoader({{"/a/.slang-format-ignore", "**/*.sv"}});
    EXPECT_TRUE(isIgnored("/a/b/c/d/foo.sv", loader));
}

TEST(IsIgnored, LastMatchWins) {
    const auto loader = makeFileLoader({{"/a/b/.slang-format-ignore", "*.sv\n!*.sv"}});
    EXPECT_FALSE(isIgnored("/a/b/foo.sv", loader));
}

TEST(IsIgnored, MatchesDoubleStar) {
    const auto loader = makeFileLoader({{"/a/.slang-format-ignore", "**/test/*.sv"}});
    EXPECT_TRUE(isIgnored("/a/b/test/foo.sv", loader));
}

TEST(IsIgnored, MatchesEllipsis) {
    const auto loader = makeFileLoader({{"/a/.slang-format-ignore", ".../test/*.sv"}});
    EXPECT_TRUE(isIgnored("/a/b/test/foo.sv", loader));
}

TEST(IsIgnored, MatchesSimpleGlob) {
    const auto loader = makeFileLoader({{"/a/b/.slang-format-ignore", "*.sv"}});
    EXPECT_TRUE(isIgnored("/a/b/foo.sv", loader));
}

TEST(IsIgnored, NegationReIncludes) {
    const auto loader = makeFileLoader({{"/a/b/.slang-format-ignore", "*.sv\n!important.sv"}});
    EXPECT_TRUE(isIgnored("/a/b/foo.sv", loader));
    EXPECT_FALSE(isIgnored("/a/b/important.sv", loader));
}

TEST(IsIgnored, NoIgnoreFile) {
    const auto loader = makeFileLoader({});
    EXPECT_FALSE(isIgnored("/a/b/c/foo.sv", loader));
}

TEST(IsIgnored, NoMatch) {
    const auto loader = makeFileLoader({{"/a/b/.slang-format-ignore", "*.v"}});
    EXPECT_FALSE(isIgnored("/a/b/foo.sv", loader));
}

TEST(IsIgnored, PatternRelativeToIgnoreFileDir) {
    const auto loader = makeFileLoader({{"/a/.slang-format-ignore", "b/c/*.sv"}});
    EXPECT_TRUE(isIgnored("/a/b/c/foo.sv", loader));
    EXPECT_FALSE(isIgnored("/a/b/foo.sv", loader));
}

TEST(IsIgnored, PrefersFirstNameVariant) {
    int dotCalls{0};
    auto loader = [&dotCalls](const std::filesystem::path& p) -> std::optional<std::string> {
        if (p == std::filesystem::path("/a/b/.slang-format-ignore")) {
            dotCalls++;
            return "*.sv";
        }

        if (p == std::filesystem::path("/a/b/_slang-format-ignore")) {
            return "*.v";
        }

        return std::nullopt;
    };

    EXPECT_TRUE(isIgnored("/a/b/foo.sv", loader));
    EXPECT_EQ(dotCalls, 1);
}

TEST(IsIgnored, SkipsBlankLines) {
    const auto loader = makeFileLoader({{"/a/b/.slang-format-ignore", "\n\n*.sv\n\n"}});
    EXPECT_TRUE(isIgnored("/a/b/foo.sv", loader));
}

TEST(IsIgnored, SkipsCommentLines) {
    const auto loader = makeFileLoader(
        {{"/a/b/.slang-format-ignore", "# This is a comment\n*.sv"}});
    EXPECT_TRUE(isIgnored("/a/b/foo.sv", loader));
}

TEST(IsIgnored, StopsAtFirstIgnoreFile) {
    const auto loader = makeFileLoader({
        {"/a/.slang-format-ignore", "*.sv"},
        {"/a/b/.slang-format-ignore", "*.v"},
    });
    EXPECT_FALSE(isIgnored("/a/b/foo.sv", loader));
    EXPECT_TRUE(isIgnored("/a/b/foo.v", loader));
}

TEST(IsIgnored, TrimsWhitespace) {
    const auto loader = makeFileLoader({{"/a/b/.slang-format-ignore", "  *.sv  "}});
    EXPECT_TRUE(isIgnored("/a/b/foo.sv", loader));
}
