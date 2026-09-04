// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "FileLoader.h"
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

constexpr std::array<std::string_view, 2> ConfigFileNames{
    ".slang-format",
    "_slang-format",
};

constexpr std::string_view toString(BlockBreakStyle style) {
    switch (style) {
        case BlockBreakStyle::Always:
            return "Always";
        case BlockBreakStyle::Never:
            return "Never";
        case BlockBreakStyle::OnlyMultiline:
            return "OnlyMultiline";
    }
    return "Never";
}

constexpr std::string_view toString(DimensionBoundsStyle style) {
    switch (style) {
        case DimensionBoundsStyle::LSBFirst:
            return "LSBFirst";
        case DimensionBoundsStyle::MSBFirst:
            return "MSBFirst";
        case DimensionBoundsStyle::Preserve:
            return "Preserve";
    }
    return "Preserve";
}

constexpr std::string_view toString(EventSeparatorStyle style) {
    switch (style) {
        case EventSeparatorStyle::Comma:
            return "Comma";
        case EventSeparatorStyle::Or:
            return "Or";
        case EventSeparatorStyle::Preserve:
            return "Preserve";
    }
    return "Preserve";
}

void parseAlignConsecutive(const YAML::Node& node, AlignConsecutiveStyle& config) {
    if (auto v = node["AcrossComments"]) {
        config.AcrossComments = v.as<bool>();
    }

    if (auto v = node["AcrossEmptyLines"]) {
        config.AcrossEmptyLines = v.as<bool>();
    }

    if (auto v = node["AcrossParameterPortList"]) {
        config.AcrossParameterPortList = v.as<bool>();
    }

    if (auto v = node["AlignColon"]) {
        config.AlignColon = v.as<bool>();
    }

    if (auto v = node["Enabled"]) {
        config.Enabled = v.as<bool>();
    }

    if (auto v = node["PadLeft"]) {
        config.PadLeft = v.as<bool>();
    }

    if (auto v = node["PadRight"]) {
        config.PadRight = v.as<bool>();
    }
}

void parseInsertBeginEnd(const YAML::Node& node, InsertBeginEndStyle& config) {
    if (auto v = node["Enabled"]) {
        config.Enabled = v.as<bool>();
    }

    if (auto v = node["AlwaysStatements"]) {
        config.AlwaysStatements = v.as<bool>();
    }

    if (auto v = node["ControlStatements"]) {
        config.ControlStatements = v.as<bool>();
    }

    if (auto v = node["InitialStatements"]) {
        config.InitialStatements = v.as<bool>();
    }
}

void parseInsertParens(const YAML::Node& node, InsertParensStyle& config) {
    if (auto v = node["Delays"]) {
        config.Delays = v.as<bool>();
    }

    if (auto v = node["ExpressionEvents"]) {
        config.ExpressionEvents = v.as<bool>();
    }

    if (auto v = node["ImplicitEvents"]) {
        config.ImplicitEvents = v.as<bool>();
    }

    if (auto v = node["NamedEvents"]) {
        config.NamedEvents = v.as<bool>();
    }
}

BlockBreakStyle parseBlockBreak(std::string_view s) {
    if (s == "Always") {
        return BlockBreakStyle::Always;
    }

    if (s == "OnlyMultiline") {
        return BlockBreakStyle::OnlyMultiline;
    }

    return BlockBreakStyle::Never;
}

DimensionBoundsStyle parseDimensionBounds(std::string_view s) {
    if (s == "LSBFirst") {
        return DimensionBoundsStyle::LSBFirst;
    }

    if (s == "MSBFirst") {
        return DimensionBoundsStyle::MSBFirst;
    }

    return DimensionBoundsStyle::Preserve;
}

EventSeparatorStyle parseEventSeparator(std::string_view s) {
    if (s == "Comma") {
        return EventSeparatorStyle::Comma;
    }

    if (s == "Or") {
        return EventSeparatorStyle::Or;
    }

    return EventSeparatorStyle::Preserve;
}

void parseBreakStyle(const YAML::Node& node, Style& style) {
    if (auto v = node["BreakAfterAlways"]) {
        style.BreakAfterAlways = parseBlockBreak(v.as<std::string>());
    }

    if (auto v = node["BreakAfterBegin"]) {
        style.BreakAfterBegin = v.as<bool>();
    }

    if (auto v = node["BreakAfterInitial"]) {
        style.BreakAfterInitial = parseBlockBreak(v.as<std::string>());
    }

    if (auto v = node["BreakBeforeAlways"]) {
        style.BreakBeforeAlways = parseBlockBreak(v.as<std::string>());
    }

    if (auto v = node["BreakBeforeEnd"]) {
        style.BreakBeforeEnd = v.as<bool>();
    }

    if (auto v = node["BreakBeforeFunction"]) {
        style.BreakBeforeFunction = v.as<bool>();
    }

    if (auto v = node["BreakBeforeInitial"]) {
        style.BreakBeforeInitial = parseBlockBreak(v.as<std::string>());
    }

    if (auto v = node["BreakBeforeSpecifyBlock"]) {
        style.BreakBeforeSpecifyBlock = v.as<bool>();
    }

    if (auto v = node["BreakBeforeTask"]) {
        style.BreakBeforeTask = v.as<bool>();
    }
}

