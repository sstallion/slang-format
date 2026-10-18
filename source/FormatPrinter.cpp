// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "FormatPrinter.h"
#include "TokenHelper.h"

#include <string_view>
#include <utility>

#include <slang/parsing/Token.h>
#include <slang/parsing/TokenKind.h>
#include <slang/syntax/AllSyntax.h>
#include <slang/syntax/SyntaxKind.h>
#include <slang/syntax/SyntaxNode.h>
#include <slang/syntax/SyntaxTree.h>
#include <slang/util/Util.h>

using namespace slang;
using namespace slang::format;
using namespace slang::parsing;
using namespace slang::syntax;

namespace {

bool matchesPragma(std::string_view text, std::string_view pragma) {
    auto trim = [](std::string_view s) {
        while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) {
            s.remove_prefix(1);
        }

        while (!s.empty() &&
               (s.back() == ' ' || s.back() == '\t' || s.back() == '\n' || s.back() == '\r')) {
            s.remove_suffix(1);
        }
        return s;
    };

    text = trim(text);
    if (text.starts_with("//")) {
        text.remove_prefix(2);
        return trim(text) == pragma;
    }

    if (text.starts_with("/*") && text.ends_with("*/")) {
        text.remove_prefix(2);
        text.remove_suffix(2);
        return trim(text) == pragma;
    }

    return false;
}

bool isBlockOpen(TokenKind kind) {
    return kind == TokenKind::BeginKeyword || kind == TokenKind::ForkKeyword;
}

bool isBlockClose(TokenKind kind) {
    return kind == TokenKind::EndKeyword || kind == TokenKind::JoinKeyword ||
           kind == TokenKind::JoinAnyKeyword || kind == TokenKind::JoinNoneKeyword;
}

} // namespace

