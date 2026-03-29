// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

/// Strips a leading newline and the common leading whitespace prefix from all non-empty lines.
/// Allows raw string literals to be indented at call-site level.
inline std::string dedent(std::string_view s) {
    if (!s.empty() && s.front() == '\n') {
        s.remove_prefix(1);
    }

    // Split on newlines, preserving empty segments produced by consecutive or trailing newlines.
    std::vector<std::string_view> lines;
    while (true) {
        auto pos = s.find('\n');
        if (pos == std::string_view::npos) {
            lines.push_back(s);
            break;
        }
        lines.push_back(s.substr(0, pos));
        s.remove_prefix(pos + 1);
    }

    // Find the minimum leading-space count across non-empty lines.
    size_t minIndent{std::string_view::npos};
    for (const auto line : lines) {
        if (line.empty()) {
            continue;
        }

        auto firstNonSpace = line.find_first_not_of(' ');
        if (firstNonSpace == std::string_view::npos) {
            continue; // empty line
        }

        minIndent = std::min(minIndent, firstNonSpace);
    }

    if (minIndent == std::string_view::npos) {
        minIndent = 0;
    }

    // Strip common prefix and rejoin.
    std::string result;
    for (size_t i = 0; i < lines.size(); i++) {
        const auto line = lines.at(i);
        if (!line.empty() && line.find_first_not_of(' ') != std::string_view::npos) {
            result += line.substr(std::min(minIndent, line.size()));
        }

        if (i + 1 < lines.size()) {
            result += '\n';
        }
    }

    return result;
}
