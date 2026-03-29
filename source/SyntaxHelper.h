// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include <slang/syntax/SyntaxNode.h>

namespace slang::format {

/// Returns true if \p kind is an always block.
inline bool isAlwaysBlockKind(slang::syntax::SyntaxKind kind) {
    return kind == slang::syntax::SyntaxKind::AlwaysBlock ||
           kind == slang::syntax::SyntaxKind::AlwaysCombBlock ||
           kind == slang::syntax::SyntaxKind::AlwaysFFBlock ||
           kind == slang::syntax::SyntaxKind::AlwaysLatchBlock;
}

/// Returns true if \p kind is an initial block.
inline bool isInitialBlockKind(slang::syntax::SyntaxKind kind) {
    return kind == slang::syntax::SyntaxKind::InitialBlock ||
           kind == slang::syntax::SyntaxKind::FinalBlock;
}

} // namespace slang::format
