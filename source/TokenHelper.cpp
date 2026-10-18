// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "TokenHelper.h"

#include <algorithm>
#include <cstddef>

#include <slang/parsing/LexerFacts.h>
#include <slang/parsing/Token.h>
#include <slang/parsing/TokenKind.h>
#include <slang/syntax/SyntaxNode.h>
#include <slang/text/CharInfo.h>
#include <slang/util/Util.h>

using namespace slang;
using namespace slang::format;
using namespace slang::parsing;
using namespace slang::syntax;

namespace {

bool isIdentChar(char c) {
    return isValidCIdChar(c) || c == '$';
}

} // namespace

bool slang::format::hasLeadingNewline(Token tok) {
    if (!tok) {
        return false;
    }

    return std::ranges::any_of(tok.trivia(),
                               [](const auto& t) { return t.kind == TriviaKind::EndOfLine; });
}

bool slang::format::hasLeadingComment(Token tok) {
    if (!tok) {
        return false;
    }

    return std::ranges::any_of(tok.trivia(), [](const auto& t) {
        return t.kind == TriviaKind::LineComment || t.kind == TriviaKind::BlockComment;
    });
}

bool slang::format::hasLeadingTrailingComment(Token tok) {
    if (!tok) {
        return false;
    }

    for (const auto& t : tok.trivia()) {
        if (t.kind == TriviaKind::EndOfLine) {
            return false;
        }
        if (t.kind == TriviaKind::LineComment || t.kind == TriviaKind::BlockComment) {
            return true;
        }
    }
    return false;
}

bool slang::format::hasLeadingBlankLine(Token tok) {
    if (!tok) {
        return false;
    }

    bool sawNewline = false;
    for (const auto& t : tok.trivia()) {
        if (t.kind == TriviaKind::EndOfLine) {
            if (sawNewline) {
                return true;
            }
            sawNewline = true;
        }
        else if (t.kind != TriviaKind::Whitespace) {
            sawNewline = false;
        }
    }
    return false;
}

size_t slang::format::leadingWhitespaceLength(Token tok) {
    if (!tok) {
        return 0;
    }

    size_t len = 0;
    for (const auto& t : tok.trivia()) {
        if (t.kind == TriviaKind::Whitespace) {
            len += t.getRawText().length();
        }
    }
    return len;
}

bool slang::format::needsSeparator(char last, char next) {
    return isIdentChar(last) && isIdentChar(next);
}

bool slang::format::needsSpaceBetween(TokenKind prev, TokenKind curr) {
    if (prev == TokenKind::Unknown) {
        return false;
    }

    const bool prevIsKeyword = LexerFacts::isKeyword(prev);
    const bool currIsKeyword = LexerFacts::isKeyword(curr);

    if (prevIsKeyword && prev != TokenKind::NewKeyword &&
        (curr == TokenKind::OpenBrace || curr == TokenKind::OpenParenthesis ||
         curr == TokenKind::Hash || curr == TokenKind::IntegerBase ||
         curr == TokenKind::UnbasedUnsizedLiteral || curr == TokenKind::StringLiteral)) {
        return true;
    }

    if (prev == TokenKind::Identifier && (curr == TokenKind::OpenBrace || curr == TokenKind::At)) {
        return true;
    }

    if ((prev == TokenKind::CloseBrace || prev == TokenKind::Star) &&
        (currIsKeyword || curr == TokenKind::Identifier)) {
        return true;
    }
    if (prev == TokenKind::CloseParenthesis &&
        (currIsKeyword || curr == TokenKind::Identifier || curr == TokenKind::SystemIdentifier ||
         curr == TokenKind::OpenBrace || curr == TokenKind::OpenParenthesis ||
         curr == TokenKind::OpenBracket || curr == TokenKind::Exclamation)) {
        return true;
    }

    if ((prev == TokenKind::Colon || prev == TokenKind::StringLiteral) && currIsKeyword) {
        return true;
    }

    if (prev == TokenKind::EqualsArrow || curr == TokenKind::EqualsArrow) {
        return true;
    }

    if (curr == TokenKind::DoubleHash) {
        return true;
    }

    if (prev == TokenKind::CloseBracket && curr == TokenKind::DoubleHash) {
        return true;
    }

    return false;
}

size_t slang::format::flatWidth(const SyntaxNode& node, bool ignoreNewlines) {
    size_t width = 0;
    char lastChar = 0;
    auto prevKind = TokenKind::Unknown;
    for (auto it = node.tokens_begin(); it != node.tokens_end(); ++it) {
        auto tok = *it;
        for (const auto& t : tok.trivia()) {
            if (t.kind == TriviaKind::LineComment || t.kind == TriviaKind::BlockComment) {
                return 0;
            }
            if (!ignoreNewlines && t.kind == TriviaKind::EndOfLine) {
                return 0;
            }
        }

        auto raw = tok.rawText();
        if (raw.empty()) {
            continue;
        }

        if ((lastChar != 0 && needsSeparator(lastChar, raw.front())) ||
            needsSpaceBetween(prevKind, tok.kind)) {
            width++;
        }
        width += raw.size();
        lastChar = raw.back();
        prevKind = tok.kind;
    }
    return width;
}
