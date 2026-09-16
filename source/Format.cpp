// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Align.h"
#include "Format.h"
#include "Rewriter.h"
#include "Style.h"
#include "SyntaxHelper.h"

#include <algorithm>
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
#include <slang/syntax/SyntaxNode.h>
#include <slang/syntax/SyntaxTree.h>
#include <slang/syntax/SyntaxVisitor.h>
#include <slang/text/CharInfo.h>
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

bool isIdentChar(char c) {
    return isValidCIdChar(c) || c == '$';
}

bool isEventControl(const TimingControlSyntax& tc) {
    return tc.kind == SyntaxKind::EventControl ||
           tc.kind == SyntaxKind::EventControlWithExpression ||
           tc.kind == SyntaxKind::ImplicitEventControl;
}

bool needsSeparator(char last, char next) {
    return isIdentChar(last) && isIdentChar(next);
}

size_t flatWidth(const SyntaxNode& node) {
    size_t width = 0;
    bool first = true;
    for (auto it = node.tokens_begin(); it != node.tokens_end(); ++it) {
        auto tok = *it;
        for (const auto& t : tok.trivia()) {
            if (t.kind == TriviaKind::LineComment || t.kind == TriviaKind::BlockComment ||
                t.kind == TriviaKind::EndOfLine) {
                return 0;
            }
        }

        auto raw = tok.rawText();
        if (raw.empty()) {
            continue;
        }

        if (!first) {
            width++;
        }
        width += raw.size();
        first = false;
    }
    return width;
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

    struct Result {
        std::string output;
        std::vector<LineMetadata> lineMetadata;
    };

    Result print(const SyntaxTree& tree) {
        tree.root().visit(*this);

        // The EndOfFile token is not part of the visited subtree when tree.root()
        // returns a member node (e.g. ModuleDeclarationSyntax). Emit its
        // leading trivia, which carries the final newlines from the source.
        emitToken(tree.getMetadata().eofToken);
        return {.output = std::move(output), .lineMetadata = std::move(lineMetadata)};
    }

    // Called by the base-class visitDefault for every token child of an unhandled node.
    void visitToken(Token tok) { emitToken(tok); }

    void handle(const ModuleDeclarationSyntax& module) {
        nextIsPrimary = true;
        module.header->visit(*this);

        depth++;
        for (auto* member : module.members) {
            if (style.OneStatementPerLine && formatEnabled &&
                !hasLeadingNewline(member->getFirstToken())) {
                forceNewline();
            }

            nextIsPrimary = true;
            member->visit(*this);
        }

        nextIsPrimary = true;
        emitEndTokenTrivia(module.endmodule);

        depth--;
        nextIsPrimary = true;
        emitToken(module.endmodule);
        if (module.blockName != nullptr) {
            module.blockName->visit(*this);
        }
    }

    void handle(const AnsiPortListSyntax& p) {
        if (formatEnabled && !atLineStart && !output.empty() && !hasLeadingNewline(p.openParen)) {
            if (style.SpaceBeforeParens.PortList && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceBeforeParens.PortList) {
                stripTrailingSpaces();
            }
        }
        emitToken(p.openParen);

        depth++;
        visitSeparatedList(p.ports);

        portItemNextIndent = true;
        nextIsPrimary = true;
        emitEndTokenTrivia(p.closeParen);
        portItemNextIndent = false;

        depth--;
        nextIsPrimary = true;
        nextLineKind = LineMetadata::Kind::PortListBoundary;
        emitToken(p.closeParen);
    }

    void handle(const BinaryExpressionSyntax& expr) {
        expr.left->visit(*this);
        beforeOperator = formatEnabled;
        emitToken(expr.operatorToken);

        for (auto* attr : expr.attributes) {
            attr->visit(*this);
        }

        expr.right->visit(*this);
    }

    void handle(const NonAnsiPortListSyntax& p) {
        if (formatEnabled && !atLineStart && !output.empty() && !hasLeadingNewline(p.openParen)) {
            if (style.SpaceBeforeParens.PortList && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceBeforeParens.PortList) {
                stripTrailingSpaces();
            }
        }
        emitToken(p.openParen);

        depth++;
        visitSeparatedList(p.ports);

        portItemNextIndent = true;
        nextIsPrimary = true;
        emitEndTokenTrivia(p.closeParen);
        portItemNextIndent = false;

        depth--;
        nextIsPrimary = true;
        nextLineKind = LineMetadata::Kind::PortListBoundary;
        emitToken(p.closeParen);
    }

    void handle(const ParameterPortListSyntax& p) {
        if (formatEnabled && !atLineStart && !output.empty() && !hasLeadingNewline(p.hash)) {
            if (style.SpaceBeforeParens.ParameterList && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceBeforeParens.ParameterList) {
                stripTrailingSpaces();
            }
        }
        emitToken(p.hash);
        emitToken(p.openParen);

        depth++;
        visitSeparatedList(p.declarations);

        portItemNextIndent = true;
        nextIsPrimary = true;
        emitEndTokenTrivia(p.closeParen);
        portItemNextIndent = false;

        depth--;
        nextIsPrimary = true;
        nextLineKind = LineMetadata::Kind::PortListBoundary;
        emitToken(p.closeParen);
    }

    void handle(const WildcardPortListSyntax& p) {
        if (formatEnabled && !atLineStart && !output.empty() && !hasLeadingNewline(p.openParen)) {
            if (style.SpaceBeforeParens.PortList && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceBeforeParens.PortList) {
                stripTrailingSpaces();
            }
        }
        emitToken(p.openParen);
        emitToken(p.dot);
        emitToken(p.star);
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
            bool const needsBreak = (firstItem && breakAfterBegin) ||
                                    (!firstItem && style.OneStatementPerLine && formatEnabled &&
                                     !hasLeadingNewline(item->getFirstToken()));

            if (needsBreak) {
                forceNewline();
            }

            firstItem = false;
            nextIsPrimary = true;
            item->visit(*this);
        }

        nextIsPrimary = true;
        emitEndTokenTrivia(block.end);

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
        if (isAlwaysBlockKind(proc.kind) &&
            shouldBreakBeforeProcedural(style.BreakBeforeAlways, *proc.statement,
                                        style.OneStatementPerLine)) {
            insertBlankLineBefore(proc.getFirstToken());
        }

        if (isInitialBlockKind(proc.kind) &&
            shouldBreakBeforeProcedural(style.BreakBeforeInitial, *proc.statement,
                                        style.OneStatementPerLine)) {
            insertBlankLineBefore(proc.getFirstToken());
        }

        if (proc.kind == SyntaxKind::AlwaysBlock &&
            proc.statement->kind == SyntaxKind::TimingControlStatement) {
            nextLineKind = LineMetadata::Kind::TimingControl;
        }

        nextIsPrimary = true;
        for (auto* attr : proc.attributes) {
            attr->visit(*this);
        }

        emitToken(proc.keyword);

        if (isAlwaysBlockKind(proc.kind) && formatEnabled && !atLineStart && !output.empty() &&
            !hasLeadingNewline(proc.statement->getFirstToken())) {
            if (style.SpaceAfterAlways && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceAfterAlways) {
                stripTrailingSpaces();
            }
        }

        auto brkStyle = BlockBreakStyle::Never;
        if (isAlwaysBlockKind(proc.kind)) {
            brkStyle = style.BreakAfterAlways;
        }
        else if (proc.kind == SyntaxKind::InitialBlock) {
            brkStyle = style.BreakAfterInitial;
        }

        visitProceduralBody(*proc.statement, brkStyle);
    }

    void handle(const ConditionalExpressionSyntax& expr) {
        expr.predicate->visit(*this);
        beforeOperator = formatEnabled;
        emitToken(expr.question);

        for (auto* attr : expr.attributes) {
            attr->visit(*this);
        }

        expr.left->visit(*this);
        beforeOperator = formatEnabled;
        emitToken(expr.colon);
        expr.right->visit(*this);
    }

    void handle(const EqualsTypeClauseSyntax& clause) {
        beforeOperator = formatEnabled;
        emitToken(clause.equals);
        clause.type->visit(*this);
    }

    void handle(const EqualsValueClauseSyntax& clause) {
        beforeOperator = formatEnabled;
        emitToken(clause.equals);
        clause.expr->visit(*this);
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

        if (formatEnabled && !atLineStart && !output.empty() &&
            !hasLeadingNewline(stmt.openParen)) {
            if (style.SpaceBeforeParens.ControlStatements && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceBeforeParens.ControlStatements) {
                stripTrailingSpaces();
            }
        }

        emitToken(stmt.openParen);
        stmt.predicate->visit(*this);
        emitToken(stmt.closeParen);

        if (formatEnabled && !atLineStart && !output.empty() &&
            !hasLeadingNewline(stmt.statement->getFirstToken())) {
            if (style.SpaceAfterParens.ControlStatements && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceAfterParens.ControlStatements) {
                stripTrailingSpaces();
            }
        }

        if (shouldBreakConditionalBody(*stmt.statement)) {
            forceStatementBreak(stmt.statement->getFirstToken());
        }

        visitBody(*stmt.statement);

        if (stmt.elseClause != nullptr) {
            if (!BlockStatementSyntax::isKind(stmt.statement->kind)) {
                forceStatementBreak(stmt.elseClause->elseKeyword);
            }

            nextIsPrimary = true;
            emitToken(stmt.elseClause->elseKeyword);

            auto& clause = *stmt.elseClause->clause;
            if (clause.kind == SyntaxKind::ConditionalStatement) {
                clause.visit(*this);
            }
            else {
                if (shouldBreakConditionalBody(clause)) {
                    forceStatementBreak(clause.getFirstToken());
                }

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

        if (formatEnabled && !atLineStart && !output.empty() &&
            !hasLeadingNewline(caseStmt.openParen)) {
            if (style.SpaceBeforeParens.ControlStatements && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceBeforeParens.ControlStatements) {
                stripTrailingSpaces();
            }
        }

        emitToken(caseStmt.openParen);
        caseStmt.expr->visit(*this);
        emitToken(caseStmt.closeParen);

        if (caseStmt.matchesOrInside && !caseStmt.matchesOrInside.isMissing()) {
            if (formatEnabled && !atLineStart && !output.empty() &&
                !hasLeadingNewline(caseStmt.matchesOrInside)) {
                if (style.SpaceAfterParens.ControlStatements && output.back() != ' ') {
                    output += ' ';
                }
                else if (!style.SpaceAfterParens.ControlStatements) {
                    stripTrailingSpaces();
                }
            }
        }

        emitToken(caseStmt.matchesOrInside);

        depth++;
        for (auto* item : caseStmt.items) {
            nextIsPrimary = true;
            item->visit(*this);
        }

        nextIsPrimary = true;
        emitEndTokenTrivia(caseStmt.endcase);

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

        if (formatEnabled && !atLineStart && !output.empty() && !hasLeadingNewline(item.colon)) {
            if (style.SpaceBeforeCaseColon && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceBeforeCaseColon) {
                stripTrailingSpaces();
            }
        }

        emitToken(item.colon);

        if (formatEnabled && !atLineStart && !output.empty() &&
            !hasLeadingNewline(item.clause->getFirstToken())) {
            if (style.SpaceAfterCaseColon && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceAfterCaseColon) {
                stripTrailingSpaces();
            }
        }

        visitCaseItemClause(*item.clause);
    }

    void handle(const DefaultCaseItemSyntax& item) {
        nextIsPrimary = true;
        emitToken(item.defaultKeyword);

        if (formatEnabled && !atLineStart && !output.empty() && !hasLeadingNewline(item.colon)) {
            if (style.SpaceBeforeCaseColon && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceBeforeCaseColon) {
                stripTrailingSpaces();
            }
        }

        emitToken(item.colon);

        if (formatEnabled && !atLineStart && !output.empty() &&
            !hasLeadingNewline(item.clause->getFirstToken())) {
            if (style.SpaceAfterCaseColon && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceAfterCaseColon) {
                stripTrailingSpaces();
            }
        }

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

        if (formatEnabled && !atLineStart && !output.empty() &&
            !hasLeadingNewline(loop.openParen)) {
            if (style.SpaceBeforeParens.ControlStatements && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceBeforeParens.ControlStatements) {
                stripTrailingSpaces();
            }
        }

        emitToken(loop.openParen);
        loop.expr->visit(*this);
        emitToken(loop.closeParen);

        if (formatEnabled && !atLineStart && !output.empty() &&
            !hasLeadingNewline(loop.statement->getFirstToken())) {
            if (style.SpaceAfterParens.ControlStatements && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceAfterParens.ControlStatements) {
                stripTrailingSpaces();
            }
        }

        if (!BlockStatementSyntax::isKind(loop.statement->kind)) {
            forceStatementBreak(loop.statement->getFirstToken());
        }

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

        if (formatEnabled && !atLineStart && !output.empty() &&
            !hasLeadingNewline(loop.openParen)) {
            if (style.SpaceBeforeParens.ControlStatements && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceBeforeParens.ControlStatements) {
                stripTrailingSpaces();
            }
        }

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

        if (formatEnabled && !atLineStart && !output.empty() &&
            !hasLeadingNewline(loop.statement->getFirstToken())) {
            if (style.SpaceAfterParens.ControlStatements && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceAfterParens.ControlStatements) {
                stripTrailingSpaces();
            }
        }

        if (!BlockStatementSyntax::isKind(loop.statement->kind)) {
            forceStatementBreak(loop.statement->getFirstToken());
        }

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

        if (!BlockStatementSyntax::isKind(loop.statement->kind)) {
            forceStatementBreak(loop.statement->getFirstToken());
        }

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

        if (formatEnabled && !atLineStart && !output.empty() &&
            !hasLeadingNewline(loop.loopList->openParen)) {
            if (style.SpaceBeforeParens.ControlStatements && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceBeforeParens.ControlStatements) {
                stripTrailingSpaces();
            }
        }

        loop.loopList->visit(*this);

        if (formatEnabled && !atLineStart && !output.empty() &&
            !hasLeadingNewline(loop.statement->getFirstToken())) {
            if (style.SpaceAfterParens.ControlStatements && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceAfterParens.ControlStatements) {
                stripTrailingSpaces();
            }
        }

        if (!BlockStatementSyntax::isKind(loop.statement->kind)) {
            forceStatementBreak(loop.statement->getFirstToken());
        }

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

        bool const bareBody = !BlockStatementSyntax::isKind(loop.statement->kind);
        if (bareBody) {
            forceStatementBreak(loop.statement->getFirstToken());
        }

        visitBody(*loop.statement);

        if (bareBody) {
            forceStatementBreak(loop.whileKeyword);
            nextIsPrimary = true;
        }

        emitToken(loop.whileKeyword);

        if (formatEnabled && !atLineStart && !output.empty() &&
            !hasLeadingNewline(loop.openParen)) {
            if (style.SpaceBeforeParens.ControlStatements && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceBeforeParens.ControlStatements) {
                stripTrailingSpaces();
            }
        }

        emitToken(loop.openParen);
        loop.expr->visit(*this);
        emitToken(loop.closeParen);
        emitToken(loop.semi);
    }

    void handle(const TimingControlStatementSyntax& tcs) {
        if (tcs.label != nullptr) {
            tcs.label->visit(*this);
        }

        for (auto* attr : tcs.attributes) {
            attr->visit(*this);
        }

        tcs.timingControl->visit(*this);

        if (isEventControl(*tcs.timingControl) && formatEnabled && !atLineStart &&
            !output.empty() && output.back() == ')' &&
            !hasLeadingNewline(tcs.statement->getFirstToken())) {
            if (style.SpaceAfterParens.EventControls && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceAfterParens.EventControls) {
                stripTrailingSpaces();
            }
        }

        visitBody(*tcs.statement);
    }

    void handle(const GenerateBlockSyntax& b) { visitScopedBlock(b, b.end, b.members); }

    void handle(const GenerateRegionSyntax& r) { visitScopedBlock(r, r.endgenerate, r.members); }

    void handle(const FunctionDeclarationSyntax& f) {
        bool const shouldBreak = (f.kind == SyntaxKind::FunctionDeclaration &&
                                  style.BreakBeforeFunction) ||
                                 (f.kind == SyntaxKind::TaskDeclaration && style.BreakBeforeTask);

        if (shouldBreak) {
            insertBlankLineBefore(f.getFirstToken());
        }

        visitScopedBlock(f, f.end, f.items);
    }

    void handle(const SpecifyBlockSyntax& s) {
        if (style.BreakBeforeSpecifyBlock) {
            insertBlankLineBefore(s.getFirstToken());
        }

        visitScopedBlock(s, s.endspecify, s.items);
    }

    void handle(const DataDeclarationSyntax& decl) {
        nextLineKind = LineMetadata::Kind::Declaration;
        for (auto* attr : decl.attributes) {
            attr->visit(*this);
        }

        for (auto tok : decl.modifiers) {
            emitToken(tok);
        }

        decl.type->visit(*this);
        visitDeclarators(decl.declarators);
        emitToken(decl.semi);
    }

    void handle(const NetDeclarationSyntax& decl) {
        nextLineKind = LineMetadata::Kind::Declaration;
        for (auto* attr : decl.attributes) {
            attr->visit(*this);
        }

        emitToken(decl.netType);
        if (decl.strength != nullptr) {
            decl.strength->visit(*this);
        }

        emitToken(decl.expansionHint);
        decl.type->visit(*this);
        if (decl.delay != nullptr) {
            decl.delay->visit(*this);
        }

        visitDeclarators(decl.declarators);
        emitToken(decl.semi);
    }

    void handle(const PortDeclarationSyntax& decl) {
        nextLineKind = LineMetadata::Kind::Declaration;
        for (auto* attr : decl.attributes) {
            attr->visit(*this);
        }

        decl.header->visit(*this);
        visitDeclarators(decl.declarators);
        emitToken(decl.semi);
    }

    void handle(const ParameterDeclarationStatementSyntax& decl) {
        nextLineKind = LineMetadata::Kind::Declaration;
        for (auto* attr : decl.attributes) {
            attr->visit(*this);
        }

        decl.parameter->visit(*this);
        emitToken(decl.semi);
    }

    void handle(const VariableDimensionSyntax& dim) {
        inDimension = true;
        visitDefault(dim);
        inDimension = false;
    }

    void handle(const ContinuousAssignSyntax& assign) {
        nextLineKind = LineMetadata::Kind::Assignment;
        for (auto* attr : assign.attributes) {
            attr->visit(*this);
        }

        emitToken(assign.assign);
        if (assign.strength != nullptr) {
            assign.strength->visit(*this);
        }

        if (assign.delay != nullptr) {
            assign.delay->visit(*this);
        }

        for (const auto& elem : assign.assignments.elems()) {
            if (elem.isNode()) {
                elem.node()->visit(*this);
            }
            else if (elem.isToken()) {
                emitToken(elem.token());
            }
        }

        emitToken(assign.semi);
    }

    void handle(const HierarchicalInstanceSyntax& inst) {
        if (inst.decl != nullptr) {
            if (formatEnabled && !atLineStart && !output.empty() && output.back() == ')') {
                output += ' ';
            }
            inst.decl->visit(*this);
        }

        if (formatEnabled && !atLineStart && !output.empty() &&
            !hasLeadingNewline(inst.openParen)) {
            if (style.SpaceBeforeParens.PortList && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceBeforeParens.PortList) {
                stripTrailingSpaces();
            }
        }
        emitToken(inst.openParen);

        depth++;
        visitSeparatedList(inst.connections);

        portItemNextIndent = true;
        nextIsPrimary = true;
        emitEndTokenTrivia(inst.closeParen);
        portItemNextIndent = false;

        depth--;
        nextIsPrimary = true;
        nextLineKind = LineMetadata::Kind::PortListBoundary;
        emitToken(inst.closeParen);
    }

    void handle(const HierarchyInstantiationSyntax& inst) {
        for (auto* attr : inst.attributes) {
            attr->visit(*this);
        }

        emitToken(inst.type);
        if (inst.parameters != nullptr) {
            inst.parameters->visit(*this);
        }

        for (const auto& elem : inst.instances.elems()) {
            if (elem.isNode()) {
                elem.node()->visit(*this);
            }
            else if (elem.isToken()) {
                emitToken(elem.token());
            }
        }

        emitToken(inst.semi);
    }

    void handle(const ParameterValueAssignmentSyntax& p) {
        if (formatEnabled && !atLineStart && !output.empty() && !hasLeadingNewline(p.hash)) {
            if (style.SpaceBeforeParens.ParameterList && output.back() != ' ') {
                output += ' ';
            }
            else if (!style.SpaceBeforeParens.ParameterList) {
                stripTrailingSpaces();
            }
        }
        emitToken(p.hash);
        emitToken(p.openParen);

        depth++;
        visitSeparatedList(p.parameters);

        portItemNextIndent = true;
        nextIsPrimary = true;
        emitEndTokenTrivia(p.closeParen);
        portItemNextIndent = false;

        depth--;
        nextIsPrimary = true;
        nextLineKind = LineMetadata::Kind::PortListBoundary;
        emitToken(p.closeParen);
    }

private:
    const Style& style;
    std::string output;
    std::vector<LineMetadata> lineMetadata;
    LineMetadata currentLineMeta;
    size_t lineStart = 0; ///< Index into output where the current line begins.
    unsigned depth = 0;
    unsigned lineDepth = 0; ///< Depth when content was last emitted on the current line.
    bool atLineStart = true;
    bool nextIsPrimary = true;
    bool portItemNextIndent = false;
    bool formatEnabled = true;
    unsigned emptyLineCount = 0;
    size_t triviaSkip = 0;
    bool beforeOperator = false;
    bool afterOperator = false;
    unsigned bracketDepth = 0;
    bool inDimension = false;
    bool afterDimCloseBracket = false;
    bool afterComma = false;
    bool afterSemicolon = false;
    bool afterOpenBrace = false;
    bool afterOpenBracket = false;
    bool afterOpenParen = false;
    bool afterCloseBracket = false;
    bool afterIntegerBase = false;
    bool spaceBeforeCloseBracePending = false;
    bool spaceBeforeCloseBracketPending = false;
    bool spaceBeforeCloseParenPending = false;
    std::optional<std::regex> offRegex;
    std::optional<LineMetadata::Kind> nextLineKind;

    static bool hasLeadingNewline(Token tok) {
        if (!tok) {
            return false;
        }

        return std::ranges::any_of(tok.trivia(),
                                   [](const auto& t) { return t.kind == TriviaKind::EndOfLine; });
    }

    static bool hasLeadingTrailingComment(Token tok) {
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

    size_t emitLeadingTrailingComment(Token tok) {
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

    // Emit standalone comment trivia of a closing token at the current (body) depth before the
    // depth is decremented; sets triviaSkip so emitToken does not re-emit. Trailing comments on
    // the previous line (before the first EndOfLine) are left for emitToken to handle normally.
    void emitEndTokenTrivia(Token tok) {
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

    // Returns true if the token's leading trivia contains a blank line (two consecutive EndOfLine
    // pieces) before any non-whitespace trivia.
    static bool hasLeadingBlankLine(Token tok) {
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

    [[nodiscard]] bool outputHasBlankLine() const {
        return atLineStart && output.size() >= 2 && output[output.size() - 1] == '\n' &&
               output[output.size() - 2] == '\n';
    }

    void finalizeLine() {
        currentLineMeta.depth = lineDepth;
        lineMetadata.push_back(std::move(currentLineMeta));
        currentLineMeta = {};
    }

    void forceStatementBreak(Token tok) {
        if (style.OneStatementPerLine && formatEnabled && !hasLeadingNewline(tok)) {
            forceNewline();
        }
    }

    [[nodiscard]] bool shouldBreakConditionalBody(const SyntaxNode& body) const {
        return !BlockStatementSyntax::isKind(body.kind) && !shouldCompactConditionalBody(body);
    }

    [[nodiscard]] bool shouldCompactConditionalBody(const SyntaxNode& body) const {
        if (!style.CompactConditionals || !style.OneStatementPerLine || !formatEnabled) {
            return false;
        }

        auto width = flatWidth(body);
        if (width == 0) {
            return false;
        }

        if (style.ColumnLimit == 0) {
            return true;
        }

        return output.size() - lineStart + 1 + width <= style.ColumnLimit;
    }

    // Strip trailing spaces from output and emit a newline, setting atLineStart.
    void forceNewline() {
        while (!output.empty() && output.back() == ' ') {
            output.pop_back();
        }
        output += '\n';
        finalizeLine();
        lineStart = output.size();
        atLineStart = true;
    }

    void insertBlankLineBefore(Token firstToken) {
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

    bool shouldSuppressBlankLine() {
        if (style.RemoveEmptyLines || emptyLineCount > 0) {
            emptyLineCount++;
        }
        return formatEnabled && emptyLineCount > style.MaxEmptyLinesToKeep;
    }

    void emitTrivia(const Trivia& t) {
        if (t.kind == TriviaKind::EndOfLine) {
            beforeOperator = false;
            afterOperator = false;
            afterComma = false;
            afterIntegerBase = false;
            afterSemicolon = false;
            afterOpenBrace = false;
            afterOpenBracket = false;
            afterOpenParen = false;
            spaceBeforeCloseBracePending = false;
            spaceBeforeCloseBracketPending = false;
            spaceBeforeCloseParenPending = false;

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
                output += '\n';
                finalizeLine();
                lineStart = output.size();
                atLineStart = true;
            }
            return;
        }

        if (t.kind == TriviaKind::Whitespace) {
            emitWhitespaceTrivia(t);
            return;
        }

        emitDeferredSpaces();
        beforeOperator = false;
        afterOperator = false;
        afterComma = false;
        afterIntegerBase = false;
        afterSemicolon = false;
        afterOpenBrace = false;
        afterOpenBracket = false;
        afterOpenParen = false;

        auto raw = t.getRawText();

        if (t.kind == TriviaKind::LineComment || t.kind == TriviaKind::BlockComment) {
            bool const isOff = formatEnabled && matchesPragma(raw, "slang-format off");
            bool const isOn = !formatEnabled && matchesPragma(raw, "slang-format on");

            if (atLineStart) {
                currentLineMeta.kind = LineMetadata::Kind::Comment;
            }
            else {
                currentLineMeta.trailingCommentPos = output.size() - lineStart;
            }

            // Indent while still in the current format state (before toggling).
            if (formatEnabled && atLineStart) {
                emitIndentRaw();
            }
            output += raw;
            lineDepth = depth;
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

    void emitWhitespaceTrivia(const Trivia& t) {
        if (!formatEnabled) {
            output += t.getRawText();
            return;
        }
    }

    [[nodiscard]] bool shouldSpaceAroundOperator() const {
        if (!style.SpaceAroundOperators) {
            return false;
        }
        if (bracketDepth > 0 && !style.SpacesInBrackets) {
            return false;
        }
        return true;
    }

    [[nodiscard]] static bool needsStripBefore(TokenKind kind) {
        return kind == TokenKind::Comma || kind == TokenKind::Semicolon;
    }

    void normalizeBeforeOpen(TokenKind kind) {
        if (!formatEnabled || atLineStart) {
            return;
        }

        if (kind == TokenKind::OpenBracket && inDimension && style.SpaceBeforeBrackets &&
            !output.empty() && output.back() != ' ') {
            output += ' ';
        }
    }

    void normalizeBeforeClose(TokenKind kind) {
        if (spaceBeforeCloseBracePending && kind == TokenKind::CloseBrace) {
            stripTrailingSpaces();
            if (style.SpacesInBraces) {
                output += ' ';
            }
        }
        if (spaceBeforeCloseBracketPending && kind == TokenKind::CloseBracket) {
            stripTrailingSpaces();
            if (inDimension && style.SpacesInBrackets) {
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

    void stripTrailingSpaces() {
        while (output.size() > lineStart && output.back() == ' ') {
            output.pop_back();
        }
    }

    // Emit computed indentation directly into output; used for comment trivia
    // and raw text.
    void emitIndentRaw() {
        if (portItemNextIndent) {
            output.append(((depth - 1) * style.IndentWidth) + style.ParameterPortListIndentWidth,
                          ' ');
        }
        else {
            unsigned spaces = depth * style.IndentWidth;
            if (!nextIsPrimary) {
                spaces += style.ContinuationIndentWidth;
            }
            output.append(spaces, ' ');
        }
        atLineStart = false;
    }

    void emitDeferredSpaces() {
        if (afterOperator && shouldSpaceAroundOperator()) {
            output += ' ';
        }
        if (afterComma && style.SpaceAfterComma) {
            output += ' ';
        }
        if (afterSemicolon && style.SpaceAfterSemicolon) {
            output += ' ';
        }
        if (afterDimCloseBracket && style.SpaceAfterBrackets) {
            output += ' ';
        }
        if (afterOpenBrace && style.SpacesInBraces) {
            output += ' ';
        }
        if (afterOpenBracket && inDimension && style.SpacesInBrackets) {
            output += ' ';
        }
        if (afterOpenParen && style.SpacesInParens) {
            output += ' ';
        }
    }

    void emitSeparator(std::string_view raw) {
        if (formatEnabled && !atLineStart && !afterIntegerBase && !raw.empty() && !output.empty() &&
            needsSeparator(output.back(), raw.front())) {
            output += ' ';
        }
    }

    void emitToken(Token tok) {
        if (!tok || tok.isMissing()) {
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
        emitSeparator(raw);

        if (formatEnabled && atLineStart && !raw.empty()) {
            if (nextLineKind) {
                currentLineMeta.kind = *nextLineKind;
                nextLineKind.reset();
            }

            if (portItemNextIndent) {
                output.append(
                    ((depth - 1) * style.IndentWidth) + style.ParameterPortListIndentWidth, ' ');
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
            if (nextLineKind) {
                currentLineMeta.kind = *nextLineKind;
                nextLineKind.reset();
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

        output += raw;
        lineDepth = depth;
        nextIsPrimary = false;
        emptyLineCount = 0;
        updateAfterTokenFlags(tok);
    }

    void updateAfterTokenFlags(Token tok) {
        afterOperator = formatEnabled && beforeOperator;
        beforeOperator = false;
        afterComma = formatEnabled && tok.kind == TokenKind::Comma;
        afterSemicolon = formatEnabled && tok.kind == TokenKind::Semicolon;
        afterOpenBrace = formatEnabled && (tok.kind == TokenKind::OpenBrace ||
                                           tok.kind == TokenKind::ApostropheOpenBrace);
        if (afterOpenBrace) {
            spaceBeforeCloseBracePending = true;
        }
        afterOpenBracket = formatEnabled && tok.kind == TokenKind::OpenBracket;
        if (afterOpenBracket) {
            spaceBeforeCloseBracketPending = true;
            bracketDepth++;
        }
        afterOpenParen = formatEnabled && tok.kind == TokenKind::OpenParenthesis;
        if (afterOpenParen) {
            spaceBeforeCloseParenPending = true;
        }
        afterCloseBracket = formatEnabled && tok.kind == TokenKind::CloseBracket;
        afterDimCloseBracket = afterCloseBracket && inDimension;
        afterIntegerBase = formatEnabled && tok.kind == TokenKind::IntegerBase;
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

    void visitDeclarators(const SeparatedSyntaxList<DeclaratorSyntax>& list) {
        bool first = true;
        for (const auto& elem : list.elems()) {
            if (elem.isNode()) {
                if (!first && atLineStart) {
                    currentLineMeta.kind = LineMetadata::Kind::Continuation;
                }
                first = false;
                elem.node()->visit(*this);
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
    void visitProceduralBody(const StatementSyntax& stmt, BlockBreakStyle brkStyle) {
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

            if (isEventControl(*tcs.timingControl) && formatEnabled && !atLineStart &&
                !output.empty() && output.back() == ')' &&
                !hasLeadingNewline(body->getFirstToken())) {
                if (style.SpaceAfterParens.EventControls && output.back() != ' ') {
                    output += ' ';
                }
                else if (!style.SpaceAfterParens.EventControls) {
                    stripTrailingSpaces();
                }
            }
        }

        bool const forceBreak = shouldBreakAfterProcedural(brkStyle, *body,
                                                           style.OneStatementPerLine);
        if (forceBreak && !hasLeadingNewline(body->getFirstToken())) {
            forceNewline();
            nextIsPrimary = true;
        }

        visitBody(*body);
    }

    static bool shouldBreakAfterProcedural(BlockBreakStyle brkStyle, const StatementSyntax& body,
                                           bool oneStatementPerLine) {
        if (brkStyle == BlockBreakStyle::Never) {
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

        if (brkStyle == BlockBreakStyle::Always) {
            return true;
        }

        if (BlockStatementSyntax::isKind(body.kind)) {
            const auto& block = body.as<BlockStatementSyntax>();
            if (block.items.size() <= 1) {
                return false;
            }

            if (oneStatementPerLine) {
                return true;
            }

            if (hasLeadingNewline(block.end)) {
                return true;
            }

            return std::ranges::any_of(block.items, [](const auto* item) {
                return item->getFirstToken() && hasLeadingNewline(item->getFirstToken());
            });
        }

        if (ConditionalStatementSyntax::isKind(body.kind) &&
            body.as<ConditionalStatementSyntax>().elseClause != nullptr) {
            return true;
        }

        return containsBlock(body);
    }

    static bool shouldBreakBeforeProcedural(BlockBreakStyle brkStyle, const StatementSyntax& stmt,
                                            bool oneStatementPerLine) {
        if (brkStyle == BlockBreakStyle::Never) {
            return false;
        }

        const auto* body = &stmt;
        if (stmt.kind == SyntaxKind::TimingControlStatement) {
            body = stmt.as<TimingControlStatementSyntax>().statement;
        }

        if (brkStyle == BlockBreakStyle::Always) {
            return true;
        }

        if (BlockStatementSyntax::isKind(body->kind)) {
            const auto& block = body->as<BlockStatementSyntax>();
            if (block.items.size() <= 1) {
                return block.items.size() == 1 && containsBlock(*block.items.front());
            }

            if (oneStatementPerLine) {
                return true;
            }

            if (hasLeadingNewline(block.end)) {
                return true;
            }

            return std::ranges::any_of(block.items, [](const auto* item) {
                return item->getFirstToken() && hasLeadingNewline(item->getFirstToken());
            });
        }

        if (ConditionalStatementSyntax::isKind(body->kind) &&
            body->as<ConditionalStatementSyntax>().elseClause != nullptr) {
            return true;
        }

        return containsBlock(*body);
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
                    if (!depthBumped) {
                        depth++;
                    }

                    nextIsPrimary = true;
                    emitEndTokenTrivia(tok);
                    depth--;
                    depthBumped = false;

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
    tree = applyInsertParens(tree, style);
    tree = applyEventSeparator(tree, style);
    tree = applyPackedDimensionBounds(tree, style);
    tree = applyUnpackedDimensionBounds(tree, style);
    FormatPrinter printer(style);

    auto [result, lineMetadata] = printer.print(*tree);
    result = applyAlignment(result, style, lineMetadata);

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
