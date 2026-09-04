// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Rewriter.h"
#include "Style.h"
#include "SyntaxHelper.h"

#include <memory>
#include <span>

#include <slang/numeric/SVInt.h>
#include <slang/parsing/Token.h>
#include <slang/parsing/TokenKind.h>
#include <slang/syntax/AllSyntax.h>
#include <slang/syntax/SyntaxKind.h>
#include <slang/syntax/SyntaxNode.h>
#include <slang/syntax/SyntaxTree.h>
#include <slang/syntax/SyntaxVisitor.h>
#include <slang/util/SmallVector.h>

using namespace slang;
using namespace slang::syntax;
using namespace slang::format;

namespace {

/// Rewrites the syntax tree to insert begin/end around bare statement bodies.
class BeginEndInserter : public SyntaxRewriter<BeginEndInserter> {
public:
    explicit BeginEndInserter(const Style& style) : style(style) {}

    void handle(const ConditionalStatementSyntax& stmt) {
        if (style.InsertBeginEnd.ControlStatements) {
            wrapInBlock(*stmt.statement);
            if (stmt.elseClause != nullptr) {
                auto& clause = *stmt.elseClause->clause;
                if (clause.kind != SyntaxKind::ConditionalStatement) {
                    wrapInBlock(clause.as<StatementSyntax>());
                }
            }
        }

        visitDefault(stmt);
    }

    void handle(const LoopStatementSyntax& s) {
        wrapControlBody(*s.statement);
        visitDefault(s);
    }

    void handle(const ForLoopStatementSyntax& s) {
        wrapControlBody(*s.statement);
        visitDefault(s);
    }

    void handle(const ForeverStatementSyntax& s) {
        wrapControlBody(*s.statement);
        visitDefault(s);
    }

    void handle(const ForeachLoopStatementSyntax& s) {
        wrapControlBody(*s.statement);
        visitDefault(s);
    }

    void handle(const DoWhileStatementSyntax& s) {
        wrapControlBody(*s.statement);
        visitDefault(s);
    }

    void handle(const StandardCaseItemSyntax& item) {
        wrapCaseItemClause(*item.clause);
        visitDefault(item);
    }

    void handle(const DefaultCaseItemSyntax& item) {
        wrapCaseItemClause(*item.clause);
        visitDefault(item);
    }

    void handle(const ProceduralBlockSyntax& proc) {
        bool const wrapAlways = style.InsertBeginEnd.AlwaysStatements &&
                                isAlwaysBlockKind(proc.kind);
        bool const wrapInitial = style.InsertBeginEnd.InitialStatements &&
                                 isInitialBlockKind(proc.kind);
        if (wrapAlways || wrapInitial) {
            const StatementSyntax* body = proc.statement;
            if (body->kind == SyntaxKind::TimingControlStatement) {
                body = body->as<TimingControlStatementSyntax>().statement;
            }

            wrapInBlock(*body);
        }

        visitDefault(proc);
    }

private:
    const Style& style;

    void wrapInBlock(const StatementSyntax& body) {
        if (BlockStatementSyntax::isKind(body.kind)) {
            return;
        }

        auto* spaceTrivia = alloc.emplace<parsing::Trivia>(parsing::TriviaKind::Whitespace, " ");
        auto beginTok = makeToken(parsing::TokenKind::BeginKeyword, "begin",
                                  std::span<const parsing::Trivia>{spaceTrivia, 1});
        auto endTok = makeToken(parsing::TokenKind::EndKeyword, "end",
                                std::span<const parsing::Trivia>{spaceTrivia, 1});

        auto* bodyCopy = static_cast<StatementSyntax*>(deepClone(body, alloc));

        SmallVector<SyntaxNode*> itemsBuffer;
        itemsBuffer.push_back(bodyCopy);
        SyntaxList<SyntaxNode> const items(itemsBuffer.copy(alloc));

        auto& block = factory.blockStatement(SyntaxKind::SequentialBlockStatement, nullptr,
                                             SyntaxList<AttributeInstanceSyntax>(nullptr), beginTok,
                                             nullptr, items, endTok, nullptr);
        replace(body, block);
    }

    void wrapControlBody(const StatementSyntax& body) {
        if (style.InsertBeginEnd.ControlStatements) {
            wrapInBlock(body);
        }
    }

