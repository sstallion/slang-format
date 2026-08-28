// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Ignore.h"

#include <filesystem>
#include <optional>
#include <string>

#include <gtest/gtest.h>

using namespace slang::format;

TEST(IsIgnored, ReturnsFalseWhenNoIgnoreFile) {
    auto loader = [](const std::filesystem::path&) -> std::optional<std::string> {
        return std::nullopt;
    };

    EXPECT_FALSE(isIgnored("/a/b/c/foo.sv", loader));
}

TEST(IsIgnored, MatchesSimpleGlob) {
    auto loader = [](const std::filesystem::path& p) -> std::optional<std::string> {
        if (p == std::filesystem::path("/a/b/.slang-format-ignore")) {
            return "*.sv";
        }

        return std::nullopt;
    };

    EXPECT_TRUE(isIgnored("/a/b/foo.sv", loader));
}

TEST(IsIgnored, MatchesRecursiveGlobWithDoubleStar) {
    auto loader = [](const std::filesystem::path& p) -> std::optional<std::string> {
        if (p == std::filesystem::path("/a/.slang-format-ignore")) {
            return "**/test/*.sv";
        }

        return std::nullopt;
    };

    EXPECT_TRUE(isIgnored("/a/b/test/foo.sv", loader));
}

TEST(IsIgnored, MatchesRecursiveGlobWithEllipsis) {
    auto loader = [](const std::filesystem::path& p) -> std::optional<std::string> {
        if (p == std::filesystem::path("/a/.slang-format-ignore")) {
            return ".../test/*.sv";
        }

        return std::nullopt;
    };

    EXPECT_TRUE(isIgnored("/a/b/test/foo.sv", loader));
}

TEST(IsIgnored, NegationReIncludes) {
    auto loader = [](const std::filesystem::path& p) -> std::optional<std::string> {
        if (p == std::filesystem::path("/a/b/.slang-format-ignore")) {
            return "*.sv\n!important.sv";
        }

        return std::nullopt;
    };

    EXPECT_TRUE(isIgnored("/a/b/foo.sv", loader));
    EXPECT_FALSE(isIgnored("/a/b/important.sv", loader));
}

TEST(IsIgnored, LastMatchWins) {
    auto loader = [](const std::filesystem::path& p) -> std::optional<std::string> {
        if (p == std::filesystem::path("/a/b/.slang-format-ignore")) {
            return "*.sv\n!*.sv";
        }

        return std::nullopt;
    };

    EXPECT_FALSE(isIgnored("/a/b/foo.sv", loader));
}

TEST(IsIgnored, PatternRelativeToIgnoreFileDir) {
    auto loader = [](const std::filesystem::path& p) -> std::optional<std::string> {
        if (p == std::filesystem::path("/a/.slang-format-ignore")) {
            return "b/c/*.sv";
        }

        return std::nullopt;
    };

    EXPECT_TRUE(isIgnored("/a/b/c/foo.sv", loader));
    EXPECT_FALSE(isIgnored("/a/b/foo.sv", loader));
}

TEST(IsIgnored, FindsIgnoreInAncestorDir) {
    auto loader = [](const std::filesystem::path& p) -> std::optional<std::string> {
        if (p == std::filesystem::path("/a/.slang-format-ignore")) {
            return "**/*.sv";
        }

        return std::nullopt;
    };

    EXPECT_TRUE(isIgnored("/a/b/c/d/foo.sv", loader));
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

TEST(IsIgnored, StopsAtFirstIgnoreFile) {
    auto loader = [](const std::filesystem::path& p) -> std::optional<std::string> {
        if (p == std::filesystem::path("/a/b/.slang-format-ignore")) {
            return "*.v";
        }

        if (p == std::filesystem::path("/a/.slang-format-ignore")) {
            return "*.sv";
        }

        return std::nullopt;
    };

    EXPECT_FALSE(isIgnored("/a/b/foo.sv", loader));
    EXPECT_TRUE(isIgnored("/a/b/foo.v", loader));
}

TEST(IsIgnored, SkipsCommentLines) {
    auto loader = [](const std::filesystem::path& p) -> std::optional<std::string> {
        if (p == std::filesystem::path("/a/b/.slang-format-ignore")) {
            return "# This is a comment\n*.sv";
        }

        return std::nullopt;
    };

    EXPECT_TRUE(isIgnored("/a/b/foo.sv", loader));
}

TEST(IsIgnored, SkipsBlankLines) {
    auto loader = [](const std::filesystem::path& p) -> std::optional<std::string> {
        if (p == std::filesystem::path("/a/b/.slang-format-ignore")) {
            return "\n\n*.sv\n\n";
        }

        return std::nullopt;
    };

    EXPECT_TRUE(isIgnored("/a/b/foo.sv", loader));
}

TEST(IsIgnored, TrimsWhitespace) {
    auto loader = [](const std::filesystem::path& p) -> std::optional<std::string> {
        if (p == std::filesystem::path("/a/b/.slang-format-ignore")) {
            return "  *.sv  ";
        }

        return std::nullopt;
    };

    EXPECT_TRUE(isIgnored("/a/b/foo.sv", loader));
}

TEST(IsIgnored, NoMatchReturnsFalse) {
    auto loader = [](const std::filesystem::path& p) -> std::optional<std::string> {
        if (p == std::filesystem::path("/a/b/.slang-format-ignore")) {
            return "*.v";
        }

        return std::nullopt;
    };

    EXPECT_FALSE(isIgnored("/a/b/foo.sv", loader));
}
