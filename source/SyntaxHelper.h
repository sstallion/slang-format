// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include <slang/syntax/AllSyntax.h>
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

/// Returns true if \p kind is a packed dimension parent.
inline bool isPackedDimensionParent(slang::syntax::SyntaxKind kind) {
    return slang::syntax::IntegerTypeSyntax::isKind(kind) ||
           slang::syntax::ImplicitTypeSyntax::isKind(kind) ||
           slang::syntax::StructUnionTypeSyntax::isKind(kind) ||
           slang::syntax::EnumTypeSyntax::isKind(kind);
}

/// Returns true if \p kind is an unpacked dimension parent.
inline bool isUnpackedDimensionParent(slang::syntax::SyntaxKind kind) {
    return slang::syntax::DeclaratorSyntax::isKind(kind);
}

} // namespace slang::format
