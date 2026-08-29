// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "Rewriter.h"
#include "Style.h"
#include "SyntaxHelper.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <istream>
#include <iterator>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <slang/parsing/ParserMetadata.h>
#include <slang/parsing/Token.h>
#include <slang/parsing/TokenKind.h>
#include <slang/syntax/AllSyntax.h>
#include <slang/syntax/SyntaxKind.h>
#include <slang/syntax/SyntaxTree.h>
#include <slang/syntax/SyntaxVisitor.h>
#include <slang/text/SourceManager.h>

using namespace slang;
using namespace slang::format;
using namespace slang::parsing;
using namespace slang::syntax;

namespace {

/// Detects whether any descendant is a BlockStatementSyntax.
struct BlockFinder : SyntaxVisitor<BlockFinder> {
    bool found = false;
    void handle(const BlockStatementSyntax& /*node*/) { found = true; }
};

bool containsBlock(const SyntaxNode& node) {
    BlockFinder finder;
    node.visit(finder);
    return finder.found;
}

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

/// Handles ingle-pass tree walk combining serialization, indentation, break
/// handling, and empty-line limiting. Structural changes are handled by the
/// SyntaxRewriter before this pass.
class FormatPrinter : public SyntaxVisitor<FormatPrinter> {
public:
    explicit FormatPrinter(const Style& style) : style(style) {
        if (!style.OneLineFormatOffRegex.empty()) {
            offRegex.emplace(style.OneLineFormatOffRegex);
        }
    }

    std::string print(const SyntaxTree& tree) {
        tree.root().visit(*this);

        // The EndOfFile token is not part of the visited subtree when tree.root()
        // returns a member node (e.g. ModuleDeclarationSyntax). Emit its
        // leading trivia, which carries the final newlines from the source.
        emitToken(tree.getMetadata().eofToken);
        return std::move(output);
    }

    // Called by the base-class visitDefault for every token child of an
    // unhandled node. NOLINTNEXTLINE(bugprone-derived-method-shadowing-base-method)
    void visitToken(Token tok) { emitToken(tok); }

    void handle(const ModuleDeclarationSyntax& module) {
        nextIsPrimary = true;
        module.header->visit(*this);

        depth++;
        for (auto* member : module.members) {
            nextIsPrimary = true;
            member->visit(*this);
        }

        depth--;
        nextIsPrimary = true;
        emitToken(module.endmodule);
        if (module.blockName != nullptr) {
            module.blockName->visit(*this);
        }
    }

    void handle(const AnsiPortListSyntax& p) {
        emitToken(p.openParen);

        depth++;
        visitSeparatedList(p.ports);

        depth--;
        nextIsPrimary = true;
        emitToken(p.closeParen);
    }

    void handle(const NonAnsiPortListSyntax& p) {
        emitToken(p.openParen);

        depth++;
        visitSeparatedList(p.ports);

        depth--;
        nextIsPrimary = true;
        emitToken(p.closeParen);
    }

    void handle(const ParameterPortListSyntax& p) {
        emitToken(p.hash);
        emitToken(p.openParen);

        depth++;
        visitSeparatedList(p.declarations);

        depth--;
        nextIsPrimary = true;
        emitToken(p.closeParen);
    }

    void handle(const BlockStatementSyntax& block) {
        if (block.label != nullptr) {
            block.label->visit(*this);
        }

        for (auto* attr : block.attributes) {
            attr->visit(*this);
        }

        emitToken(block.begin);
        if (block.blockName != nullptr) {
            block.blockName->visit(*this);
        }

        bool const breakAfterBegin = style.BreakAfterBegin && !block.items.empty() &&
                                     !hasLeadingNewline(block.items.front()->getFirstToken());
        bool const breakBeforeEnd = style.BreakBeforeEnd && !hasLeadingNewline(block.end);

        depth++;
        bool firstItem{true};
        for (auto* item : block.items) {
            if (firstItem && breakAfterBegin) {
                forceNewline();
            }

            firstItem = false;
            nextIsPrimary = true;
            item->visit(*this);
        }

        depth--;
        if (breakBeforeEnd) {
            forceNewline();
        }
        nextIsPrimary = true;
        emitToken(block.end);
        if (block.endBlockName != nullptr) {
            block.endBlockName->visit(*this);
        }
    }

    void handle(const ProceduralBlockSyntax& proc) {
        nextIsPrimary = true;
        for (auto* attr : proc.attributes) {
            attr->visit(*this);
        }

        emitToken(proc.keyword);

        auto brkStyle = BreakAfterBlockStyle::Never;
        if (isAlwaysBlockKind(proc.kind)) {
            brkStyle = style.BreakAfterAlways;
        }
        else if (proc.kind == SyntaxKind::InitialBlock) {
            brkStyle = style.BreakAfterInitial;
        }

        visitProceduralBody(*proc.statement, brkStyle);
    }

