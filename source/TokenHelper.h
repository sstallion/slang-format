// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include <cstddef>

#include <slang/parsing/Token.h>
#include <slang/parsing/TokenKind.h>
#include <slang/syntax/SyntaxNode.h>

namespace slang::format {

/// Returns true if \p tok has end-of-line trivia.
bool hasLeadingNewline(slang::parsing::Token tok);

/// Returns true if \p tok has line or block comment trivia.
bool hasLeadingComment(slang::parsing::Token tok);

/// Returns true if \p tok has comment trivia before its first end-of-line trivia.
bool hasLeadingTrailingComment(slang::parsing::Token tok);

/// Returns true if the trivia of \p tok contains a blank line.
bool hasLeadingBlankLine(slang::parsing::Token tok);

/// Returns the total width of whitespace trivia on \p tok.
size_t leadingWhitespaceLength(slang::parsing::Token tok);

/// Returns true if \p last and \p next would merge into a single token without a separator.
bool needsSeparator(char last, char next);

/// Returns true if a space is required between tokens of kind \p prev and \p curr.
bool needsSpaceBetween(slang::parsing::TokenKind prev, slang::parsing::TokenKind curr);

/// Returns the width of \p node flattened onto one line, or 0 if it contains a comment, or a
/// newline unless \p ignoreNewlines is true.
size_t flatWidth(const slang::syntax::SyntaxNode& node, bool ignoreNewlines = false);

} // namespace slang::format
