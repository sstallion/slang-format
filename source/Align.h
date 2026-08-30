// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace slang::format {

struct Style;

/// Applies all alignment post-processing passes to the formatted output.
std::string applyAlignment(const std::string& output, const Style& style,
                           const std::vector<unsigned>& lineDepths);

} // namespace slang::format
