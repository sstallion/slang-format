// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include "FileLoader.h"

#include <filesystem>

namespace slang::format {

/// Determines whether a file path is ignored by the nearest ignore file in
/// the directory hierarchy.
bool isIgnored(const std::filesystem::path& filePath, FileLoader loader = {});

} // namespace slang::format