struct AlignConsecutiveFields {
    bool acrossParameterPortList = false;
    bool alignColon = false;
    bool padLeftRight = false;
};

void emitAlignConsecutive(YAML::Emitter& out, const AlignConsecutiveStyle& config,
                          AlignConsecutiveFields fields) {
    out << YAML::BeginMap;
    out << YAML::Key << "AcrossComments" << YAML::Value << config.AcrossComments;
    out << YAML::Key << "AcrossEmptyLines" << YAML::Value << config.AcrossEmptyLines;
    if (fields.acrossParameterPortList) {
        out << YAML::Key << "AcrossParameterPortList" << YAML::Value
            << config.AcrossParameterPortList;
    }
    if (fields.alignColon) {
        out << YAML::Key << "AlignColon" << YAML::Value << config.AlignColon;
    }
    out << YAML::Key << "Enabled" << YAML::Value << config.Enabled;
    if (fields.padLeftRight) {
        out << YAML::Key << "PadLeft" << YAML::Value << config.PadLeft;
        out << YAML::Key << "PadRight" << YAML::Value << config.PadRight;
    }
    out << YAML::EndMap;
}

void emitInsertBeginEnd(YAML::Emitter& out, const InsertBeginEndStyle& config) {
    out << YAML::BeginMap;
    out << YAML::Key << "AlwaysStatements" << YAML::Value << config.AlwaysStatements;
    out << YAML::Key << "ControlStatements" << YAML::Value << config.ControlStatements;
    out << YAML::Key << "Enabled" << YAML::Value << config.Enabled;
    out << YAML::Key << "InitialStatements" << YAML::Value << config.InitialStatements;
    out << YAML::EndMap;
}

void emitInsertParens(YAML::Emitter& out, const InsertParensStyle& config) {
    out << YAML::BeginMap;
    out << YAML::Key << "Delays" << YAML::Value << config.Delays;
    out << YAML::Key << "ExpressionEvents" << YAML::Value << config.ExpressionEvents;
    out << YAML::Key << "ImplicitEvents" << YAML::Value << config.ImplicitEvents;
    out << YAML::Key << "NamedEvents" << YAML::Value << config.NamedEvents;
    out << YAML::EndMap;
}

} // namespace