    void handle(const ConditionalStatementSyntax& stmt) {
        if (stmt.label != nullptr) {
            stmt.label->visit(*this);
        }

        for (auto* attr : stmt.attributes) {
            attr->visit(*this);
        }

        emitToken(stmt.uniqueOrPriority);
        emitToken(stmt.ifKeyword);
        emitToken(stmt.openParen);
        stmt.predicate->visit(*this);
        emitToken(stmt.closeParen);

        visitBody(*stmt.statement);

        if (stmt.elseClause != nullptr) {
            nextIsPrimary = true;
            emitToken(stmt.elseClause->elseKeyword);

            auto& clause = *stmt.elseClause->clause;
            if (clause.kind == SyntaxKind::ConditionalStatement) {
                clause.visit(*this);
            }
            else {
                visitBody(clause.as<StatementSyntax>());
            }
        }
    }

    void handle(const CaseStatementSyntax& caseStmt) {
        if (caseStmt.label != nullptr) {
            caseStmt.label->visit(*this);
        }

        for (auto* attr : caseStmt.attributes) {
            attr->visit(*this);
        }

        emitToken(caseStmt.uniqueOrPriority);
        emitToken(caseStmt.caseKeyword);
        emitToken(caseStmt.openParen);
        caseStmt.expr->visit(*this);
        emitToken(caseStmt.closeParen);
        emitToken(caseStmt.matchesOrInside);

        depth++;
        for (auto* item : caseStmt.items) {
            nextIsPrimary = true;
            item->visit(*this);
        }

        depth--;
        nextIsPrimary = true;
        emitToken(caseStmt.endcase);
    }

    void handle(const StandardCaseItemSyntax& item) {
        nextIsPrimary = true;

        // Emit expressions (elements and separators).
        for (const auto& elem : item.expressions.elems()) {
            if (elem.isNode()) {
                elem.node()->visit(*this);
            }
            else if (elem.isToken()) {
                emitToken(elem.token());
            }
        }

        emitToken(item.colon);
        visitCaseItemClause(*item.clause);
    }

    void handle(const DefaultCaseItemSyntax& item) {
        nextIsPrimary = true;
        emitToken(item.defaultKeyword);
        emitToken(item.colon);
        visitCaseItemClause(*item.clause);
    }

    void handle(const LoopStatementSyntax& loop) {
        if (loop.label != nullptr) {
            loop.label->visit(*this);
        }

        for (auto* attr : loop.attributes) {
            attr->visit(*this);
        }

        emitToken(loop.repeatOrWhile);
        emitToken(loop.openParen);
        loop.expr->visit(*this);
        emitToken(loop.closeParen);
        visitBody(*loop.statement);
    }

    void handle(const ForLoopStatementSyntax& loop) {
        if (loop.label != nullptr) {
            loop.label->visit(*this);
        }

        for (auto* attr : loop.attributes) {
            attr->visit(*this);
        }

        emitToken(loop.forKeyword);
        emitToken(loop.openParen);
        for (const auto& elem : loop.initializers.elems()) {
            if (elem.isNode()) {
                elem.node()->visit(*this);
            }
            else if (elem.isToken()) {
                emitToken(elem.token());
            }
        }

        emitToken(loop.semi1);
        if (loop.stopExpr != nullptr) {
            loop.stopExpr->visit(*this);
        }

        emitToken(loop.semi2);
        for (const auto& elem : loop.steps.elems()) {
            if (elem.isNode()) {
                elem.node()->visit(*this);
            }
            else if (elem.isToken()) {
                emitToken(elem.token());
            }
        }

        emitToken(loop.closeParen);
        visitBody(*loop.statement);
    }

    void handle(const ForeverStatementSyntax& loop) {
        if (loop.label != nullptr) {
            loop.label->visit(*this);
        }

        for (auto* attr : loop.attributes) {
            attr->visit(*this);
        }

        emitToken(loop.foreverKeyword);
        visitBody(*loop.statement);
    }

    void handle(const ForeachLoopStatementSyntax& loop) {
        if (loop.label != nullptr) {
            loop.label->visit(*this);
        }

        for (auto* attr : loop.attributes) {
            attr->visit(*this);
        }

        emitToken(loop.keyword);
        loop.loopList->visit(*this);
        visitBody(*loop.statement);
    }

    void handle(const DoWhileStatementSyntax& loop) {
        if (loop.label != nullptr) {
            loop.label->visit(*this);
        }

        for (auto* attr : loop.attributes) {
            attr->visit(*this);
        }

        emitToken(loop.doKeyword);
        visitBody(*loop.statement);
        emitToken(loop.whileKeyword);
        emitToken(loop.openParen);
        loop.expr->visit(*this);
        emitToken(loop.closeParen);
        emitToken(loop.semi);
    }

    void handle(const GenerateBlockSyntax& b) { visitScopedBlock(b, b.end, b.members); }

    void handle(const GenerateRegionSyntax& r) { visitScopedBlock(r, r.endgenerate, r.members); }

    void handle(const FunctionDeclarationSyntax& f) { visitScopedBlock(f, f.end, f.items); }