    void wrapCaseItemClause(const SyntaxNode& clause) {
        if (style.InsertBeginEnd.ControlStatements) {
            if (StatementSyntax::isKind(clause.kind) && clause.kind != SyntaxKind::EmptyStatement) {
                wrapInBlock(clause.as<StatementSyntax>());
            }
        }
    }
};

/// Rewrites event expression separators to use a consistent style.
class EventSeparatorRewriter : public SyntaxRewriter<EventSeparatorRewriter> {
public:
    explicit EventSeparatorRewriter(EventSeparatorStyle sepStyle) : sepStyle(sepStyle) {}

    void handle(const BinaryEventExpressionSyntax& expr) {
        auto const opKind = expr.operatorToken.kind;

        parsing::TokenKind targetKind{};
        std::string_view targetText;

        if (sepStyle == EventSeparatorStyle::Comma && opKind == parsing::TokenKind::OrKeyword) {
            targetKind = parsing::TokenKind::Comma;
            targetText = ",";
        }
        else if (sepStyle == EventSeparatorStyle::Or && opKind == parsing::TokenKind::Comma) {
            targetKind = parsing::TokenKind::OrKeyword;
            targetText = "or";
        }
        else {
            visitDefault(expr);
            return;
        }

        auto* newLeft = deepClone(*expr.left, alloc);
        auto* newRight = deepClone(*expr.right, alloc);

        std::span<const parsing::Trivia> trivia;
        if (targetKind == parsing::TokenKind::OrKeyword) {
            trivia = {&SingleSpace, 1};
        }

        auto newOp = makeToken(targetKind, targetText, trivia);

        auto& newExpr = factory.binaryEventExpression(*newLeft, newOp, *newRight);
        replace(expr, newExpr);
    }

private:
    EventSeparatorStyle sepStyle;
};

bool isPackedDimensionParent(SyntaxKind kind) {
    return IntegerTypeSyntax::isKind(kind) || ImplicitTypeSyntax::isKind(kind) ||
           StructUnionTypeSyntax::isKind(kind) || EnumTypeSyntax::isKind(kind);
}

bool isUnpackedDimensionParent(SyntaxKind kind) {
    return DeclaratorSyntax::isKind(kind);
}

/// Rewrites packed dimension ranges to enforce bound ordering.
class PackedDimensionRewriter : public SyntaxRewriter<PackedDimensionRewriter> {
public:
    explicit PackedDimensionRewriter(DimensionBoundsStyle boundsStyle) : boundsStyle(boundsStyle) {}

    void handle(const VariableDimensionSyntax& dim) {
        if (dim.specifier == nullptr ||
            dim.specifier->kind != SyntaxKind::RangeDimensionSpecifier) {
            visitDefault(dim);
            return;
        }

        if (dim.parent == nullptr || !isPackedDimensionParent(dim.parent->kind)) {
            visitDefault(dim);
            return;
        }

        auto const& rangeSpec = dim.specifier->as<RangeDimensionSpecifierSyntax>();
        if (rangeSpec.selector->kind != SyntaxKind::SimpleRangeSelect) {
            visitDefault(dim);
            return;
        }

        auto const& sel = rangeSpec.selector->as<RangeSelectSyntax>();
        if (sel.left->kind != SyntaxKind::IntegerLiteralExpression ||
            sel.right->kind != SyntaxKind::IntegerLiteralExpression) {
            visitDefault(dim);
            return;
        }

        auto const& leftLit = sel.left->as<LiteralExpressionSyntax>();
        auto const& rightLit = sel.right->as<LiteralExpressionSyntax>();

        auto leftVal = leftLit.literal.intValue();
        auto rightVal = rightLit.literal.intValue();

        if (leftVal.hasUnknown() || rightVal.hasUnknown()) {
            visitDefault(dim);
            return;
        }

        bool shouldSwap = false;
        if (boundsStyle == DimensionBoundsStyle::MSBFirst) {
            shouldSwap = static_cast<bool>(leftVal < rightVal);
        }
        else if (boundsStyle == DimensionBoundsStyle::LSBFirst) {
            shouldSwap = static_cast<bool>(leftVal > rightVal);
        }

        if (!shouldSwap) {
            visitDefault(dim);
            return;
        }

        auto* newLeft = deepClone(*sel.right, alloc);
        auto* newRight = deepClone(*sel.left, alloc);

        auto& newRange = factory.rangeSelect(sel.kind, *newLeft, sel.range, *newRight);
        replace(sel, newRange);
    }

private:
    DimensionBoundsStyle boundsStyle;
};

/// Rewrites unpacked dimension ranges to enforce bound ordering.
class UnpackedDimensionRewriter : public SyntaxRewriter<UnpackedDimensionRewriter> {
public:
    explicit UnpackedDimensionRewriter(DimensionBoundsStyle boundsStyle) :
        boundsStyle(boundsStyle) {}

