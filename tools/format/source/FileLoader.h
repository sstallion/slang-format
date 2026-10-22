// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include <filesystem>
#include <functional>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace slang::format {

/// Returns file content, or std::nullopt if the file does not exist.
using FileLoader = std::function<std::optional<std::string>(const std::filesystem::path&)>;

/// Walks upward from \p searchDir to the filesystem root, checking each
/// \p candidates filename at every level. Returns the matched file path and
/// content on first match, or std::nullopt if the root is reached.
std::optional<std::pair<std::filesystem::path, std::string>> findFileInHierarchy(
    const std::filesystem::path& searchDir, std::span<const std::string_view> candidates,
    const FileLoader& loader);

} // namespace slang::format