    void handle(const SpecifyBlockSyntax& s) { visitScopedBlock(s, s.endspecify, s.items); }

private:
    const Style& style;
    std::string output;
    unsigned depth = 0;
    bool atLineStart = true;
    bool nextIsPrimary = true;
    bool portItemNextIndent = false;
    bool formatEnabled = true;
    unsigned emptyLineCount = 0;
    std::optional<std::regex> offRegex;

    static bool hasLeadingNewline(Token tok) {
        if (!tok) {
            return false;
        }

        return std::ranges::any_of(tok.trivia(),
                                   [](const auto& t) { return t.kind == TriviaKind::EndOfLine; });
    }

    // Strip trailing spaces from output and emit a newline, setting atLineStart.
    void forceNewline() {
        while (!output.empty() && output.back() == ' ') {
            output.pop_back();
        }
        output += '\n';
        atLineStart = true;
    }

    void emitTrivia(const Trivia& t) {
        if (t.kind == TriviaKind::EndOfLine) {
            if (atLineStart) {
                // Blank line.
                emptyLineCount++;
                if (formatEnabled && emptyLineCount > style.MaxEmptyLinesToKeep) {
                    return;
                }

                output += '\n';
            }
            else {
                emptyLineCount = 0;
                output += '\n';
                atLineStart = true;
            }
            return;
        }

        if (t.kind == TriviaKind::Whitespace) {
            if (atLineStart && formatEnabled) {
                return; // leading whitespace replaced by computed indent
            }
            output += t.getRawText();
            return;
        }

        auto raw = t.getRawText();

        if (t.kind == TriviaKind::LineComment || t.kind == TriviaKind::BlockComment) {
            bool const isOff = formatEnabled && matchesPragma(raw, "slang-format off");
            bool const isOn = !formatEnabled && matchesPragma(raw, "slang-format on");

            // Indent while still in the current format state (before toggling).
            if (formatEnabled && atLineStart) {
                emitIndentRaw();
            }
            output += raw;
            atLineStart = false; // just emitted text on this line

            // Toggle after emit: off-pragma was indented (format was on);
            // on-pragma was verbatim.
            if (isOff) {
                formatEnabled = false;
            }
            else if (isOn) {
                formatEnabled = true;
            }
            return;
        }

        output += raw;
    }

    // Emit computed indentation directly into output; used for comment trivia
    // and raw text.
    void emitIndentRaw() {
        unsigned spaces = depth * style.IndentWidth;
        if (!nextIsPrimary) {
            spaces += style.ContinuationIndentWidth;
        }
        output.append(spaces, ' ');
        atLineStart = false;
    }

    void emitToken(Token tok) {
        if (!tok || tok.isMissing()) {
            return;
        }

        for (const auto& t : tok.trivia()) {
            emitTrivia(t);
        }

        auto raw = tok.rawText();
        if (formatEnabled && atLineStart && !raw.empty()) {
            if (portItemNextIndent) {
                output.append(style.ParameterPortListIndentWidth, ' ');
                portItemNextIndent = false;
            }
            else if (nextIsPrimary) {
                output.append(static_cast<std::size_t>(depth) * style.IndentWidth, ' ');
            }
            else {
                output.append((static_cast<std::size_t>(depth) * style.IndentWidth) +
                                  style.ContinuationIndentWidth,
                              ' ');
            }

            atLineStart = false;
        }
        else if (atLineStart) {
            atLineStart = false;
        }

        output += raw;
        nextIsPrimary = false;
        emptyLineCount = 0;
    }

    // Emit all elements and separators of a SeparatedSyntaxList.
    template<typename T>
    void visitSeparatedList(const SeparatedSyntaxList<T>& list) {
        for (auto& elem : list.elems()) {
            if (elem.isNode()) {
                portItemNextIndent = true;
                nextIsPrimary = true;
                elem.node()->visit(*this);
                portItemNextIndent = false;
            }
            else if (elem.isToken()) {
                emitToken(elem.token());
            }
        }
    }