    void handle(const VariableDimensionSyntax& dim) {
        if (dim.specifier == nullptr ||
            dim.specifier->kind != SyntaxKind::RangeDimensionSpecifier) {
            visitDefault(dim);
            return;
        }

        if (dim.parent == nullptr || !isUnpackedDimensionParent(dim.parent->kind)) {
            visitDefault(dim);
            return;
        }

        auto const& rangeSpec = dim.specifier->as<RangeDimensionSpecifierSyntax>();
        if (rangeSpec.selector->kind != SyntaxKind::SimpleRangeSelect) {
            visitDefault(dim);
            return;
        }

        auto const& sel = rangeSpec.selector->as<RangeSelectSyntax>();
        if (sel.left->kind != SyntaxKind::IntegerLiteralExpression ||
            sel.right->kind != SyntaxKind::IntegerLiteralExpression) {
            visitDefault(dim);
            return;
        }

        auto const& leftLit = sel.left->as<LiteralExpressionSyntax>();
        auto const& rightLit = sel.right->as<LiteralExpressionSyntax>();

        auto leftVal = leftLit.literal.intValue();
        auto rightVal = rightLit.literal.intValue();

        if (leftVal.hasUnknown() || rightVal.hasUnknown()) {
            visitDefault(dim);
            return;
        }

        bool shouldSwap = false;
        if (boundsStyle == DimensionBoundsStyle::MSBFirst) {
            shouldSwap = static_cast<bool>(leftVal < rightVal);
        }
        else if (boundsStyle == DimensionBoundsStyle::LSBFirst) {
            shouldSwap = static_cast<bool>(leftVal > rightVal);
        }

        if (!shouldSwap) {
            visitDefault(dim);
            return;
        }

        auto* newLeft = deepClone(*sel.right, alloc);
        auto* newRight = deepClone(*sel.left, alloc);

        auto& newRange = factory.rangeSelect(sel.kind, *newLeft, sel.range, *newRight);
        replace(sel, newRange);
    }

private:
    DimensionBoundsStyle boundsStyle;
};

} // namespace

namespace slang::format {

std::shared_ptr<SyntaxTree> applyBeginEndInsertion(std::shared_ptr<SyntaxTree> tree,
                                                   const Style& style) {
    if (!style.InsertBeginEnd.Enabled) {
        return tree;
    }

    for (;;) {
        BeginEndInserter inserter(style);
        auto newTree = inserter.transform(tree);
        if (newTree == tree) {
            break;
        }

        tree = newTree;
    }

    return tree;
}

std::shared_ptr<SyntaxTree> applyEventSeparator(std::shared_ptr<SyntaxTree> tree,
                                                const Style& style) {
    if (style.EventSeparator == EventSeparatorStyle::Preserve) {
        return tree;
    }

    for (;;) {
        EventSeparatorRewriter rewriter(style.EventSeparator);
        auto newTree = rewriter.transform(tree);
        if (newTree == tree) {
            break;
        }

        tree = newTree;
    }

    return tree;
}

std::shared_ptr<SyntaxTree> applyPackedDimensionBounds(std::shared_ptr<SyntaxTree> tree,
                                                       const Style& style) {
    if (style.PackedDimensionBounds == DimensionBoundsStyle::Preserve) {
        return tree;
    }

    PackedDimensionRewriter rewriter(style.PackedDimensionBounds);
    return rewriter.transform(tree);
}

std::shared_ptr<SyntaxTree> applyUnpackedDimensionBounds(std::shared_ptr<SyntaxTree> tree,
                                                         const Style& style) {
    if (style.UnpackedDimensionBounds == DimensionBoundsStyle::Preserve) {
        return tree;
    }

    UnpackedDimensionRewriter rewriter(style.UnpackedDimensionBounds);
    return rewriter.transform(tree);
}

} // namespace slang::format
