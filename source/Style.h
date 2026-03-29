// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include <filesystem>
#include <functional>
#include <optional>
#include <string>

#include <yaml-cpp/yaml.h>

namespace slang::format {

enum class BreakAfterBlockStyle { Never, Always, OnlyMultiline };

/// Controls automatic insertion of begin/end around statement bodies.
struct InsertBeginEndStyle {
    /// If false, disables all begin/end insertion regardless of other options.
    bool Enabled = false;

    /// If true, wrap bare single-statement bodies of control statements.
    bool ControlStatements = true;

    /// If true, wrap bare statement bodies of always statements.
    bool AlwaysStatements = true;

    /// If true, wrap bare statement bodies of initial statements.
    bool InitialStatements = true;

    bool operator==(const InsertBeginEndStyle&) const = default;
};

/// Defines all formatting style options.
struct Style {
    /// Maximum number of consecutive empty lines to keep.
    unsigned MaxEmptyLinesToKeep = 1;

    /// Number of spaces for each indentation level.
    unsigned IndentWidth = 2;

    /// Number of spaces added for continuation lines relative to the enclosing statement.
    unsigned ContinuationIndentWidth = 2;

    /// Number of spaces for parameter port list items in module declarations.
    unsigned ParameterPortListIndentWidth = 2;

    /// If true, indent statements following a case label when they appear on the next line.
    bool IndentCaseItem = true;

    /// Disables formatting for any line matching this pattern.
    std::string OneLineFormatOffRegex;

    /// Controls insertion of a newline between always and its body.
    BreakAfterBlockStyle BreakAfterAlways = BreakAfterBlockStyle::OnlyMultiline;

    /// Controls insertion of a newline between initial and its body.
    BreakAfterBlockStyle BreakAfterInitial = BreakAfterBlockStyle::OnlyMultiline;

    /// If true, enforce a newline after every begin statement.
    bool BreakAfterBegin = true;

    /// If true, enforce a newline before every end statement.
    bool BreakBeforeEnd = true;

    /// Controls insertion of begin/end around bare statements.
    InsertBeginEndStyle InsertBeginEnd;

    bool operator==(const Style&) const = default;
};

/// Returns the default style.
Style getDefaultStyle();

/// Parses a YAML node into \p style. Throws std::runtime_error on error.
void parseConfiguration(const YAML::Node& node, Style& style);

/// Returns file content, or std::nullopt if the file does not exist.
using FileLoader = std::function<std::optional<std::string>(const std::filesystem::path&)>;

/// Searches for a configuration file starting from \p searchDir, walking up the directory hierarchy
/// to the root. Returns the default style if no configuration file is found.
Style getStyle(const std::filesystem::path& searchDir, FileLoader loader = {});

} // namespace slang::format
