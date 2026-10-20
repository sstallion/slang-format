// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include "FileLoader.h"
#include "slang/format/Style.h"

#include <filesystem>
#include <string>

#include <yaml-cpp/yaml.h>

namespace slang::format {

/// Serializes \p style to a YAML document string.
std::string dumpConfig(const Style& style);

/// Parses a YAML node into \p style. When the node contains BasedOnStyle, the named predefined
/// style replaces \p style before the remaining keys are applied. Throws std::runtime_error on
/// error, including an unknown BasedOnStyle name.
void parseConfig(const YAML::Node& node, Style& style);

/// Searches for a configuration file starting from \p searchDir, walking up the directory
/// hierarchy to the root. Returns the Default style if no configuration file is found.
Style loadConfig(const std::filesystem::path& searchDir, FileLoader loader = {});

} // namespace slang::format