namespace slang::format {

Style getDefaultStyle() {
    return {};
}

std::string dumpConfiguration(const Style& style) {
    constexpr AlignConsecutiveFields portListFields{.acrossParameterPortList = true};
    constexpr AlignConsecutiveFields allFields{.acrossParameterPortList = true,
                                               .alignColon = true,
                                               .padLeftRight = true};

    YAML::Emitter out;
    out << YAML::BeginDoc;
    out << YAML::BeginMap;
    out << YAML::Key << "AlignConsecutiveAssignments" << YAML::Value;
    emitAlignConsecutive(out, style.AlignConsecutiveAssignments, portListFields);
    out << YAML::Key << "AlignTrailingComments" << YAML::Value;
    emitAlignConsecutive(out, style.AlignTrailingComments, portListFields);
    out << YAML::Key << "AlignConsecutiveDeclarations" << YAML::Value;
    emitAlignConsecutive(out, style.AlignConsecutiveDeclarations, portListFields);
    out << YAML::Key << "AlignConsecutivePackedDimensions" << YAML::Value;
    emitAlignConsecutive(out, style.AlignConsecutivePackedDimensions, allFields);
    out << YAML::Key << "AlignConsecutiveTimingControls" << YAML::Value;
    emitAlignConsecutive(out, style.AlignConsecutiveTimingControls, {});
    out << YAML::Key << "BreakAfterAlways" << YAML::Value
        << std::string{toString(style.BreakAfterAlways)};
    out << YAML::Key << "BreakAfterBegin" << YAML::Value << style.BreakAfterBegin;
    out << YAML::Key << "BreakAfterInitial" << YAML::Value
        << std::string{toString(style.BreakAfterInitial)};
    out << YAML::Key << "BreakBeforeAlways" << YAML::Value
        << std::string{toString(style.BreakBeforeAlways)};
    out << YAML::Key << "BreakBeforeEnd" << YAML::Value << style.BreakBeforeEnd;
    out << YAML::Key << "BreakBeforeFunction" << YAML::Value << style.BreakBeforeFunction;
    out << YAML::Key << "BreakBeforeInitial" << YAML::Value
        << std::string{toString(style.BreakBeforeInitial)};
    out << YAML::Key << "BreakBeforeSpecifyBlock" << YAML::Value << style.BreakBeforeSpecifyBlock;
    out << YAML::Key << "BreakBeforeTask" << YAML::Value << style.BreakBeforeTask;
    out << YAML::Key << "ContinuationIndentWidth" << YAML::Value << style.ContinuationIndentWidth;
    out << YAML::Key << "EventSeparator" << YAML::Value
        << std::string{toString(style.EventSeparator)};
    out << YAML::Key << "IndentCaseItem" << YAML::Value << style.IndentCaseItem;
    out << YAML::Key << "IndentWidth" << YAML::Value << style.IndentWidth;
    out << YAML::Key << "InsertBeginEnd" << YAML::Value;
    emitInsertBeginEnd(out, style.InsertBeginEnd);
    out << YAML::Key << "InsertParens" << YAML::Value;
    emitInsertParens(out, style.InsertParens);
    out << YAML::Key << "MaxEmptyLinesToKeep" << YAML::Value << style.MaxEmptyLinesToKeep;
    out << YAML::Key << "OneLineFormatOffRegex" << YAML::Value << style.OneLineFormatOffRegex;
    out << YAML::Key << "PackedDimensionBounds" << YAML::Value
        << std::string{toString(style.PackedDimensionBounds)};
    out << YAML::Key << "ParameterPortListIndentWidth" << YAML::Value
        << style.ParameterPortListIndentWidth;
    out << YAML::Key << "UnpackedDimensionBounds" << YAML::Value
        << std::string{toString(style.UnpackedDimensionBounds)};
    out << YAML::EndMap;
    out << YAML::EndDoc;
    return out.c_str();
}

void parseConfiguration(const YAML::Node& node, Style& style) {
    if (!node.IsMap()) {
        throw std::runtime_error("configuration must be a YAML mapping");
    }

    if (auto n = node["AlignConsecutiveAssignments"]) {
        parseAlignConsecutive(n, style.AlignConsecutiveAssignments);
    }

    if (auto n = node["AlignTrailingComments"]) {
        parseAlignConsecutive(n, style.AlignTrailingComments);
    }

    if (auto n = node["AlignConsecutiveDeclarations"]) {
        parseAlignConsecutive(n, style.AlignConsecutiveDeclarations);
    }

    if (auto n = node["AlignConsecutivePackedDimensions"]) {
        parseAlignConsecutive(n, style.AlignConsecutivePackedDimensions);
    }

    if (auto n = node["AlignConsecutiveTimingControls"]) {
        parseAlignConsecutive(n, style.AlignConsecutiveTimingControls);
    }

    if (auto v = node["MaxEmptyLinesToKeep"]) {
        style.MaxEmptyLinesToKeep = v.as<unsigned>();
    }

    bool hasContinuation{false};
    if (auto v = node["IndentWidth"]) {
        style.IndentWidth = v.as<unsigned>();
    }

    if (auto v = node["ContinuationIndentWidth"]) {
        style.ContinuationIndentWidth = v.as<unsigned>();
        hasContinuation = true;
    }

    bool hasParamPort{false};
    if (auto v = node["ParameterPortListIndentWidth"]) {
        style.ParameterPortListIndentWidth = v.as<unsigned>();
        hasParamPort = true;
    }

    if (!hasContinuation) {
        style.ContinuationIndentWidth = style.IndentWidth;
    }

    if (!hasParamPort) {
        style.ParameterPortListIndentWidth = style.ContinuationIndentWidth;
    }

    if (auto v = node["IndentCaseItem"]) {
        style.IndentCaseItem = v.as<bool>();
    }

    if (auto v = node["EventSeparator"]) {
        style.EventSeparator = parseEventSeparator(v.as<std::string>());
    }

    if (auto v = node["OneLineFormatOffRegex"]) {
        style.OneLineFormatOffRegex = v.as<std::string>();
    }

    if (auto v = node["PackedDimensionBounds"]) {
        style.PackedDimensionBounds = parseDimensionBounds(v.as<std::string>());
    }

    if (auto v = node["UnpackedDimensionBounds"]) {
        style.UnpackedDimensionBounds = parseDimensionBounds(v.as<std::string>());
    }

    parseBreakStyle(node, style);

    if (auto n = node["InsertBeginEnd"]) {
        parseInsertBeginEnd(n, style.InsertBeginEnd);
    }

    if (auto n = node["InsertParens"]) {
        parseInsertParens(n, style.InsertParens);
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

    auto result = findFileInHierarchy(searchDir, ConfigFileNames, loader);
    if (!result) {
        return getDefaultStyle();
    }

    auto& [path, content] = *result;
    try {
        YAML::Node const node = YAML::Load(content);
        auto style = getDefaultStyle();
        parseConfiguration(node, style);
        return style;
    }
    catch (const std::exception& e) {
        throw std::runtime_error(path.string() + ": " + e.what());
    }
}

} // namespace slang::format
