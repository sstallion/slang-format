// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "FileLoader.h"
#include "Style.h"

#include <algorithm>
#include <array>
#include <cstddef>
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

constexpr std::string_view toString(BracketAlignmentStyle style) {
    switch (style) {
        case BracketAlignmentStyle::Align:
            return "Align";
        case BracketAlignmentStyle::AlwaysBreak:
            return "AlwaysBreak";
        case BracketAlignmentStyle::BlockIndent:
            return "BlockIndent";
        case BracketAlignmentStyle::DontAlign:
            return "DontAlign";
    }
    return "Align";
}

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

constexpr std::string_view toString(LineEndingStyle style) {
    switch (style) {
        case LineEndingStyle::CRLF:
            return "CRLF";
        case LineEndingStyle::DeriveCRLF:
            return "DeriveCRLF";
        case LineEndingStyle::DeriveLF:
            return "DeriveLF";
        case LineEndingStyle::LF:
            return "LF";
    }
    return "DeriveLF";
}

constexpr std::string_view toString(RangeSpacingStyle style) {
    switch (style) {
        case RangeSpacingStyle::Always:
            return "Always";
        case RangeSpacingStyle::MatchSource:
            return "MatchSource";
        case RangeSpacingStyle::Never:
            return "Never";
        case RangeSpacingStyle::Preserve:
            return "Preserve";
    }
    return "MatchSource";
}

constexpr std::array AllBlockBreakStyles{BlockBreakStyle::Always, BlockBreakStyle::Never,
                                         BlockBreakStyle::OnlyMultiline};
constexpr std::array AllBracketAlignmentStyles{BracketAlignmentStyle::Align,
                                               BracketAlignmentStyle::AlwaysBreak,
                                               BracketAlignmentStyle::BlockIndent,
                                               BracketAlignmentStyle::DontAlign};
constexpr std::array AllDimensionBoundsStyles{DimensionBoundsStyle::LSBFirst,
                                              DimensionBoundsStyle::MSBFirst,
                                              DimensionBoundsStyle::Preserve};
constexpr std::array AllEventSeparatorStyles{EventSeparatorStyle::Comma, EventSeparatorStyle::Or,
                                             EventSeparatorStyle::Preserve};
constexpr std::array AllLineEndingStyles{LineEndingStyle::CRLF, LineEndingStyle::DeriveCRLF,
                                         LineEndingStyle::DeriveLF, LineEndingStyle::LF};
constexpr std::array AllRangeSpacingStyles{RangeSpacingStyle::Always,
                                           RangeSpacingStyle::MatchSource, RangeSpacingStyle::Never,
                                           RangeSpacingStyle::Preserve};

template<typename T, size_t N>
void parseEnum(const YAML::Node& node, std::string_view key, T& target,
               const std::array<T, N>& values, T fallback) {
    const auto v = node[key];
    if (!v) {
        return;
    }

    const auto text = v.as<std::string>();
    const auto it = std::ranges::find_if(values,
                                                [&](T value) { return toString(value) == text; });
    target = it != values.end() ? *it : fallback;
}

void parseBool(const YAML::Node& node, std::string_view key, bool& target) {
    if (auto v = node[key]) {
        target = v.as<bool>();
    }
}

void parseUnsigned(const YAML::Node& node, std::string_view key, unsigned& target) {
    if (auto v = node[key]) {
        target = v.as<unsigned>();
    }
}

void parseString(const YAML::Node& node, std::string_view key, std::string& target) {
    if (auto v = node[key]) {
        target = v.as<std::string>();
    }
}

YAML::Node nodeWithAlias(const YAML::Node& node, std::string_view key, std::string_view alias) {
    auto v = node[key];
    return v ? v : node[alias];
}

