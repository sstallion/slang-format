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

/// Aligns a group of consecutive declarations by type width, identifier position, and equals sign.
void alignGroupDeclarations(std::string& result, const std::vector<std::string_view>& lines,
                            const std::vector<LineInfo>& infos, GroupRange range,
                            const InferContext& ctx);

/// Aligns a group of consecutive timing control assignments by delay width and equals sign.
void alignGroupTimingControls(std::string& result, const std::vector<std::string_view>& lines,
                              const std::vector<LineInfo>& infos, GroupRange range,
                              const InferContext& ctx);

} // namespace slang::format
