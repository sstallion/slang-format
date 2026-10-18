// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include "AlignClassifier.h"
#include "AlignGroup.h"

#include <string>
#include <string_view>
#include <vector>

namespace slang::format {

/// Aligns trailing comments within a group to the same column.
void alignGroupTrailingComments(std::string& result, const std::vector<std::string_view>& lines,
                                const std::vector<LineInfo>& infos, GroupRange range,
                                unsigned minSpaces);

} // namespace slang::format
