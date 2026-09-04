// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include "Style.h"

#include <memory>

#include <slang/syntax/SyntaxTree.h>

namespace slang::format {

/// Rewrites \p tree according to \p style options. Iterates until no bare
/// bodies remain, handling nested wrapping correctly. Returns the original
/// tree changes are needed.
std::shared_ptr<slang::syntax::SyntaxTree> applyBeginEndInsertion(
    std::shared_ptr<slang::syntax::SyntaxTree> tree, const Style& style);

/// Rewrites \p tree to normalize event expression separators according to \p style. Returns the
/// original tree if no changes are needed.
std::shared_ptr<slang::syntax::SyntaxTree> applyEventSeparator(
    std::shared_ptr<slang::syntax::SyntaxTree> tree, const Style& style);

/// Rewrites \p tree to normalize packed dimension bound ordering according to \p style. Returns the
/// original tree if no changes are needed.
std::shared_ptr<slang::syntax::SyntaxTree> applyPackedDimensionBounds(
    std::shared_ptr<slang::syntax::SyntaxTree> tree, const Style& style);

/// Rewrites \p tree to normalize unpacked dimension bound ordering according to \p style. Returns
/// the original tree if no changes are needed.
std::shared_ptr<slang::syntax::SyntaxTree> applyUnpackedDimensionBounds(
    std::shared_ptr<slang::syntax::SyntaxTree> tree, const Style& style);

} // namespace slang::format
