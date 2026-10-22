// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include "slang/format/Export.h"

#include <string_view>

namespace slang::format {

class SLANG_FORMAT_EXPORT VersionInfo {
public:
    VersionInfo() = delete;

    /// Gets the major version number of the library.
    static int getMajor();

    /// Gets the minor version number of the library.
    static int getMinor();

    /// Gets the patch version of the library.
    static int getPatch();

    /// Gets the prerelease version of library, or an empty string if
    /// prerelease information is not available.
    static std::string_view getPrerelease();

    /// Gets the build metadata for the library, or an empty string if build
    /// metadata is not available.
    static std::string_view getBuildMetadata();

    /// Gets the full version string of the library, formatted as
    /// `major.minor.patch[-prerelease][+build]`.
    static std::string_view getVersionString();
};

} // namespace slang::format
