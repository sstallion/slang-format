// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Style.h"

#include <array>
#include <exception>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

#include <yaml-cpp/yaml.h>

using namespace slang::format;

namespace {

constexpr std::array ConfigFileNames{
    ".slang-format",
    "_slang-format",
};

/// Returns the YAML node for \p key within \p node, or null if not present.
template<typename Key>
YAML::Node lookup(const YAML::Node& node, Key key) {
    return node[key];
}

void parseInsertBeginEnd(const YAML::Node& node, InsertBeginEndStyle& config) {
    if (auto v = lookup(node, "Enabled")) {
        config.Enabled = v.as<bool>();
    }

    if (auto v = lookup(node, "AlwaysStatements")) {
        config.AlwaysStatements = v.as<bool>();
    }

    if (auto v = lookup(node, "ControlStatements")) {
        config.ControlStatements = v.as<bool>();
    }

    if (auto v = lookup(node, "InitialStatements")) {
        config.InitialStatements = v.as<bool>();
    }
}

} // namespace

namespace slang::format {

Style getDefaultStyle() {
    return {};
}

void parseConfiguration(const YAML::Node& node, Style& style) {
    if (!node.IsMap()) {
        throw std::runtime_error("configuration must be a YAML mapping");
    }

    if (auto v = lookup(node, "MaxEmptyLinesToKeep")) {
        style.MaxEmptyLinesToKeep = v.as<unsigned>();
    }

    bool hasContinuation{false};
    if (auto v = lookup(node, "IndentWidth")) {
        style.IndentWidth = v.as<unsigned>();
    }

    if (auto v = lookup(node, "ContinuationIndentWidth")) {
        style.ContinuationIndentWidth = v.as<unsigned>();
        hasContinuation = true;
    }

    bool hasParamPort{false};
    if (auto v = lookup(node, "ParameterPortListIndentWidth")) {
        style.ParameterPortListIndentWidth = v.as<unsigned>();
        hasParamPort = true;
    }

    if (!hasContinuation) {
        style.ContinuationIndentWidth = style.IndentWidth;
    }

    if (!hasParamPort) {
        style.ParameterPortListIndentWidth = style.ContinuationIndentWidth;
    }

    if (auto v = lookup(node, "IndentCaseItem")) {
        style.IndentCaseItem = v.as<bool>();
    }

    if (auto v = lookup(node, "OneLineFormatOffRegex")) {
        style.OneLineFormatOffRegex = v.as<std::string>();
    }

    auto parseBreakAfterBlock = [](std::string_view s) {
        if (s == "Always") {
            return BreakAfterBlockStyle::Always;
        }

        if (s == "OnlyMultiline") {
            return BreakAfterBlockStyle::OnlyMultiline;
        }

        return BreakAfterBlockStyle::Never;
    };

    if (auto v = lookup(node, "BreakAfterAlways")) {
        style.BreakAfterAlways = parseBreakAfterBlock(v.as<std::string>());
    }

    if (auto v = lookup(node, "BreakAfterInitial")) {
        style.BreakAfterInitial = parseBreakAfterBlock(v.as<std::string>());
    }

    if (auto v = lookup(node, "BreakAfterBegin")) {
        style.BreakAfterBegin = v.as<bool>();
    }

    if (auto v = lookup(node, "BreakBeforeEnd")) {
        style.BreakBeforeEnd = v.as<bool>();
    }

    if (auto n = lookup(node, "InsertBeginEnd")) {
        parseInsertBeginEnd(n, style.InsertBeginEnd);
    }
}

Style getStyle(const std::filesystem::path& searchDir, FileLoader loader) {
    if (!loader) {
        loader = [](const std::filesystem::path& path) -> std::optional<std::string> {
            try {
                return YAML::Dump(YAML::LoadFile(path.string()));
            }
            catch (const YAML::BadFile&) {
                return std::nullopt;
            }
        };
    }

    for (std::filesystem::path path = searchDir;; path = path.parent_path()) {
        for (std::string_view const name : ConfigFileNames) {
            auto candidate = path / name;
            if (auto content = loader(candidate)) {
                try {
                    YAML::Node const node = YAML::Load(*content);
                    auto style = getDefaultStyle();
                    parseConfiguration(node, style);
                    return style;
                }
                catch (const std::exception& e) {
                    throw std::runtime_error(candidate.string() + ": " + e.what());
                }
            }
        }

        auto parent = path.parent_path();
        if (parent == path) {
            break;
        }
    }

    return getDefaultStyle();
}

} // namespace slang::format
