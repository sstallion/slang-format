// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Rewriter.h"
#include "Style.h"
#include "SyntaxHelper.h"

#include <memory>
#include <span>

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

} // namespace slang::format
