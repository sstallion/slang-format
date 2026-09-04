// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "FileLoader.h"
#include "Ignore.h"

#include <array>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <slang/text/Glob.h>

using namespace slang::format;

namespace {

constexpr std::array<std::string_view, 2> IgnoreFileNames{
    ".slang-format-ignore",
    "_slang-format-ignore",
};

struct IgnoreEntry {
    std::string pattern;
    bool negated;
};

std::string trimWhitespace(std::string_view sv) {
    auto start = sv.find_first_not_of(" \t\r");
    if (start == std::string_view::npos) {
        return {};
    }

    auto end = sv.find_last_not_of(" \t\r");
    return std::string{sv.substr(start, end - start + 1)};
}

std::string translatePattern(std::string_view sv) {
    std::string result;
    result.reserve(sv.size());

    for (std::size_t i = 0; i < sv.size(); i++) {
        if (sv[i] == '*' && i + 1 < sv.size() && sv[i + 1] == '*') {
            result += "...";
            i++;
        }
        else {
            result += sv[i];
        }
    }

    return result;
}

std::vector<IgnoreEntry> parseIgnoreFile(std::string_view content) {
    std::vector<IgnoreEntry> entries;

    while (!content.empty()) {
        auto pos = content.find('\n');
        auto line = content.substr(0, pos);
        content = pos == std::string_view::npos ? std::string_view{} : content.substr(pos + 1);

        auto trimmed = trimWhitespace(line);
        if (trimmed.empty() || trimmed.starts_with('#')) {
            continue;
        }

        auto negated = false;
        if (trimmed.starts_with('!')) {
            negated = true;
            trimmed = trimmed.substr(1);
        }

        entries.push_back({.pattern = translatePattern(trimmed), .negated = negated});
    }

    return entries;
}

} // namespace

namespace slang::format {

bool isIgnored(const std::filesystem::path& filePath, FileLoader loader) {
    if (!loader) {
        loader = [](const std::filesystem::path& path) -> std::optional<std::string> {
            std::ifstream stream{path};
            if (!stream) {
                return std::nullopt;
            }

            std::string content{std::istreambuf_iterator<char>{stream},
                                std::istreambuf_iterator<char>{}};
            return content;
        };
    }

    auto result = findFileInHierarchy(filePath.parent_path(), IgnoreFileNames, loader);
    if (!result) {
        return false;
    }

    auto& [ignorePath, content] = *result;
    auto ignoreDir = ignorePath.parent_path();
    auto entries = parseIgnoreFile(content);

    auto relativePath = std::filesystem::relative(filePath, ignoreDir);
    auto relativeStr = relativePath.generic_string();

    auto ignored = false;
    for (const auto& entry : entries) {
        if (slang::svGlobMatches(std::filesystem::path{relativeStr},
                                 std::filesystem::path{entry.pattern})) {
            ignored = !entry.negated;
        }
    }

    return ignored;
}

} // namespace slang::format
