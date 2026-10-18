// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Align.h"
#include "FormatPrinter.h"
#include "TokenHelper.h"

#include <algorithm>
#include <cstddef>

#include <slang/syntax/AllSyntax.h>
#include <slang/syntax/SyntaxKind.h>
#include <slang/syntax/SyntaxNode.h>
#include <slang/util/Util.h>

using namespace slang;
using namespace slang::format;
using namespace slang::parsing;
using namespace slang::syntax;

namespace {

bool hasExpandingBlock(const SyntaxNode& node) {
    for (size_t i = 0; i < node.getChildCount(); i++) {
        const auto* child = node.childNode(i);
        if (child == nullptr) {
            continue;
        }
        if (child->kind == SyntaxKind::DistConstraintList ||
            child->kind == SyntaxKind::ConstraintBlock) {
            return true;
        }
        if (hasExpandingBlock(*child)) {
            return true;
        }
    }
    return false;
}

} // namespace

namespace slang::format {

void FormatPrinter::handle(const BinaryBinsSelectExprSyntax& expr) {
    expr.left->visit(*this);
    beforeOperator = formatEnabled;
    emitToken(expr.op);
    expr.right->visit(*this);
}

void FormatPrinter::handle(const BinsSelectionSyntax& bs) {
    visitDefault(bs.attributes);

    emitToken(bs.keyword);
    emitToken(bs.name);
    beforeOperator = formatEnabled;
    emitToken(bs.equals);
    bs.expr->visit(*this);
    if (bs.iff != nullptr) {
        bs.iff->visit(*this);
    }
    emitToken(bs.semi);
}

void FormatPrinter::handle(const ClassDeclarationSyntax& c) {
    visitDefault(c.attributes);

    emitToken(c.virtualOrInterface);
    emitToken(c.classKeyword);
    if (c.finalSpecifier != nullptr) {
        c.finalSpecifier->visit(*this);
    }

    emitToken(c.name);
    if (c.parameters != nullptr) {
        c.parameters->visit(*this);
    }

    if (c.extendsClause != nullptr) {
        c.extendsClause->visit(*this);
    }

    if (c.implementsClause != nullptr) {
        c.implementsClause->visit(*this);
    }

    emitToken(c.semi);
    visitMembers(c.items, c.endClass, c.endBlockName);
}

void FormatPrinter::handle(const ConstraintBlockSyntax& b) {
    const auto itemCount = b.items.size();

    if (formatEnabled && style.AllowShortConstraintBlocksOnASingleLine && itemCount == 1 &&
        !hasExpandingBlock(*b.items[0])) {
        const auto w = flatWidth(*b.items[0], true);
        if (w > 0) {
            const auto currentCol = output.size() - lineStart;
            const auto needed = currentCol + 1 + w + 1;
            if (style.ColumnLimit == 0 || needed <= style.ColumnLimit) {
                emitToken(b.openBrace);
                triviaSkip = b.items[0]->getFirstToken().trivia().size();
                b.items[0]->visit(*this);
                triviaSkip = b.closeBrace.trivia().size();
                emitToken(b.closeBrace);
                return;
            }
        }
    }

    const bool nested = isInsideControlCondition(b);
    unsigned const extraDepth = nested && style.IndentWidth > 0
                                    ? style.ContinuationIndentWidth / style.IndentWidth
                                    : 0;

    emitToken(b.openBrace);

    depth += 1 + extraDepth;
    for (auto* item : b.items) {
        if (formatEnabled && !atLineStart && !hasLeadingNewline(item->getFirstToken())) {
            forceNewline();
        }
        nextIsPrimary = true;
        item->visit(*this);
    }

    nextIsPrimary = true;
    emitEndTokenTrivia(b.closeBrace);

    depth -= 1 + extraDepth;
    if (nested) {
        depth += extraDepth;
    }

    breakBeforeClose(b.closeBrace);

    nextIsPrimary = true;
    emitToken(b.closeBrace);
    if (nested) {
        depth -= extraDepth;
    }
}

void FormatPrinter::handle(const ConstraintDeclarationSyntax& c) {
    visitDefault(c.attributes);

    visitDefault(c.qualifiers);

    emitToken(c.keyword);
    visitDefault(c.specifiers);

    c.name->visit(*this);
    c.block->visit(*this);
}

void FormatPrinter::handle(const CoverCrossSyntax& c) {
    visitDefault(c.attributes);

    if (c.label != nullptr) {
        c.label->visit(*this);
    }

    emitToken(c.cross);
    visitDefault(c.items);

    if (c.iff != nullptr) {
        c.iff->visit(*this);
    }

    if (c.members.empty()) {
        emitToken(c.emptySemi);
        return;
    }

    if (formatEnabled && style.AllowShortCoverpointsOnASingleLine &&
        membersFitOnSingleLine(c.members)) {
        emitToken(c.openBrace);
        emitMembersInline(c.members);
        triviaSkip = c.closeBrace.trivia().size();
        emitToken(c.closeBrace);
        emitToken(c.emptySemi);
        return;
    }

    emitToken(c.openBrace);

    depth++;
    for (auto* member : c.members) {
        if (formatEnabled && !atLineStart && !hasLeadingNewline(member->getFirstToken())) {
            forceNewline();
        }
        nextIsPrimary = true;
        member->visit(*this);
    }

    nextIsPrimary = true;
    emitEndTokenTrivia(c.closeBrace);

    depth--;

    breakBeforeClose(c.closeBrace);

    nextIsPrimary = true;
    emitToken(c.closeBrace);
    emitToken(c.emptySemi);
}

void FormatPrinter::handle(const CoverageBinsSyntax& cb) {
    visitDefault(cb.attributes);

    emitToken(cb.wildcard);
    emitToken(cb.keyword);
    emitToken(cb.name);
    if (cb.size != nullptr) {
        cb.size->visit(*this);
    }
    beforeOperator = formatEnabled;
    emitToken(cb.equals);
    cb.initializer->visit(*this);
    if (cb.iff != nullptr) {
        cb.iff->visit(*this);
    }
    emitToken(cb.semi);
}

void FormatPrinter::handle(const CovergroupDeclarationSyntax& c) {
    visitDefault(c.attributes);

    emitToken(c.covergroup);
    emitToken(c.extends);
    emitToken(c.name);
    if (c.portList != nullptr) {
        c.portList->visit(*this);
    }

    if (c.event != nullptr) {
        c.event->visit(*this);
    }

    emitToken(c.semi);
    visitMembers(c.members, c.endgroup, c.endBlockName);
}

void FormatPrinter::handle(const CoverpointSyntax& c) {
    visitDefault(c.attributes);

    c.type->visit(*this);
    if (c.label != nullptr) {
        emitToken(c.label->name);
        emitToken(c.label->colon);
    }

    emitToken(c.coverpoint);
    c.expr->visit(*this);
    if (c.iff != nullptr) {
        c.iff->visit(*this);
    }

    if (c.members.empty()) {
        emitToken(c.emptySemi);
        return;
    }

    if (formatEnabled && style.AllowShortCoverpointsOnASingleLine &&
        membersFitOnSingleLine(c.members)) {
        emitToken(c.openBrace);
        emitMembersInline(c.members);
        triviaSkip = c.closeBrace.trivia().size();
        emitToken(c.closeBrace);
        emitToken(c.emptySemi);
        return;
    }

    emitToken(c.openBrace);

    depth++;
    for (auto* member : c.members) {
        if (formatEnabled && !atLineStart && !hasLeadingNewline(member->getFirstToken())) {
            forceNewline();
        }
        nextIsPrimary = true;
        member->visit(*this);
    }

    nextIsPrimary = true;
    emitEndTokenTrivia(c.closeBrace);

    depth--;

    breakBeforeClose(c.closeBrace);

    nextIsPrimary = true;
    emitToken(c.closeBrace);
    emitToken(c.emptySemi);
}

void FormatPrinter::handle(const DistConstraintListSyntax& dc) {
    emitToken(dc.dist);
    emitToken(dc.openBrace);

    depth++;
    for (const auto& elem : dc.items.elems()) {
        if (elem.isNode()) {
            if (formatEnabled && !atLineStart && !hasLeadingNewline(elem.node()->getFirstToken())) {
                forceNewline();
            }
            nextLineKind = LineMetadata::Kind::DistItem;
            nextIsPrimary = true;
            elem.node()->visit(*this);
        }
        else if (elem.isToken()) {
            emitToken(elem.token());
        }
    }

    depth--;
    breakBeforeClose(dc.closeBrace);
    emitEndTokenTrivia(dc.closeBrace);
    nextIsPrimary = true;
    emitToken(dc.closeBrace);
}

void FormatPrinter::handle(const DistWeightSyntax& dw) {
    if (formatEnabled) {
        stripTrailingSpaces();
        auto srcSpaces = leadingWhitespaceLength(dw.op);
        output.append(std::max(srcSpaces, size_t{1}), ' ');
    }
    emitToken(dw.op);
    emitToken(dw.extraOp);
    if (formatEnabled) {
        output += ' ';
    }
    dw.expr->visit(*this);
}

void FormatPrinter::handle(const ImplicationConstraintSyntax& ic) {
    ic.left->visit(*this);
    beforeOperator = formatEnabled;
    emitToken(ic.arrow);
    ic.constraints->visit(*this);
}

bool FormatPrinter::isInsideControlCondition(const SyntaxNode& node) {
    for (auto* p = node.parent; p != nullptr; p = p->parent) {
        if (p->kind == SyntaxKind::ConditionalPredicate) {
            return true;
        }
        if (p->kind == SyntaxKind::ConditionalStatement ||
            p->kind == SyntaxKind::DoWhileStatement || p->kind == SyntaxKind::WaitStatement) {
            return false;
        }
    }
    return false;
}

} // namespace slang::format
