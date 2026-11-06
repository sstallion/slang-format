// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include "FileLoader.h"

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <utility>

/// Returns a FileLoader that provides the given path map and reports every other path as missing.
inline slang::format::FileLoader makeFileLoader(
    std::map<std::filesystem::path, std::string> files) {
    return
        [files = std::move(files)](const std::filesystem::path& p) -> std::optional<std::string> {
            const auto it = files.find(p);
            if (it == files.end()) {
                return std::nullopt;
            }

            return it->second;
        };
}
