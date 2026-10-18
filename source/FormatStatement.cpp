// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "FormatPrinter.h"
#include "SyntaxHelper.h"
#include "TokenHelper.h"

#include <slang/syntax/AllSyntax.h>
#include <slang/syntax/SyntaxKind.h>
#include <slang/syntax/SyntaxNode.h>

using namespace slang;
using namespace slang::format;
using namespace slang::parsing;
using namespace slang::syntax;

namespace slang::format {

void FormatPrinter::handle(const BlockStatementSyntax& block) {
    if (block.label != nullptr) {
        block.label->visit(*this);
    }

    visitDefault(block.attributes);

    emitToken(block.begin);
    if (block.blockName != nullptr) {
        block.blockName->visit(*this);
    }

    const bool breakAfterBegin = style.BreakAfterBegin && !block.items.empty() &&
                                 !hasLeadingNewline(block.items.front()->getFirstToken());
    const bool breakBeforeEnd = style.BreakBeforeEnd && !hasLeadingNewline(block.end);

    depth++;
    bool firstItem{true};
    for (auto* item : block.items) {
        const bool needsBreak = (firstItem && breakAfterBegin) ||
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

void FormatPrinter::handle(const CaseStatementSyntax& caseStmt) {
    if (caseStmt.label != nullptr) {
        caseStmt.label->visit(*this);
    }

    visitDefault(caseStmt.attributes);

    emitToken(caseStmt.uniqueOrPriority);
    emitToken(caseStmt.caseKeyword);

    normalizeSpaceBefore(caseStmt.openParen, style.SpaceBeforeParens.ControlStatements, true);

    emitToken(caseStmt.openParen);
    caseStmt.expr->visit(*this);
    emitToken(caseStmt.closeParen);

    if (caseStmt.matchesOrInside && !caseStmt.matchesOrInside.isMissing()) {
        normalizeSpaceBefore(caseStmt.matchesOrInside, style.SpaceAfterParens.ControlStatements,
                             true);
    }

    emitToken(caseStmt.matchesOrInside);

    visitMembers(caseStmt.items, caseStmt.endcase, nullptr);
}

void FormatPrinter::handle(const ConcurrentAssertionStatementSyntax& stmt) {
    if (stmt.label != nullptr) {
        stmt.label->visit(*this);

        if (style.BreakAfterAssertionLabel && formatEnabled) {
            forceNewline();
            nextIsPrimary = true;
        }
    }

    visitDefault(stmt.attributes);

    emitToken(stmt.keyword);
    emitToken(stmt.propertyOrSequence);
    emitToken(stmt.openParen);
    stmt.propertySpec->visit(*this);
    emitToken(stmt.closeParen);

    if (stmt.action->statement != nullptr) {
        stmt.action->statement->visit(*this);
    }

    if (stmt.action->elseClause != nullptr) {
        nextIsPrimary = true;
        emitToken(stmt.action->elseClause->elseKeyword);
        stmt.action->elseClause->clause->visit(*this);
    }
}

void FormatPrinter::handle(const ConditionalStatementSyntax& stmt) {
    if (stmt.label != nullptr) {
        stmt.label->visit(*this);
    }

    visitDefault(stmt.attributes);

    emitToken(stmt.uniqueOrPriority);
    emitToken(stmt.ifKeyword);

    normalizeSpaceBefore(stmt.openParen, style.SpaceBeforeParens.ControlStatements, true);

    emitToken(stmt.openParen);
    stmt.predicate->visit(*this);
    emitToken(stmt.closeParen);

    normalizeSpaceBefore(stmt.statement->getFirstToken(), style.SpaceAfterParens.ControlStatements,
                         true);

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

void FormatPrinter::handle(const DefaultCaseItemSyntax& item) {
    nextIsPrimary = true;
    emitToken(item.defaultKeyword);

    normalizeSpaceBefore(item.colon, style.SpaceBeforeCaseColon, false);

    emitToken(item.colon);

    if (item.clause->kind == SyntaxKind::GenerateBlock && formatEnabled) {
        forceNewline();
        nextIsPrimary = true;
    }
    else {
        normalizeSpaceBefore(item.clause->getFirstToken(), style.SpaceAfterCaseColon, false);
    }

    visitCaseItemClause(*item.clause);
}

void FormatPrinter::handle(const DefaultRsCaseItemSyntax& item) {
    emitToken(item.defaultKeyword);

    ensureSingleSpace();

    emitToken(item.colon);

    ensureSpace();

    item.item->visit(*this);
    emitToken(item.semi);
}

void FormatPrinter::handle(const DoWhileStatementSyntax& loop) {
    if (loop.label != nullptr) {
        loop.label->visit(*this);
    }

    visitDefault(loop.attributes);

    emitToken(loop.doKeyword);

    const bool bareBody = !BlockStatementSyntax::isKind(loop.statement->kind);
    if (bareBody) {
        forceStatementBreak(loop.statement->getFirstToken());
    }

    visitBody(*loop.statement);

    if (bareBody) {
        forceStatementBreak(loop.whileKeyword);
        nextIsPrimary = true;
    }

    emitToken(loop.whileKeyword);

    normalizeSpaceBefore(loop.openParen, style.SpaceBeforeParens.ControlStatements, true);

    emitToken(loop.openParen);
    loop.expr->visit(*this);
    emitToken(loop.closeParen);
    emitToken(loop.semi);
}

void FormatPrinter::handle(const ForLoopStatementSyntax& loop) {
    if (loop.label != nullptr) {
        loop.label->visit(*this);
    }

    visitDefault(loop.attributes);

    emitToken(loop.forKeyword);

    normalizeSpaceBefore(loop.openParen, style.SpaceBeforeParens.ControlStatements, true);

    emitToken(loop.openParen);
    visitDefault(loop.initializers);

    emitToken(loop.semi1);
    if (loop.stopExpr != nullptr) {
        loop.stopExpr->visit(*this);
    }

    emitToken(loop.semi2);
    visitDefault(loop.steps);

    emitToken(loop.closeParen);

    normalizeSpaceBefore(loop.statement->getFirstToken(), style.SpaceAfterParens.ControlStatements,
                         true);

    if (!BlockStatementSyntax::isKind(loop.statement->kind)) {
        forceStatementBreak(loop.statement->getFirstToken());
    }

    visitBody(*loop.statement);
}

void FormatPrinter::handle(const ForeachLoopStatementSyntax& loop) {
    if (loop.label != nullptr) {
        loop.label->visit(*this);
    }

    visitDefault(loop.attributes);

    emitToken(loop.keyword);

    normalizeSpaceBefore(loop.loopList->openParen, style.SpaceBeforeParens.ControlStatements, true);

    loop.loopList->visit(*this);

    normalizeSpaceBefore(loop.statement->getFirstToken(), style.SpaceAfterParens.ControlStatements,
                         true);

    if (shouldBreakConditionalBody(*loop.statement)) {
        forceStatementBreak(loop.statement->getFirstToken());
    }

    visitBody(*loop.statement);
}

void FormatPrinter::handle(const ForeverStatementSyntax& loop) {
    if (loop.label != nullptr) {
        loop.label->visit(*this);
    }

    visitDefault(loop.attributes);

    emitToken(loop.foreverKeyword);

    if (shouldBreakConditionalBody(*loop.statement)) {
        forceStatementBreak(loop.statement->getFirstToken());
    }

    visitBody(*loop.statement);
}

void FormatPrinter::handle(const ImmediateAssertionStatementSyntax& stmt) {
    auto hasElse = stmt.action->elseClause != nullptr;

    if (stmt.label != nullptr) {
        stmt.label->visit(*this);

        if (style.BreakAfterAssertionLabel && formatEnabled && hasElse) {
            forceNewline();
            nextIsPrimary = true;
        }
    }

    visitDefault(stmt.attributes);

    emitToken(stmt.keyword);
    if (stmt.delay != nullptr) {
        stmt.delay->visit(*this);
        ensureSpace();
    }
    stmt.expr->visit(*this);

    if (stmt.action->statement != nullptr) {
        stmt.action->statement->visit(*this);
    }

    if (hasElse) {
        nextIsPrimary = true;
        emitToken(stmt.action->elseClause->elseKeyword);
        stmt.action->elseClause->clause->visit(*this);
    }
}

void FormatPrinter::handle(const LoopStatementSyntax& loop) {
    if (loop.label != nullptr) {
        loop.label->visit(*this);
    }

    visitDefault(loop.attributes);

    emitToken(loop.repeatOrWhile);

    normalizeSpaceBefore(loop.openParen, style.SpaceBeforeParens.ControlStatements, true);

    emitToken(loop.openParen);
    loop.expr->visit(*this);
    emitToken(loop.closeParen);

    normalizeSpaceBefore(loop.statement->getFirstToken(), style.SpaceAfterParens.ControlStatements,
                         true);

    if (shouldBreakConditionalBody(*loop.statement)) {
        forceStatementBreak(loop.statement->getFirstToken());
    }

    visitBody(*loop.statement);
}

void FormatPrinter::handle(const NamedBlockClauseSyntax& clause) {
    ensureSpace();
    emitToken(clause.colon);
    ensureSpace();
    emitToken(clause.name);
}

void FormatPrinter::handle(const NamedLabelSyntax& label) {
    emitToken(label.name);
    ensureSpace();
    emitToken(label.colon);
}

void FormatPrinter::handle(const ProductionSyntax& prod) {
    if (prod.dataType != nullptr) {
        prod.dataType->visit(*this);
    }

    emitToken(prod.name);

    if (prod.portList != nullptr) {
        prod.portList->visit(*this);
    }

    ensureSingleSpace();

    emitToken(prod.colon);

    ensureSpace();

    visitDefault(prod.rules);

    emitToken(prod.semi);
}

void FormatPrinter::handle(const RandCaseItemSyntax& item) {
    item.expr->visit(*this);

    normalizeSpaceBefore(item.colon, style.SpaceBeforeCaseColon, false);

    emitToken(item.colon);

    normalizeSpaceBefore(item.statement->getFirstToken(), style.SpaceAfterCaseColon, false);

    item.statement->visit(*this);
}

void FormatPrinter::handle(const RandCaseStatementSyntax& stmt) {
    if (stmt.label != nullptr) {
        stmt.label->visit(*this);
    }

    visitDefault(stmt.attributes);

    emitToken(stmt.randCase);

    visitMembers(stmt.items, stmt.endCase, nullptr);
}

void FormatPrinter::handle(const RandSequenceStatementSyntax& stmt) {
    if (stmt.label != nullptr) {
        stmt.label->visit(*this);
    }

    visitDefault(stmt.attributes);

    emitToken(stmt.randsequence);
    emitToken(stmt.openParen);
    emitToken(stmt.firstProduction);
    emitToken(stmt.closeParen);

    visitMembers(stmt.productions, stmt.endsequence, nullptr);
}

void FormatPrinter::handle(const RsCaseSyntax& c) {
    emitToken(c.keyword);
    emitToken(c.openParen);
    c.expr->visit(*this);
    emitToken(c.closeParen);

    depth++;
    for (auto* item : c.items) {
        if (formatEnabled && !hasLeadingNewline(item->getFirstToken())) {
            forceNewline();
        }
        nextIsPrimary = true;
        item->visit(*this);
    }

    if (formatEnabled && !hasLeadingNewline(c.endcase)) {
        forceNewline();
    }
    nextIsPrimary = true;
    emitEndTokenTrivia(c.endcase);

    depth--;
    nextIsPrimary = true;
    emitToken(c.endcase);
}

void FormatPrinter::handle(const StandardCaseItemSyntax& item) {
    nextIsPrimary = true;

    visitDefault(item.expressions);

    normalizeSpaceBefore(item.colon, style.SpaceBeforeCaseColon, false);

    emitToken(item.colon);

    normalizeSpaceBefore(item.clause->getFirstToken(), style.SpaceAfterCaseColon, false);

    visitCaseItemClause(*item.clause);
}

void FormatPrinter::handle(const StandardRsCaseItemSyntax& item) {
    visitDefault(item.expressions);

    ensureSingleSpace();

    emitToken(item.colon);

    ensureSpace();

    item.item->visit(*this);
    emitToken(item.semi);
}

void FormatPrinter::handle(const TimingControlStatementSyntax& tcs) {
    if (tcs.label != nullptr) {
        tcs.label->visit(*this);
    }

    visitDefault(tcs.attributes);

    tcs.timingControl->visit(*this);

    if (isEventControl(*tcs.timingControl) && !output.empty() && output.back() == ')') {
        normalizeSpaceBefore(tcs.statement->getFirstToken(), style.SpaceAfterParens.EventControls,
                             true);
    }

    if (tcs.statement->kind == SyntaxKind::EmptyStatement) {
        tcs.statement->visit(*this);
    }
    else {
        visitBody(*tcs.statement);
    }
}

void FormatPrinter::forceStatementBreak(Token tok) {
    if (style.OneStatementPerLine && formatEnabled && !hasLeadingNewline(tok)) {
        forceNewline();
    }
}

bool FormatPrinter::shouldBreakConditionalBody(const SyntaxNode& body) const {
    return !BlockStatementSyntax::isKind(body.kind) && !shouldCompactConditionalBody(body);
}

bool FormatPrinter::shouldCompactConditionalBody(const SyntaxNode& body) const {
    if (!style.CompactControlStatements || !style.OneStatementPerLine || !formatEnabled) {
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

void FormatPrinter::visitCaseItemClause(const SyntaxNode& clause) {
    const bool nextLine = clause.getFirstToken() && hasLeadingNewline(clause.getFirstToken());
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

} // namespace slang::format
