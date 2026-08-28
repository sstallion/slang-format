// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include <filesystem>
#include <functional>
#include <optional>
#include <string>

namespace slang::format {

/// Returns file content, or std::nullopt if the file does not exist.
using FileLoader = std::function<std::optional<std::string>(const std::filesystem::path&)>;

/// Determines whether a file path is ignored by the nearest ignore file in
/// the directory hierarchy.
bool isIgnored(const std::filesystem::path& filePath, FileLoader loader = {});

} // namespace slang::format