namespace slang::format {

void FormatPrinter::captureTokenSpacing(Token tok) {
    auto srcSpaces = static_cast<unsigned>(leadingWhitespaceLength(tok));
    unsigned outSpaces = 0;
    for (auto it = output.rbegin(); it != output.rend() && *it == ' '; ++it) {
        outSpaces++;
    }
    currentLineMeta.tokenSpacings.push_back({.sourceSpaces = srcSpaces, .outputSpaces = outSpaces});
}

void FormatPrinter::breakBeforeClose(Token closeTok) {
    if (!formatEnabled || atLineStart || hasLeadingNewline(closeTok)) {
        return;
    }

    if (closeTok.kind == TokenKind::CloseBrace) {
        spaceBeforeCloseBracePending = false;
    }
    else if (closeTok.kind == TokenKind::CloseParenthesis) {
        spaceBeforeCloseParenPending = false;
    }

    forceNewline();
}

void FormatPrinter::emitDeferredSpaces() {
    if (after.operatorToken && shouldSpaceAroundOperator()) {
        output += ' ';
    }
    if (after.comma && style.SpaceAfterComma) {
        output += ' ';
    }
    if (after.semicolon && style.SpaceAfterSemicolon) {
        output += ' ';
    }
    if (afterPackedDimCloseBracket && style.SpaceAfterBrackets.PackedDimensions) {
        output += ' ';
    }
    if (afterUnpackedDimCloseBracket && style.SpaceAfterBrackets.UnpackedDimensions) {
        output += ' ';
    }
    if (after.openBrace && style.SpacesInBraces) {
        output += ' ';
    }
    if (after.openBracket && inDimension() && style.SpacesInBrackets) {
        output += ' ';
    }
    if (after.openParen && style.SpacesInParens) {
        output += ' ';
    }
}

void FormatPrinter::emitDirectiveTrivia(const Trivia& t) {
    auto* node = t.syntax();
    if (node == nullptr) {
        return;
    }

    if (node->kind == SyntaxKind::MacroUsage) {
        skipExpandedToken = true;
    }

    auto it = node->tokens_begin();
    auto end = node->tokens_end();
    if (it == end) {
        return;
    }

    auto first = *it;
    for (const auto& inner : first.trivia()) {
        emitTrivia(inner);
    }

    if (atLineStart) {
        currentLineMeta.kind = LineMetadata::Kind::Comment;
    }

    emitRawText(first.rawText());
    lineDepth = depth;
    atLineStart = false;
    it++;

    for (; it != end; ++it) {
        auto tok = *it;
        for (const auto& inner : tok.trivia()) {
            if (inner.kind == TriviaKind::EndOfLine) {
                emitTrivia(inner);
            }
            else if (inner.kind == TriviaKind::Whitespace) {
                if (!formatEnabled || !atLineStart || depth == 0) {
                    output += inner.getRawText();
                }
            }
        }

        if (isBlockClose(tok.kind) && depth > 0) {
            depth--;
        }

        if (formatEnabled && atLineStart && depth > 0) {
            emitIndent();
        }
        emitRawText(tok.rawText());
        lineDepth = depth;
        atLineStart = false;

        if (isBlockOpen(tok.kind)) {
            depth++;
        }
    }

    lineDepth = depth;
    atLineStart = false;
}

void FormatPrinter::emitEndTokenTrivia(Token tok) {
    if (!tok || tok.isMissing()) {
        return;
    }

    auto trivia = tok.trivia();
    bool foundEol{false};
    for (const auto& t : trivia) {
        if (t.kind == TriviaKind::EndOfLine) {
            foundEol = true;
            break;
        }
    }

    if (!foundEol) {
        return;
    }

    for (const auto& t : trivia) {
        emitTrivia(t);
    }
    triviaSkip = trivia.size();
}

size_t FormatPrinter::indentWidth() const {
    if (parenAlignColumn > 0) {
        return parenAlignColumn;
    }
    if (portItemNextIndent) {
        return ((depth - 1) * style.IndentWidth) + style.ParameterPortListIndentWidth;
    }
    size_t w = static_cast<size_t>(depth) * style.IndentWidth;
    if (!nextIsPrimary) {
        w += style.ContinuationIndentWidth;
    }
    return w;
}

void FormatPrinter::emitIndent() {
    output.append(indentWidth(), ' ');
    if (parenAlignColumn == 0 && portItemNextIndent) {
        portItemNextIndent = false;
    }
}

size_t FormatPrinter::emitLeadingTrailingComment(Token tok) {
    size_t consumed = 0;
    for (const auto& t : tok.trivia()) {
        emitTrivia(t);
        consumed++;
        if (t.kind == TriviaKind::EndOfLine) {
            break;
        }
    }
    return consumed;
}

void FormatPrinter::emitRawText(std::string_view raw) {
    output += raw;
    for (auto ch : raw) {
        if (ch == '\n') {
            outputLineNumber++;
        }
    }
}

void FormatPrinter::emitSeparator(Token tok) {
    if (!formatEnabled || atLineStart || after.integerBase) {
        return;
    }

    auto raw = tok.rawText();
    if (raw.empty() || output.empty()) {
        return;
    }

    if (output.back() == ' ') {
        return;
    }

    if (suppressSeparator) {
        suppressSeparator = false;
        if (needsSeparator(output.back(), raw.front())) {
            output += ' ';
        }
        return;
    }

    if (needsSeparator(output.back(), raw.front()) || needsSpaceBetween(lastTokenKind, tok.kind)) {
        output += ' ';
    }
}

void FormatPrinter::emitToken(Token tok) {
    if (!tok || tok.isMissing() || tok.kind == TokenKind::Placeholder) {
        return;
    }

    auto trivia = tok.trivia();
    for (size_t i = triviaSkip; i < trivia.size(); i++) {
        emitTrivia(trivia[i]);
    }
    triviaSkip = 0;

    if (!atLineStart) {
        emitDeferredSpaces();
    }

    auto raw = tok.rawText();
    emitSeparator(tok);

    const bool wasLineStart = atLineStart;
    if (atLineStart) {
        if (nextLineKind) {
            currentLineMeta.kind = *nextLineKind;
            nextLineKind.reset();
        }

        if (formatEnabled && !raw.empty()) {
            emitIndent();
        }

        atLineStart = false;
    }

    if (formatEnabled && !atLineStart && needsStripBefore(tok.kind)) {
        stripTrailingSpaces();
    }

    if (formatEnabled && tok.kind == TokenKind::CloseBracket && bracketDepth > 0) {
        bracketDepth--;
    }

    if (formatEnabled && !atLineStart) {
        normalizeBeforeClose(tok.kind);
    }

    if (formatEnabled && !atLineStart && beforeOperator) {
        stripTrailingSpaces();
        if (shouldSpaceAroundOperator()) {
            output += ' ';
        }
    }
    normalizeBeforeOpen(tok.kind);

    if (!wasLineStart && formatEnabled && !skipSpacingCapture) {
        captureTokenSpacing(tok);
    }
    skipSpacingCapture = false;

    if (skipExpandedToken) {
        skipExpandedToken = false;
    }
    else {
        output += raw;
    }
    lineDepth = depth;
    nextIsPrimary = false;
    emptyLineCount = 0;
    updateAfterTokenFlags(tok);
    lastTokenKind = tok.kind;
}

void FormatPrinter::emitTrivia(const Trivia& t) {
    if (t.kind == TriviaKind::EndOfLine) {
        beforeOperator = false;
        after = {};
        spaceBeforeCloseBracePending = false;
        spaceBeforeCloseBracketPending = false;
        spaceBeforeCloseParenPending = false;
        suppressSeparator = false;

        if (atLineStart) {
            if (shouldSuppressBlankLine()) {
                return;
            }

            currentLineMeta.kind = LineMetadata::Kind::Empty;
            output += '\n';
            finalizeLine();
            lineStart = output.size();
        }
        else {
            emptyLineCount = 0;
            stripTrailingSpaces();
            output += '\n';
            finalizeLine();
            lineStart = output.size();
            atLineStart = true;
        }
        return;
    }

    if (t.kind == TriviaKind::Whitespace) {
        if (!formatEnabled) {
            output += t.getRawText();
        }
        return;
    }

    emitDeferredSpaces();
    beforeOperator = false;
    after = {};

    auto raw = t.getRawText();

    if (t.kind == TriviaKind::LineComment || t.kind == TriviaKind::BlockComment) {
        const bool isOff = formatEnabled && (matchesPragma(raw, "slang-format off") ||
                                             matchesPragma(raw, "verilog_format: off"));
        const bool isOn = !formatEnabled && (matchesPragma(raw, "slang-format on") ||
                                             matchesPragma(raw, "verilog_format: on"));

        if (atLineStart) {
            currentLineMeta.kind = LineMetadata::Kind::Comment;
        }
        else {
            padBeforeTrailingComment();
        }

        // Indent while still in the current format state (before toggling).
        if (formatEnabled && atLineStart) {
            output.append(indentWidth(), ' ');
        }
        output += raw;
        lineDepth = depth;
        atLineStart = false; // just emitted text on this line

        formatEnabled = isOn || (formatEnabled && !isOff);
        return;
    }

    if (t.kind == TriviaKind::Directive) {
        emitDirectiveTrivia(t);
        return;
    }

    output += raw;
}

void FormatPrinter::ensureSingleSpace() {
    if (!formatEnabled || atLineStart) {
        return;
    }

    stripTrailingSpaces();
    output += ' ';
}

void FormatPrinter::ensureSpace() {
    if (formatEnabled && !atLineStart && !output.empty() && output.back() != ' ') {
        output += ' ';
    }
}

void FormatPrinter::finalizeLine() {
    currentLineMeta.depth = lineDepth;
    currentLineMeta.lineNumber = outputLineNumber++;
    lineMetadata.push_back(std::move(currentLineMeta));
    currentLineMeta = {};
}

void FormatPrinter::forceNewline() {
    while (!output.empty() && output.back() == ' ') {
        output.pop_back();
    }
    output += '\n';
    finalizeLine();
    lineStart = output.size();
    atLineStart = true;
}

bool FormatPrinter::inDimension() const {
    return inPackedDimension || inUnpackedDimension;
}

void FormatPrinter::insertBlankLineBefore(Token firstToken) {
    if (hasLeadingBlankLine(firstToken) || outputHasBlankLine()) {
        return;
    }

    if (hasLeadingTrailingComment(firstToken)) {
        triviaSkip = emitLeadingTrailingComment(firstToken);
    }

    if (!atLineStart) {
        forceNewline();
    }

    output += '\n';
    currentLineMeta.kind = LineMetadata::Kind::Empty;
    finalizeLine();
    lineStart = output.size();
    emptyLineCount = style.MaxEmptyLinesToKeep;
}

bool FormatPrinter::needsStripBefore(TokenKind kind) {
    return kind == TokenKind::Comma || kind == TokenKind::Semicolon;
}

void FormatPrinter::normalizeBeforeClose(TokenKind kind) {
    if (spaceBeforeCloseBracePending && kind == TokenKind::CloseBrace) {
        stripTrailingSpaces();
        if (style.SpacesInBraces) {
            output += ' ';
        }
    }
    if (spaceBeforeCloseBracketPending && kind == TokenKind::CloseBracket) {
        stripTrailingSpaces();
        if (inDimension() && style.SpacesInBrackets) {
            output += ' ';
        }
    }
    if (spaceBeforeCloseParenPending && kind == TokenKind::CloseParenthesis) {
        stripTrailingSpaces();
        if (style.SpacesInParens) {
            output += ' ';
        }
    }
}

void FormatPrinter::normalizeBeforeOpen(TokenKind kind) {
    if (!formatEnabled || atLineStart) {
        return;
    }

    if (kind == TokenKind::OpenBracket && !output.empty() && output.back() != ' ') {
        if ((inPackedDimension && style.SpaceBeforeBrackets.PackedDimensions) ||
            (inUnpackedDimension && style.SpaceBeforeBrackets.UnpackedDimensions)) {
            output += ' ';
        }
    }
}

void FormatPrinter::normalizeSpaceBefore(Token next, bool wantSpace, bool suppressNextSeparator) {
    if (!formatEnabled || atLineStart || output.empty() || hasLeadingNewline(next)) {
        return;
    }

    if (wantSpace) {
        ensureSpace();
    }
    else {
        stripTrailingSpaces();
    }

    if (suppressNextSeparator) {
        suppressSeparator = true;
    }
}

bool FormatPrinter::outputHasBlankLine() const {
    return atLineStart && output.size() >= 2 && output[output.size() - 1] == '\n' &&
           output[output.size() - 2] == '\n';
}

void FormatPrinter::padBeforeTrailingComment() {
    if (!formatEnabled) {
        return;
    }
    size_t existingSpaces = 0;
    for (auto it = output.rbegin(); it != output.rend() && *it == ' '; ++it) {
        existingSpaces++;
    }
    if (existingSpaces < style.SpacesBeforeTrailingComments) {
        output.append(style.SpacesBeforeTrailingComments - existingSpaces, ' ');
    }
}

FormatPrinter::Result FormatPrinter::print(const SyntaxTree& tree) {
    tree.root().visit(*this);

    // The EndOfFile token is not part of the visited subtree when tree.root()
    // returns a member node (e.g. ModuleDeclarationSyntax). Emit its
    // leading trivia, which carries the final newlines from the source.
    if (tree.root().kind != SyntaxKind::CompilationUnit) {
        emitToken(tree.getMetadata().eofToken);
    }
    return {.output = std::move(output), .lineMetadata = std::move(lineMetadata)};
}

bool FormatPrinter::shouldSpaceAroundOperator() const {
    if (!style.SpaceAroundOperators) {
        return false;
    }
    if (bracketDepth > 0 && !style.SpacesInBrackets) {
        return false;
    }
    return true;
}

bool FormatPrinter::shouldSuppressBlankLine() {
    if (style.RemoveEmptyLines || emptyLineCount > 0) {
        emptyLineCount++;
    }
    return formatEnabled && emptyLineCount > style.MaxEmptyLinesToKeep;
}

void FormatPrinter::stripTrailingSpaces() {
    while (output.size() > lineStart && output.back() == ' ') {
        output.pop_back();
    }
}

void FormatPrinter::updateAfterTokenFlags(Token tok) {
    after = {
        .comma = formatEnabled && tok.kind == TokenKind::Comma,
        .integerBase = formatEnabled && tok.kind == TokenKind::IntegerBase,
        .openBrace = formatEnabled && (tok.kind == TokenKind::OpenBrace ||
                                       tok.kind == TokenKind::ApostropheOpenBrace),
        .openBracket = formatEnabled && tok.kind == TokenKind::OpenBracket,
        .openParen = formatEnabled && tok.kind == TokenKind::OpenParenthesis,
        .operatorToken = formatEnabled && beforeOperator,
        .semicolon = formatEnabled && tok.kind == TokenKind::Semicolon,
    };
    beforeOperator = false;

    if (after.openBrace) {
        spaceBeforeCloseBracePending = true;
    }
    if (after.openBracket) {
        spaceBeforeCloseBracketPending = true;
        bracketDepth++;
    }
    if (after.openParen) {
        spaceBeforeCloseParenPending = true;
    }
    const bool afterCloseBracket = formatEnabled && tok.kind == TokenKind::CloseBracket;
    afterPackedDimCloseBracket = afterCloseBracket && inPackedDimension;
    afterUnpackedDimCloseBracket = afterCloseBracket && inUnpackedDimension;
}

void FormatPrinter::visitBody(const StatementSyntax& body) {
    if (BlockStatementSyntax::isKind(body.kind)) {
        nextIsPrimary = true;
        body.visit(*this);
    }
    else {
        depth++;
        nextIsPrimary = true;
        body.visit(*this);
        depth--;
    }
}

void FormatPrinter::visitToken(Token tok) {
    emitToken(tok);
}

} // namespace slang::format
