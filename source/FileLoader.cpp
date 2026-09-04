// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "FileLoader.h"

#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>

using namespace slang::format;

namespace slang::format {

std::optional<std::pair<std::filesystem::path, std::string>> findFileInHierarchy(
    const std::filesystem::path& searchDir, std::span<const std::string_view> candidates,
    const FileLoader& loader) {
    for (std::filesystem::path path = searchDir;; path = path.parent_path()) {
        for (std::string_view const name : candidates) {
            auto candidate = path / name;
            if (auto content = loader(candidate)) {
                return std::pair{candidate, std::move(*content)};
            }
        }

        auto parent = path.parent_path();
        if (parent == path) {
            break;
        }
    }

    return std::nullopt;
}

} // namespace slang::format
