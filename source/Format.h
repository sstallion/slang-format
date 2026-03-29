// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include "Style.h"

#include <iosfwd>
#include <string>
#include <string_view>

namespace slang::format {

/// Reformats \p text according to \p style.
std::string reformat(std::string_view text, const Style& style);

/// Reformats the contents of \p stream according to \p style.
std::string reformat(std::istream& stream, const Style& style);

} // namespace slang::format
