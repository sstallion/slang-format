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

#include <yaml-cpp/emitter.h>
#include <yaml-cpp/emittermanip.h>
#include <yaml-cpp/yaml.h>

using namespace slang::format;

namespace {

constexpr std::array ConfigFileNames{
    ".slang-format",
    "_slang-format",
};

constexpr std::string_view toString(AlignConsecutiveDeclarationsStyle style) {
    switch (style) {
        case AlignConsecutiveDeclarationsStyle::AcrossComments:
            return "AcrossComments";
        case AlignConsecutiveDeclarationsStyle::AcrossEmptyLines:
            return "AcrossEmptyLines";
        case AlignConsecutiveDeclarationsStyle::AcrossEmptyLinesAndComments:
            return "AcrossEmptyLinesAndComments";
        case AlignConsecutiveDeclarationsStyle::AcrossParameterPortList:
            return "AcrossParameterPortList";
        case AlignConsecutiveDeclarationsStyle::Consecutive:
            return "Consecutive";
        case AlignConsecutiveDeclarationsStyle::None:
            return "None";
    }
    return "None";
}

constexpr std::string_view toString(BreakAfterBlockStyle style) {
    switch (style) {
        case BreakAfterBlockStyle::Always:
            return "Always";
        case BreakAfterBlockStyle::Never:
            return "Never";
        case BreakAfterBlockStyle::OnlyMultiline:
            return "OnlyMultiline";
    }
    return "Never";
}

/// Returns the YAML node for \p key within \p node, or null if not present.
template<typename Key>
YAML::Node lookup(const YAML::Node& node, Key key) {
    return node[key];
}

AlignConsecutiveDeclarationsStyle parseAlignConsecutiveDeclarations(std::string_view s) {
    if (s == "AcrossComments") {
        return AlignConsecutiveDeclarationsStyle::AcrossComments;
    }

    if (s == "AcrossEmptyLines") {
        return AlignConsecutiveDeclarationsStyle::AcrossEmptyLines;
    }

    if (s == "AcrossEmptyLinesAndComments") {
        return AlignConsecutiveDeclarationsStyle::AcrossEmptyLinesAndComments;
    }

    if (s == "AcrossParameterPortList") {
        return AlignConsecutiveDeclarationsStyle::AcrossParameterPortList;
    }

    if (s == "Consecutive") {
        return AlignConsecutiveDeclarationsStyle::Consecutive;
    }

    return AlignConsecutiveDeclarationsStyle::None;
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

std::string dumpConfiguration(const Style& style) {
    YAML::Emitter out;
    out << YAML::BeginDoc;
    out << YAML::BeginMap;
    out << YAML::Key << "AlignConsecutiveDeclarations" << YAML::Value
        << std::string{toString(style.AlignConsecutiveDeclarations)};
    out << YAML::Key << "BreakAfterAlways" << YAML::Value
        << std::string{toString(style.BreakAfterAlways)};
    out << YAML::Key << "BreakAfterBegin" << YAML::Value << style.BreakAfterBegin;
    out << YAML::Key << "BreakAfterInitial" << YAML::Value
        << std::string{toString(style.BreakAfterInitial)};
    out << YAML::Key << "BreakBeforeEnd" << YAML::Value << style.BreakBeforeEnd;
    out << YAML::Key << "ContinuationIndentWidth" << YAML::Value << style.ContinuationIndentWidth;
    out << YAML::Key << "IndentCaseItem" << YAML::Value << style.IndentCaseItem;
    out << YAML::Key << "IndentWidth" << YAML::Value << style.IndentWidth;
    out << YAML::Key << "InsertBeginEnd" << YAML::Value;
    out << YAML::BeginMap;
    out << YAML::Key << "AlwaysStatements" << YAML::Value << style.InsertBeginEnd.AlwaysStatements;
    out << YAML::Key << "ControlStatements" << YAML::Value
        << style.InsertBeginEnd.ControlStatements;
    out << YAML::Key << "Enabled" << YAML::Value << style.InsertBeginEnd.Enabled;
    out << YAML::Key << "InitialStatements" << YAML::Value
        << style.InsertBeginEnd.InitialStatements;
    out << YAML::EndMap;
    out << YAML::Key << "MaxEmptyLinesToKeep" << YAML::Value << style.MaxEmptyLinesToKeep;
    out << YAML::Key << "OneLineFormatOffRegex" << YAML::Value << style.OneLineFormatOffRegex;
    out << YAML::Key << "ParameterPortListIndentWidth" << YAML::Value
        << style.ParameterPortListIndentWidth;
    out << YAML::EndMap;
    out << YAML::EndDoc;
    return out.c_str();
}

void parseConfiguration(const YAML::Node& node, Style& style) {
    if (!node.IsMap()) {
        throw std::runtime_error("configuration must be a YAML mapping");
    }

    if (auto v = lookup(node, "AlignConsecutiveDeclarations")) {
        style.AlignConsecutiveDeclarations = parseAlignConsecutiveDeclarations(v.as<std::string>());
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