void parseAlignConsecutive(const YAML::Node& parent, std::string_view key,
                           AlignConsecutiveStyle& config) {
    const auto node = parent[key];
    if (!node) {
        return;
    }

    parseBool(node, "Enabled", config.Enabled);
    parseBool(node, "AcrossComments", config.AcrossComments);
    parseBool(node, "AcrossEmptyLines", config.AcrossEmptyLines);
    parseBool(node, "AcrossParameterPortList", config.AcrossParameterPortList);
    parseBool(node, "AlignColon", config.AlignColon);
    parseBool(node, "InferFromSource", config.InferFromSource);
    parseUnsigned(node, "MaxPadding", config.MaxPadding);
    parseUnsigned(node, "MinPadding", config.MinPadding);
    parseBool(node, "PadLeft", config.PadLeft);
    parseBool(node, "PadRight", config.PadRight);
}

void parseInsertBeginEnd(const YAML::Node& parent, std::string_view key,
                         InsertBeginEndStyle& config) {
    const auto node = parent[key];
    if (!node) {
        return;
    }

    parseBool(node, "Enabled", config.Enabled);
    parseBool(node, "AlwaysStatements", config.AlwaysStatements);
    parseBool(node, "ControlStatements", config.ControlStatements);
    parseBool(node, "InitialStatements", config.InitialStatements);
}

void parseInsertParens(const YAML::Node& parent, std::string_view key, InsertParensStyle& config) {
    const auto node = parent[key];
    if (!node) {
        return;
    }

    parseBool(node, "Delays", config.Delays);
    parseBool(node, "ExpressionEvents", config.ExpressionEvents);
    parseBool(node, "ImplicitEvents", config.ImplicitEvents);
    parseBool(node, "NamedEvents", config.NamedEvents);
}

void parseSpaceAfterBrackets(const YAML::Node& parent, std::string_view key,
                             SpaceAfterBracketsStyle& config) {
    const auto node = parent[key];
    if (!node) {
        return;
    }

    parseBool(node, "PackedDimensions", config.PackedDimensions);
    parseBool(node, "UnpackedDimensions", config.UnpackedDimensions);
}

void parseSpaceAfterParens(const YAML::Node& parent, std::string_view key,
                           SpaceAfterParensStyle& config) {
    const auto node = parent[key];
    if (!node) {
        return;
    }

    parseBool(node, "ControlStatements", config.ControlStatements);
    parseBool(node, "EventControls", config.EventControls);
}

void parseSpaceBeforeBrackets(const YAML::Node& parent, std::string_view key,
                              SpaceBeforeBracketsStyle& config) {
    const auto node = parent[key];
    if (!node) {
        return;
    }

    parseBool(node, "PackedDimensions", config.PackedDimensions);
    parseBool(node, "UnpackedDimensions", config.UnpackedDimensions);
}

void parseSpaceBeforeParens(const YAML::Node& parent, std::string_view key,
                            SpaceBeforeParensStyle& config) {
    const auto node = parent[key];
    if (!node) {
        return;
    }

    parseBool(node, "ControlStatements", config.ControlStatements);
    parseBool(node, "ParameterList", config.ParameterList);
    parseBool(node, "PortList", config.PortList);
}

struct AlignConsecutiveFields {
    bool acrossParameterPortList = false;
    bool alignColon = false;
    bool inferFromSource = false;
    bool maxPadding = false;
    bool minPadding = false;
    bool padLeftRight = false;
};

