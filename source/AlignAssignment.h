// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include "AlignClassifier.h"
#include "AlignGroup.h"
#include "AlignInference.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace slang::format {

/// Aligns a group of consecutive assignments by equals sign position.
void alignGroupAssignments(std::string& result, const std::vector<std::string_view>& lines,
                           const std::vector<LineInfo>& infos, GroupRange range,
                           const InferContext& ctx);

/// Aligns a group of consecutive port connections by opening parenthesis position.
void alignGroupPortConnections(std::string& result, const std::vector<std::string_view>& lines,
                               const std::vector<LineInfo>& infos, GroupRange range,
                               const InferContext& ctx);

} // namespace slang::format
