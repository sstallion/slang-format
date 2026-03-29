// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include "Style.h"

#include <memory>

#include <slang/syntax/SyntaxTree.h>

namespace slang::format {

/// Rewrites \p tree according to \p style options. Iterates until no bare bodies remain, handling
/// nested wrapping correctly. Returns the original tree changes are needed.
std::shared_ptr<slang::syntax::SyntaxTree> applyBeginEndInsertion(
    std::shared_ptr<slang::syntax::SyntaxTree> tree, const Style& style);

} // namespace slang::format