void emitAlignConsecutive(YAML::Emitter& out, const AlignConsecutiveStyle& config,
                          AlignConsecutiveFields fields) {
    out << YAML::BeginMap;
    out << YAML::Key << "Enabled" << YAML::Value << config.Enabled;
    out << YAML::Key << "AcrossComments" << YAML::Value << config.AcrossComments;
    out << YAML::Key << "AcrossEmptyLines" << YAML::Value << config.AcrossEmptyLines;
    if (fields.acrossParameterPortList) {
        out << YAML::Key << "AcrossParameterPortList" << YAML::Value
            << config.AcrossParameterPortList;
    }
    if (fields.alignColon) {
        out << YAML::Key << "AlignColon" << YAML::Value << config.AlignColon;
    }
    if (fields.inferFromSource) {
        out << YAML::Key << "InferFromSource" << YAML::Value << config.InferFromSource;
    }
    if (fields.maxPadding) {
        out << YAML::Key << "MaxPadding" << YAML::Value << config.MaxPadding;
    }
    if (fields.minPadding) {
        out << YAML::Key << "MinPadding" << YAML::Value << config.MinPadding;
    }
    if (fields.padLeftRight) {
        out << YAML::Key << "PadLeft" << YAML::Value << config.PadLeft;
        out << YAML::Key << "PadRight" << YAML::Value << config.PadRight;
    }
    out << YAML::EndMap;
}

void emitInsertBeginEnd(YAML::Emitter& out, const InsertBeginEndStyle& config) {
    out << YAML::BeginMap;
    out << YAML::Key << "Enabled" << YAML::Value << config.Enabled;
    out << YAML::Key << "AlwaysStatements" << YAML::Value << config.AlwaysStatements;
    out << YAML::Key << "ControlStatements" << YAML::Value << config.ControlStatements;
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

void emitSpaceAfterBrackets(YAML::Emitter& out, const SpaceAfterBracketsStyle& config) {
    out << YAML::BeginMap;
    out << YAML::Key << "PackedDimensions" << YAML::Value << config.PackedDimensions;
    out << YAML::Key << "UnpackedDimensions" << YAML::Value << config.UnpackedDimensions;
    out << YAML::EndMap;
}

void emitSpaceAfterParens(YAML::Emitter& out, const SpaceAfterParensStyle& config) {
    out << YAML::BeginMap;
    out << YAML::Key << "ControlStatements" << YAML::Value << config.ControlStatements;
    out << YAML::Key << "EventControls" << YAML::Value << config.EventControls;
    out << YAML::EndMap;
}

void emitSpaceBeforeBrackets(YAML::Emitter& out, const SpaceBeforeBracketsStyle& config) {
    out << YAML::BeginMap;
    out << YAML::Key << "PackedDimensions" << YAML::Value << config.PackedDimensions;
    out << YAML::Key << "UnpackedDimensions" << YAML::Value << config.UnpackedDimensions;
    out << YAML::EndMap;
}

void emitSpaceBeforeParens(YAML::Emitter& out, const SpaceBeforeParensStyle& config) {
    out << YAML::BeginMap;
    out << YAML::Key << "ControlStatements" << YAML::Value << config.ControlStatements;
    out << YAML::Key << "ParameterList" << YAML::Value << config.ParameterList;
    out << YAML::Key << "PortList" << YAML::Value << config.PortList;
    out << YAML::EndMap;
}

} // namespace