    // Visit a statement body; bare statements get an extra indentation level.
    void visitBody(const StatementSyntax& body) {
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

    // Handle a procedural block body: unwrap timing control, apply forced
    // break, then visitBody.
    void visitProceduralBody(const StatementSyntax& stmt, BreakAfterBlockStyle brkStyle) {
        const auto* body = &stmt;
        if (stmt.kind == SyntaxKind::TimingControlStatement) {
            const auto& tcs = stmt.as<TimingControlStatementSyntax>();
            if (tcs.label != nullptr) {
                tcs.label->visit(*this);
            }

            for (auto* attr : tcs.attributes) {
                attr->visit(*this);
            }

            tcs.timingControl->visit(*this);
            body = tcs.statement;
        }

        bool const forceBreak = shouldBreakAfterProcedural(brkStyle, *body);
        if (forceBreak && !hasLeadingNewline(body->getFirstToken())) {
            forceNewline();
            nextIsPrimary = true;
        }

        visitBody(*body);
    }

    static bool shouldBreakAfterProcedural(BreakAfterBlockStyle brkStyle,
                                           const StatementSyntax& body) {
        if (brkStyle == BreakAfterBlockStyle::Never) {
            return false;
        }

        if (hasLeadingNewline(body.getFirstToken())) {
            return false;
        }

        if (BlockStatementSyntax::isKind(body.kind)) {
            const auto& block = body.as<BlockStatementSyntax>();
            Token const checkTok = block.items.empty() ? block.end
                                                       : block.items.front()->getFirstToken();
            if (hasLeadingNewline(checkTok)) {
                return false;
            }
        }

        if (brkStyle == BreakAfterBlockStyle::Always) {
            return true;
        }

        if (BlockStatementSyntax::isKind(body.kind)) {
            const auto& block = body.as<BlockStatementSyntax>();
            if (block.items.size() <= 1) {
                return false;
            }

            if (hasLeadingNewline(block.end)) {
                return true;
            }

            return std::ranges::any_of(block.items, [](const auto* item) {
                return item->getFirstToken() && hasLeadingNewline(item->getFirstToken());
            });
        }

        return containsBlock(body);
    }

    void visitCaseItemClause(const SyntaxNode& clause) {
        bool const nextLine = clause.getFirstToken() && hasLeadingNewline(clause.getFirstToken());
        if (!StatementSyntax::isKind(clause.kind) || clause.kind == SyntaxKind::EmptyStatement) {
            clause.visit(*this);
        }
        else if (!nextLine || style.IndentCaseItem) {
            visitBody(clause.as<StatementSyntax>());
        }
        else {
            nextIsPrimary = true;
            clause.visit(*this);
        }
    }

    template<typename NodeT, typename ListT>
    void visitScopedBlock(const NodeT& node, Token endTok, const ListT& items) {
        nextIsPrimary = true;
        bool depthBumped{false};
        auto isItem = [&items](const SyntaxNode* child) {
            return std::ranges::any_of(items, [child](const auto* item) { return item == child; });
        };
        const auto count = node.getChildCount();
        for (size_t i = 0; i < count; i++) {
            if (auto tok = node.childToken(i)) {
                if (tok.location() == endTok.location()) {
                    if (depthBumped) {
                        depth--;
                        depthBumped = false;
                    }

                    nextIsPrimary = true;
                }

                emitToken(tok);
            }
            else if (auto* childNode = node.childNode(i)) {
                if (isItem(childNode)) {
                    if (!depthBumped) {
                        depth++;
                        depthBumped = true;
                    }

                    nextIsPrimary = true;
                }

                childNode->visit(*this);
            }
        }

        if (depthBumped) {
            depth--;
        }
    }
};

enum class LineKind { Declaration, Comment, Empty, PortListBoundary, Other };

struct LineInfo {
    LineKind kind;
    size_t indent;
    size_t typeWidth; ///< Distance from indent to identifier start.
    size_t identPos;  ///< Absolute position of identifier in the line.
    size_t equalsPos; ///< Position of '=' in declaration, or npos if absent.
    size_t dimPos;    ///< Position of first '[' after type keyword, or npos if absent.
};

constexpr std::array TypeKeywords{
    std::string_view{"bit"},      std::string_view{"byte"},      std::string_view{"int"},
    std::string_view{"integer"},  std::string_view{"logic"},     std::string_view{"longint"},
    std::string_view{"real"},     std::string_view{"realtime"},  std::string_view{"reg"},
    std::string_view{"shortint"}, std::string_view{"shortreal"}, std::string_view{"string"},
    std::string_view{"time"},     std::string_view{"wire"},
};

bool isTypeKeyword(std::string_view word) {
    return std::ranges::any_of(TypeKeywords, [&](auto kw) { return kw == word; });
}

bool isDirectionKeyword(std::string_view word) {
    return word == "inout" || word == "input" || word == "output" || word == "ref";
}

bool isParameterKeyword(std::string_view word) {
    return word == "localparam" || word == "parameter";
}

bool isWordChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
}

std::string_view extractWord(std::string_view line, size_t pos) {
    auto start = pos;
    while (pos < line.size() && isWordChar(line[pos])) {
        pos++;
    }
    return line.substr(start, pos - start);
}

size_t skipSpaces(std::string_view line, size_t pos) {
    while (pos < line.size() && line[pos] == ' ') {
        pos++;
    }
    return pos;
}

size_t skipDimensions(std::string_view line, size_t pos) {
    while (pos < line.size()) {
        pos = skipSpaces(line, pos);
        if (pos >= line.size() || line[pos] != '[') {
            break;
        }

        int depth = 1;
        pos++;
        while (pos < line.size() && depth > 0) {
            if (line[pos] == '[') {
                depth++;
            }
            else if (line[pos] == ']') {
                depth--;
            }
            pos++;
        }
    }
    return pos;
}

size_t skipSignedness(std::string_view line, size_t pos) {
    pos = skipSpaces(line, pos);
    auto word = extractWord(line, pos);
    if (word == "signed" || word == "unsigned") {
        pos += word.size();
    }
    return pos;
}

size_t skipTypeQualifiers(std::string_view line, size_t pos) {
    pos = skipSignedness(line, pos);
    return skipDimensions(line, pos);
}

size_t skipDirectionType(std::string_view line, size_t pos) {
    auto nextWord = extractWord(line, pos);
    if (isTypeKeyword(nextWord)) {
        return skipTypeQualifiers(line, pos + nextWord.size());
    }

    if (nextWord == "signed" || nextWord == "unsigned") {
        return skipDimensions(line, pos + nextWord.size());
    }

    if (pos < line.size() && line[pos] == '[') {
        return skipDimensions(line, pos);
    }
    return pos;
}

constexpr auto npos = std::string_view::npos;

size_t findEquals(std::string_view line, size_t pos) {
    while (pos < line.size()) {
        if (line[pos] == '=') {
            if (pos + 1 < line.size() && line[pos + 1] == '=') {
                pos += 2;
                continue;
            }
            if (pos > 0 && (line[pos - 1] == '!' || line[pos - 1] == '<' || line[pos - 1] == '>')) {
                pos++;
                continue;
            }
            return pos;
        }
        pos++;
    }
    return npos;
}

size_t findDimPos(std::string_view line, size_t pos, std::string_view firstWord) {
    if (isDirectionKeyword(firstWord) || isParameterKeyword(firstWord)) {
        auto nextWord = extractWord(line, pos);
        if (isTypeKeyword(nextWord)) {
            auto afterType = skipSignedness(line, pos + nextWord.size());
            auto afterSpaces = skipSpaces(line, afterType);
            if (afterSpaces < line.size() && line[afterSpaces] == '[') {
                return afterSpaces;
            }
        }
        else if (nextWord == "signed" || nextWord == "unsigned") {
            auto afterSign = skipSpaces(line, pos + nextWord.size());
            if (afterSign < line.size() && line[afterSign] == '[') {
                return afterSign;
            }
        }
        else if (pos < line.size() && line[pos] == '[') {
            return pos;
        }
    }
    else if (isTypeKeyword(firstWord)) {
        auto afterType = skipSignedness(line, pos + firstWord.size());
        auto afterSpaces = skipSpaces(line, afterType);
        if (afterSpaces < line.size() && line[afterSpaces] == '[') {
            return afterSpaces;
        }
    }
    return npos;
}

LineInfo classifyLine(std::string_view line, bool formatOff) {
    if (line.empty() || line.find_first_not_of(' ') == std::string_view::npos) {
        return {.kind = LineKind::Empty,
                .indent = 0,
                .typeWidth = 0,
                .identPos = 0,
                .equalsPos = npos,
                .dimPos = npos};
    }

    auto indentEnd = line.find_first_not_of(' ');
    auto content = line.substr(indentEnd);

    if (content.starts_with("//") || content.starts_with("/*")) {
        return {.kind = LineKind::Comment,
                .indent = indentEnd,
                .typeWidth = 0,
                .identPos = 0,
                .equalsPos = npos,
                .dimPos = npos};
    }

    if (content == ") (" || content == ");" || content == ")" || content == "#(") {
        return {.kind = LineKind::PortListBoundary,
                .indent = indentEnd,
                .typeWidth = 0,
                .identPos = 0,
                .equalsPos = npos,
                .dimPos = npos};
    }

    if (formatOff) {
        return {.kind = LineKind::Other,
                .indent = indentEnd,
                .typeWidth = 0,
                .identPos = 0,
                .equalsPos = npos,
                .dimPos = npos};
    }

    auto firstWord = extractWord(content, 0);
    if (firstWord.empty()) {
        return {.kind = LineKind::Other,
                .indent = indentEnd,
                .typeWidth = 0,
                .identPos = 0,
                .equalsPos = npos,
                .dimPos = npos};
    }

    size_t pos = indentEnd;
    size_t dimPos = npos;
    if (isDirectionKeyword(firstWord) || isParameterKeyword(firstWord)) {
        pos = skipSpaces(line, pos + firstWord.size());
        dimPos = findDimPos(line, pos, firstWord);
        pos = skipDirectionType(line, pos);
    }
    else if (isTypeKeyword(firstWord)) {
        dimPos = findDimPos(line, indentEnd, firstWord);
        pos = skipTypeQualifiers(line, pos + firstWord.size());
    }
    else {
        return {.kind = LineKind::Other,
                .indent = indentEnd,
                .typeWidth = 0,
                .identPos = 0,
                .equalsPos = npos,
                .dimPos = npos};
    }

    pos = skipSpaces(line, pos);
    if (pos >= line.size() ||
        (std::isalpha(static_cast<unsigned char>(line[pos])) == 0 && line[pos] != '_')) {
        return {.kind = LineKind::Other,
                .indent = indentEnd,
                .typeWidth = 0,
                .identPos = 0,
                .equalsPos = npos,
                .dimPos = npos};
    }

    auto eqPos = findEquals(line, pos);
    return {.kind = LineKind::Declaration,
            .indent = indentEnd,
            .typeWidth = pos - indentEnd,
            .identPos = pos,
            .equalsPos = eqPos,
            .dimPos = dimPos};
}

// NOLINTBEGIN(bugprone-easily-swappable-parameters)
void alignGroup(std::string& result, const std::vector<std::string_view>& lines,
                const std::vector<LineInfo>& infos, size_t groupStart, size_t groupEnd) {
    // NOLINTEND(bugprone-easily-swappable-parameters)
    size_t declCount = 0;
    size_t maxTypeWidth = 0;
    for (size_t i = groupStart; i < groupEnd; i++) {
        if (infos[i].kind == LineKind::Declaration) {
            declCount++;
            maxTypeWidth = std::max(maxTypeWidth, infos[i].typeWidth);
        }
    }

    for (size_t i = groupStart; i < groupEnd; i++) {
        if (infos[i].kind == LineKind::Declaration && declCount >= 2) {
            auto line = lines[i];
            auto indent = infos[i].indent;
            auto identPos = infos[i].identPos;
            auto targetPos = indent + maxTypeWidth;

            size_t typeTextEnd = identPos;
            while (typeTextEnd > indent && line[typeTextEnd - 1] == ' ') {
                typeTextEnd--;
            }

            result.append(line.substr(0, typeTextEnd));
            result.append(targetPos - (typeTextEnd - indent) - indent, ' ');
            result.append(line.substr(identPos));
        }
        else {
            result.append(lines[i]);
        }
        result += '\n';
    }
}

struct AlignState {
    size_t groupStart = 0;
    bool inGroup = false;
    std::optional<size_t> groupIndent;
};

bool shouldBreakGroup(const LineInfo& info, const AlignState& state, bool acrossEmpty,
                      bool acrossComments, bool acrossIndent) {
    if (info.kind == LineKind::Declaration) {
        return state.inGroup && !acrossIndent && state.groupIndent &&
               *state.groupIndent != info.indent;
    }

    if (info.kind == LineKind::Empty) {
        return state.inGroup && !acrossEmpty;
    }

    if (info.kind == LineKind::Comment) {
        return state.inGroup && !acrossComments;
    }

    if (info.kind == LineKind::PortListBoundary) {
        return state.inGroup && !acrossIndent;
    }

    return true;
}

// NOLINTBEGIN(bugprone-easily-swappable-parameters)
void alignGroupDimensions(std::string& result, const std::vector<std::string_view>& lines,
                          const std::vector<LineInfo>& infos, size_t groupStart, size_t groupEnd) {
    // NOLINTEND(bugprone-easily-swappable-parameters)
    size_t dimCount = 0;
    size_t maxDimCol = 0;
    for (size_t i = groupStart; i < groupEnd; i++) {
        if (infos[i].kind == LineKind::Declaration && infos[i].dimPos != npos) {
            dimCount++;
            maxDimCol = std::max(maxDimCol, infos[i].dimPos);
        }
    }

    for (size_t i = groupStart; i < groupEnd; i++) {
        if (infos[i].kind == LineKind::Declaration && infos[i].dimPos != npos && dimCount >= 2) {
            auto line = lines[i];
            auto dp = infos[i].dimPos;

            size_t preDimEnd = dp;
            while (preDimEnd > 0 && line[preDimEnd - 1] == ' ') {
                preDimEnd--;
            }

            result.append(line.substr(0, preDimEnd));
            result.append(maxDimCol - preDimEnd, ' ');
            result.append(line.substr(dp));
        }
        else {
            result.append(lines[i]);
        }
        result += '\n';
    }
}

bool shouldBreakDimGroup(const LineInfo& info, const AlignState& state, bool acrossEmpty,
                         bool acrossComments, bool acrossIndent) {
    if (info.kind == LineKind::Declaration && info.dimPos == npos) {
        return state.inGroup;
    }
    return shouldBreakGroup(info, state, acrossEmpty, acrossComments, acrossIndent);
}

std::string applyAlignConsecutivePackedDimensions(const std::string& output,
                                                  AlignConsecutiveStyle alignStyle) {
    if (alignStyle == AlignConsecutiveStyle::None) {
        return output;
    }

    bool const acrossEmpty = alignStyle == AlignConsecutiveStyle::AcrossEmptyLines ||
                             alignStyle == AlignConsecutiveStyle::AcrossEmptyLinesAndComments ||
                             alignStyle == AlignConsecutiveStyle::AcrossParameterPortList;
    bool const acrossComments = alignStyle == AlignConsecutiveStyle::AcrossComments ||
                                alignStyle == AlignConsecutiveStyle::AcrossEmptyLinesAndComments ||
                                alignStyle == AlignConsecutiveStyle::AcrossParameterPortList;
    bool const acrossIndent = alignStyle == AlignConsecutiveStyle::AcrossParameterPortList;

    std::vector<std::string_view> lines;
    std::string_view remaining{output};
    while (!remaining.empty()) {
        auto nl = remaining.find('\n');
        if (nl == std::string_view::npos) {
            lines.push_back(remaining);
            remaining = {};
        }
        else {
            lines.push_back(remaining.substr(0, nl));
            remaining.remove_prefix(nl + 1);
        }
    }

    std::vector<LineInfo> infos;
    infos.reserve(lines.size());
    bool formatOff = false;
    for (auto& line : lines) {
        if (line.find("slang-format off") != std::string_view::npos) {
            formatOff = true;
        }
        else if (line.find("slang-format on") != std::string_view::npos) {
            formatOff = false;
        }

        infos.push_back(classifyLine(line, formatOff));
    }

    std::string result;
    result.reserve(output.size());
    AlignState state;

    for (size_t i = 0; i < lines.size(); i++) {
        auto& info = infos[i];
        bool const hasDim = info.kind == LineKind::Declaration && info.dimPos != npos;
        bool const breakGroup = shouldBreakDimGroup(info, state, acrossEmpty, acrossComments,
                                                    acrossIndent);

        if (breakGroup && state.inGroup) {
            alignGroupDimensions(result, lines, infos, state.groupStart, i);
            state.inGroup = false;
            state.groupIndent.reset();
        }

        if (hasDim && !state.inGroup) {
            state.groupStart = i;
            state.inGroup = true;
            state.groupIndent = info.indent;
        }
        else if (!state.inGroup) {
            result.append(lines[i]);
            result += '\n';
        }
    }

    if (state.inGroup) {
        alignGroupDimensions(result, lines, infos, state.groupStart, lines.size());
    }

    if (!result.empty() && result.back() == '\n' && (output.empty() || output.back() != '\n')) {
        result.pop_back();
    }

    return result;
}

std::string applyAlignConsecutiveDeclarations(const std::string& output,
                                              AlignConsecutiveStyle alignStyle) {
    if (alignStyle == AlignConsecutiveStyle::None) {
        return output;
    }

    bool const acrossEmpty = alignStyle == AlignConsecutiveStyle::AcrossEmptyLines ||
                             alignStyle == AlignConsecutiveStyle::AcrossEmptyLinesAndComments ||
                             alignStyle == AlignConsecutiveStyle::AcrossParameterPortList;
    bool const acrossComments = alignStyle == AlignConsecutiveStyle::AcrossComments ||
                                alignStyle == AlignConsecutiveStyle::AcrossEmptyLinesAndComments ||
                                alignStyle == AlignConsecutiveStyle::AcrossParameterPortList;
    bool const acrossIndent = alignStyle == AlignConsecutiveStyle::AcrossParameterPortList;

    std::vector<std::string_view> lines;
    std::string_view remaining{output};
    while (!remaining.empty()) {
        auto nl = remaining.find('\n');
        if (nl == std::string_view::npos) {
            lines.push_back(remaining);
            remaining = {};
        }
        else {
            lines.push_back(remaining.substr(0, nl));
            remaining.remove_prefix(nl + 1);
        }
    }

    std::vector<LineInfo> infos;
    infos.reserve(lines.size());
    bool formatOff = false;
    for (auto& line : lines) {
        if (line.find("slang-format off") != std::string_view::npos) {
            formatOff = true;
        }
        else if (line.find("slang-format on") != std::string_view::npos) {
            formatOff = false;
        }

        infos.push_back(classifyLine(line, formatOff));
    }

    std::string result;
    result.reserve(output.size());
    AlignState state;

    for (size_t i = 0; i < lines.size(); i++) {
        auto& info = infos[i];
        bool const breakGroup = shouldBreakGroup(info, state, acrossEmpty, acrossComments,
                                                 acrossIndent);

        if (breakGroup && state.inGroup) {
            alignGroup(result, lines, infos, state.groupStart, i);
            state.inGroup = false;
            state.groupIndent.reset();
        }

        if (info.kind == LineKind::Declaration && !state.inGroup) {
            state.groupStart = i;
            state.inGroup = true;
            state.groupIndent = info.indent;
        }
        else if (!state.inGroup) {
            result.append(lines[i]);
            result += '\n';
        }
    }

    if (state.inGroup) {
        alignGroup(result, lines, infos, state.groupStart, lines.size());
    }

    if (!result.empty() && result.back() == '\n' && (output.empty() || output.back() != '\n')) {
        result.pop_back();
    }

    return result;
}

// NOLINTBEGIN(bugprone-easily-swappable-parameters)
void alignGroupEquals(std::string& result, const std::vector<std::string_view>& lines,
                      const std::vector<LineInfo>& infos, size_t groupStart, size_t groupEnd) {
    // NOLINTEND(bugprone-easily-swappable-parameters)
    size_t equalsCount = 0;
    size_t maxEqualsCol = 0;
    for (size_t i = groupStart; i < groupEnd; i++) {
        if (infos[i].kind == LineKind::Declaration && infos[i].equalsPos != npos) {
            equalsCount++;
            maxEqualsCol = std::max(maxEqualsCol, infos[i].equalsPos);
        }
    }

    for (size_t i = groupStart; i < groupEnd; i++) {
        if (infos[i].kind == LineKind::Declaration && infos[i].equalsPos != npos &&
            equalsCount >= 2) {
            auto line = lines[i];
            auto eqPos = infos[i].equalsPos;

            size_t preEqualsEnd = eqPos;
            while (preEqualsEnd > 0 && line[preEqualsEnd - 1] == ' ') {
                preEqualsEnd--;
            }

            result.append(line.substr(0, preEqualsEnd));
            result.append(maxEqualsCol - preEqualsEnd, ' ');
            result.append(line.substr(eqPos));
        }
        else {
            result.append(lines[i]);
        }
        result += '\n';
    }
}

std::string applyAlignConsecutiveAssignments(const std::string& output,
                                             AlignConsecutiveStyle alignStyle) {
    if (alignStyle == AlignConsecutiveStyle::None) {
        return output;
    }

    bool const acrossEmpty = alignStyle == AlignConsecutiveStyle::AcrossEmptyLines ||
                             alignStyle == AlignConsecutiveStyle::AcrossEmptyLinesAndComments ||
                             alignStyle == AlignConsecutiveStyle::AcrossParameterPortList;
    bool const acrossComments = alignStyle == AlignConsecutiveStyle::AcrossComments ||
                                alignStyle == AlignConsecutiveStyle::AcrossEmptyLinesAndComments ||
                                alignStyle == AlignConsecutiveStyle::AcrossParameterPortList;
    bool const acrossIndent = alignStyle == AlignConsecutiveStyle::AcrossParameterPortList;

    std::vector<std::string_view> lines;
    std::string_view remaining{output};
    while (!remaining.empty()) {
        auto nl = remaining.find('\n');
        if (nl == std::string_view::npos) {
            lines.push_back(remaining);
            remaining = {};
        }
        else {
            lines.push_back(remaining.substr(0, nl));
            remaining.remove_prefix(nl + 1);
        }
    }

    std::vector<LineInfo> infos;
    infos.reserve(lines.size());
    bool formatOff = false;
    for (auto& line : lines) {
        if (line.find("slang-format off") != std::string_view::npos) {
            formatOff = true;
        }
        else if (line.find("slang-format on") != std::string_view::npos) {
            formatOff = false;
        }

        infos.push_back(classifyLine(line, formatOff));
    }

    std::string result;
    result.reserve(output.size());
    AlignState state;

    for (size_t i = 0; i < lines.size(); i++) {
        auto& info = infos[i];
        bool const breakGroup = shouldBreakGroup(info, state, acrossEmpty, acrossComments,
                                                 acrossIndent);

        if (breakGroup && state.inGroup) {
            alignGroupEquals(result, lines, infos, state.groupStart, i);
            state.inGroup = false;
            state.groupIndent.reset();
        }

        if (info.kind == LineKind::Declaration && !state.inGroup) {
            state.groupStart = i;
            state.inGroup = true;
            state.groupIndent = info.indent;
        }
        else if (!state.inGroup) {
            result.append(lines[i]);
            result += '\n';
        }
    }

    if (state.inGroup) {
        alignGroupEquals(result, lines, infos, state.groupStart, lines.size());
    }

    if (!result.empty() && result.back() == '\n' && (output.empty() || output.back() != '\n')) {
        result.pop_back();
    }

    return result;
}

std::string applyOneLineFormatOff(const std::string& output, const std::regex& re) {
    std::string result;
    result.reserve(output.size());
    size_t pos{0};
    while (pos < output.size()) {
        size_t const lineEnd = output.find('\n', pos);
        size_t const nextPos = (lineEnd == std::string::npos) ? output.size() : lineEnd + 1;
        std::string_view const line = std::string_view(output).substr(pos, nextPos - pos);

        // Strip leading whitespace to get the content for regex matching.
        auto trimmed = line;
        while (!trimmed.empty() && (trimmed.front() == ' ' || trimmed.front() == '\t')) {
            trimmed.remove_prefix(1);
        }

        if (!trimmed.empty() && std::regex_search(std::string(trimmed), re)) {
            result.append(trimmed);
        }
        else {
            result.append(line);
        }

        pos = nextPos;
    }
    return result;
}

} // namespace