namespace slang::format {

Style getDefaultStyle() {
    return {};
}

std::string dumpConfiguration(const Style& style) {
    constexpr AlignConsecutiveFields assignFields{.acrossParameterPortList = true,
                                                  .inferFromSource = true,
                                                  .maxPadding = true,
                                                  .minPadding = true};
    constexpr AlignConsecutiveFields declFields{.acrossParameterPortList = true,
                                                .inferFromSource = true};
    constexpr AlignConsecutiveFields portListFields{.acrossParameterPortList = true};
    constexpr AlignConsecutiveFields allFields{.acrossParameterPortList = true,
                                               .alignColon = true,
                                               .inferFromSource = true,
                                               .padLeftRight = true};

    YAML::Emitter out;
    out << YAML::BeginDoc;
    out << YAML::BeginMap;
    out << YAML::Key << "AlignAfterOpenParen" << YAML::Value
        << std::string{toString(style.AlignAfterOpenParen)};
    out << YAML::Key << "AlignConsecutiveAssignments" << YAML::Value;
    emitAlignConsecutive(out, style.AlignConsecutiveAssignments, assignFields);
    out << YAML::Key << "AlignConsecutiveDeclarations" << YAML::Value;
    emitAlignConsecutive(out, style.AlignConsecutiveDeclarations, declFields);
    out << YAML::Key << "AlignConsecutivePackedDimensions" << YAML::Value;
    emitAlignConsecutive(out, style.AlignConsecutivePackedDimensions, allFields);
    out << YAML::Key << "AlignConsecutivePortConnections" << YAML::Value;
    emitAlignConsecutive(out, style.AlignConsecutivePortConnections, assignFields);
    out << YAML::Key << "AlignConsecutiveTimingControls" << YAML::Value;
    emitAlignConsecutive(out, style.AlignConsecutiveTimingControls, {});
    out << YAML::Key << "AlignTrailingComments" << YAML::Value;
    emitAlignConsecutive(out, style.AlignTrailingComments, portListFields);
    out << YAML::Key << "AllowShortConstraintBlocksOnASingleLine" << YAML::Value
        << style.AllowShortConstraintBlocksOnASingleLine;
    out << YAML::Key << "AllowShortCoverpointsOnASingleLine" << YAML::Value
        << style.AllowShortCoverpointsOnASingleLine;
    out << YAML::Key << "BinPackArguments" << YAML::Value << style.BinPackArguments;
    out << YAML::Key << "BinPackParameters" << YAML::Value << style.BinPackParameters;
    out << YAML::Key << "BinPackPortConnections" << YAML::Value << style.BinPackPortConnections;
    out << YAML::Key << "BreakAfterAlways" << YAML::Value
        << std::string{toString(style.BreakAfterAlways)};
    out << YAML::Key << "BreakAfterAssertionLabel" << YAML::Value << style.BreakAfterAssertionLabel;
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
    out << YAML::Key << "ColumnLimit" << YAML::Value << style.ColumnLimit;
    out << YAML::Key << "CompactControlStatements" << YAML::Value << style.CompactControlStatements;
    out << YAML::Key << "CompactTimeUnits" << YAML::Value << style.CompactTimeUnits;
    out << YAML::Key << "ContinuationIndentWidth" << YAML::Value << style.ContinuationIndentWidth;
    out << YAML::Key << "EventSeparator" << YAML::Value
        << std::string{toString(style.EventSeparator)};
    out << YAML::Key << "IndentCaseItem" << YAML::Value << style.IndentCaseItem;
    out << YAML::Key << "IndentWidth" << YAML::Value << style.IndentWidth;
    out << YAML::Key << "InsertBeginEnd" << YAML::Value;
    emitInsertBeginEnd(out, style.InsertBeginEnd);
    out << YAML::Key << "InsertParens" << YAML::Value;
    emitInsertParens(out, style.InsertParens);
    out << YAML::Key << "LineEnding" << YAML::Value << std::string{toString(style.LineEnding)};
    out << YAML::Key << "MaxEmptyLinesToKeep" << YAML::Value << style.MaxEmptyLinesToKeep;
    out << YAML::Key << "OneLineFormatOffRegex" << YAML::Value << style.OneLineFormatOffRegex;
    out << YAML::Key << "OneStatementPerLine" << YAML::Value << style.OneStatementPerLine;
    out << YAML::Key << "PackedDimensionBounds" << YAML::Value
        << std::string{toString(style.PackedDimensionBounds)};
    out << YAML::Key << "ParameterPortListIndentWidth" << YAML::Value
        << style.ParameterPortListIndentWidth;
    out << YAML::Key << "RangeSpacing" << YAML::Value << std::string{toString(style.RangeSpacing)};
    out << YAML::Key << "RemoveEmptyLines" << YAML::Value << style.RemoveEmptyLines;
    out << YAML::Key << "SpaceAfterAlways" << YAML::Value << style.SpaceAfterAlways;
    out << YAML::Key << "SpaceAfterBrackets" << YAML::Value;
    emitSpaceAfterBrackets(out, style.SpaceAfterBrackets);
    out << YAML::Key << "SpaceAfterCaseColon" << YAML::Value << style.SpaceAfterCaseColon;
    out << YAML::Key << "SpaceAfterComma" << YAML::Value << style.SpaceAfterComma;
    out << YAML::Key << "SpaceAfterParens" << YAML::Value;
    emitSpaceAfterParens(out, style.SpaceAfterParens);
    out << YAML::Key << "SpaceAfterSemicolon" << YAML::Value << style.SpaceAfterSemicolon;
    out << YAML::Key << "SpaceAroundOperators" << YAML::Value << style.SpaceAroundOperators;
    out << YAML::Key << "SpaceBeforeBrackets" << YAML::Value;
    emitSpaceBeforeBrackets(out, style.SpaceBeforeBrackets);
    out << YAML::Key << "SpaceBeforeCaseColon" << YAML::Value << style.SpaceBeforeCaseColon;
    out << YAML::Key << "SpaceBeforeParens" << YAML::Value;
    emitSpaceBeforeParens(out, style.SpaceBeforeParens);
    out << YAML::Key << "SpacesBeforeTrailingComments" << YAML::Value
        << style.SpacesBeforeTrailingComments;
    out << YAML::Key << "SpacesInBraces" << YAML::Value << style.SpacesInBraces;
    out << YAML::Key << "SpacesInBrackets" << YAML::Value << style.SpacesInBrackets;
    out << YAML::Key << "SpacesInParens" << YAML::Value << style.SpacesInParens;
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

    parseEnum(node, "AlignAfterOpenParen", style.AlignAfterOpenParen, AllBracketAlignmentStyles,
              BracketAlignmentStyle::Align);
    parseAlignConsecutive(node, "AlignConsecutiveAssignments", style.AlignConsecutiveAssignments);
    parseAlignConsecutive(node, "AlignConsecutiveDeclarations", style.AlignConsecutiveDeclarations);
    parseAlignConsecutive(node, "AlignConsecutivePackedDimensions",
                          style.AlignConsecutivePackedDimensions);
    parseAlignConsecutive(node, "AlignConsecutivePortConnections",
                          style.AlignConsecutivePortConnections);
    parseAlignConsecutive(node, "AlignConsecutiveTimingControls",
                          style.AlignConsecutiveTimingControls);
    parseAlignConsecutive(node, "AlignTrailingComments", style.AlignTrailingComments);
    parseBool(node, "AllowShortConstraintBlocksOnASingleLine",
              style.AllowShortConstraintBlocksOnASingleLine);
    parseBool(node, "AllowShortCoverpointsOnASingleLine", style.AllowShortCoverpointsOnASingleLine);
    parseBool(node, "BinPackArguments", style.BinPackArguments);
    parseBool(node, "BinPackParameters", style.BinPackParameters);
    parseBool(node, "BinPackPortConnections", style.BinPackPortConnections);
    parseEnum(node, "BreakAfterAlways", style.BreakAfterAlways, AllBlockBreakStyles,
              BlockBreakStyle::Never);
    parseBool(node, "BreakAfterAssertionLabel", style.BreakAfterAssertionLabel);
    parseBool(node, "BreakAfterBegin", style.BreakAfterBegin);
    parseEnum(node, "BreakAfterInitial", style.BreakAfterInitial, AllBlockBreakStyles,
              BlockBreakStyle::Never);
    parseEnum(node, "BreakBeforeAlways", style.BreakBeforeAlways, AllBlockBreakStyles,
              BlockBreakStyle::Never);
    parseBool(node, "BreakBeforeEnd", style.BreakBeforeEnd);
    parseBool(node, "BreakBeforeFunction", style.BreakBeforeFunction);
    parseEnum(node, "BreakBeforeInitial", style.BreakBeforeInitial, AllBlockBreakStyles,
              BlockBreakStyle::Never);
    parseBool(node, "BreakBeforeSpecifyBlock", style.BreakBeforeSpecifyBlock);
    parseBool(node, "BreakBeforeTask", style.BreakBeforeTask);
    parseUnsigned(node, "ColumnLimit", style.ColumnLimit);
    if (auto v = nodeWithAlias(node, "CompactControlStatements", "CompactConditionals")) {
        style.CompactControlStatements = v.as<bool>();
    }
    parseBool(node, "CompactTimeUnits", style.CompactTimeUnits);
    parseEnum(node, "EventSeparator", style.EventSeparator, AllEventSeparatorStyles,
              EventSeparatorStyle::Preserve);
    parseBool(node, "IndentCaseItem", style.IndentCaseItem);
    // ContinuationIndentWidth and ParameterPortListIndentWidth default to preceding widths.
    parseUnsigned(node, "IndentWidth", style.IndentWidth);
    style.ContinuationIndentWidth = style.IndentWidth;
    parseUnsigned(node, "ContinuationIndentWidth", style.ContinuationIndentWidth);
    style.ParameterPortListIndentWidth = style.ContinuationIndentWidth;
    parseUnsigned(node, "ParameterPortListIndentWidth", style.ParameterPortListIndentWidth);
    parseInsertBeginEnd(node, "InsertBeginEnd", style.InsertBeginEnd);
    parseInsertParens(node, "InsertParens", style.InsertParens);
    parseEnum(node, "LineEnding", style.LineEnding, AllLineEndingStyles, LineEndingStyle::DeriveLF);
    parseUnsigned(node, "MaxEmptyLinesToKeep", style.MaxEmptyLinesToKeep);
    parseString(node, "OneLineFormatOffRegex", style.OneLineFormatOffRegex);
    parseBool(node, "OneStatementPerLine", style.OneStatementPerLine);
    parseEnum(node, "PackedDimensionBounds", style.PackedDimensionBounds, AllDimensionBoundsStyles,
              DimensionBoundsStyle::Preserve);
    parseEnum(node, "RangeSpacing", style.RangeSpacing, AllRangeSpacingStyles,
              RangeSpacingStyle::MatchSource);
    parseBool(node, "RemoveEmptyLines", style.RemoveEmptyLines);
    parseBool(node, "SpaceAfterAlways", style.SpaceAfterAlways);
    parseSpaceAfterBrackets(node, "SpaceAfterBrackets", style.SpaceAfterBrackets);
    parseBool(node, "SpaceAfterCaseColon", style.SpaceAfterCaseColon);
    parseBool(node, "SpaceAfterComma", style.SpaceAfterComma);
    parseSpaceAfterParens(node, "SpaceAfterParens", style.SpaceAfterParens);
    parseBool(node, "SpaceAfterSemicolon", style.SpaceAfterSemicolon);
    parseBool(node, "SpaceAroundOperators", style.SpaceAroundOperators);
    parseSpaceBeforeBrackets(node, "SpaceBeforeBrackets", style.SpaceBeforeBrackets);
    parseBool(node, "SpaceBeforeCaseColon", style.SpaceBeforeCaseColon);
    parseSpaceBeforeParens(node, "SpaceBeforeParens", style.SpaceBeforeParens);
    parseUnsigned(node, "SpacesBeforeTrailingComments", style.SpacesBeforeTrailingComments);
    parseBool(node, "SpacesInBraces", style.SpacesInBraces);
    parseBool(node, "SpacesInBrackets", style.SpacesInBrackets);
    parseBool(node, "SpacesInParens", style.SpacesInParens);
    parseEnum(node, "UnpackedDimensionBounds", style.UnpackedDimensionBounds,
              AllDimensionBoundsStyles, DimensionBoundsStyle::Preserve);
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
        const YAML::Node node = YAML::Load(content);
        auto style = getDefaultStyle();
        parseConfiguration(node, style);
        return style;
    }
    catch (const std::exception& e) {
        throw std::runtime_error(path.string() + ": " + e.what());
    }
}

} // namespace slang::format