namespace slang::format {

std::string reformat(std::string_view text, const Style& style) {
    SourceManager sm;
    auto tree = SyntaxTree::fromText(text, sm, "source");
    tree = applyBeginEndInsertion(tree, style);
    FormatPrinter printer(style);

    auto result = printer.print(*tree);
    if (style.AlignConsecutivePackedDimensions != AlignConsecutiveStyle::None) {
        result = applyAlignConsecutivePackedDimensions(result,
                                                       style.AlignConsecutivePackedDimensions);
    }

    if (style.AlignConsecutiveDeclarations != AlignConsecutiveStyle::None) {
        result = applyAlignConsecutiveDeclarations(result, style.AlignConsecutiveDeclarations);
    }

    if (style.AlignConsecutiveAssignments != AlignConsecutiveStyle::None) {
        result = applyAlignConsecutiveAssignments(result, style.AlignConsecutiveAssignments);
    }

    if (!style.OneLineFormatOffRegex.empty()) {
        std::regex const re(style.OneLineFormatOffRegex);
        result = applyOneLineFormatOff(result, re);
    }
    return result;
}

std::string reformat(std::istream& stream, const Style& style) {
    std::string const text{std::istreambuf_iterator<char>(stream), {}};
    return reformat(text, style);
}

} // namespace slang::format
