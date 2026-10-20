// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Align.h"
#include "FormatPrinter.h"
#include "SyntaxHelper.h"
#include "TokenHelper.h"
#include "slang/format/Style.h"

#include <algorithm>

#include <slang/syntax/AllSyntax.h>
#include <slang/syntax/SyntaxKind.h>
#include <slang/syntax/SyntaxNode.h>
#include <slang/syntax/SyntaxVisitor.h>
#include <slang/util/Util.h>

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

} // namespace

namespace slang::format {

void FormatPrinter::handle(const CaseGenerateSyntax& cg) {
    visitDefault(cg.attributes);

    emitToken(cg.keyword);

    normalizeSpaceBefore(cg.openParen, style.SpaceBeforeParens.ControlStatements, true);

    emitToken(cg.openParen);
    cg.condition->visit(*this);
    emitToken(cg.closeParen);

    visitMembers(cg.items, cg.endCase, nullptr);
}

void FormatPrinter::handle(const CheckerDeclarationSyntax& c) {
    visitDefault(c.attributes);

    emitToken(c.keyword);
    emitToken(c.name);
    if (c.portList != nullptr) {
        c.portList->visit(*this);
    }

    emitToken(c.semi);
    visitMembers(c.members, c.end, c.endBlockName);
}

void FormatPrinter::handle(const ClockingDeclarationSyntax& c) {
    visitDefault(c.attributes);

    emitToken(c.globalOrDefault);
    emitToken(c.clocking);
    emitToken(c.blockName);
    emitToken(c.at);
    c.event->visit(*this);
    emitToken(c.semi);
    visitMembers(c.items, c.endClocking, c.endBlockName);
}

void FormatPrinter::handle(const CompilationUnitSyntax& cu) {
    for (auto* member : cu.members) {
        nextIsPrimary = true;
        member->visit(*this);
    }

    nextIsPrimary = true;
    emitToken(cu.endOfFile);
}

void FormatPrinter::handle(const ConfigDeclarationSyntax& c) {
    visitDefault(c.attributes);

    emitToken(c.config);
    emitToken(c.name);
    emitToken(c.semi1);

    depth++;
    for (auto* param : c.localparams) {
        nextIsPrimary = true;
        param->visit(*this);
    }

    nextIsPrimary = true;
    emitToken(c.design);
    visitDefault(c.topCells);

    emitToken(c.semi2);

    for (auto* rule : c.rules) {
        nextIsPrimary = true;
        rule->visit(*this);
    }

    nextIsPrimary = true;
    emitEndTokenTrivia(c.endconfig);

    depth--;
    nextIsPrimary = true;
    emitToken(c.endconfig);
    if (c.blockName != nullptr) {
        c.blockName->visit(*this);
    }
}

void FormatPrinter::handle(const FunctionDeclarationSyntax& f) {
    const bool shouldBreak = (f.kind == SyntaxKind::FunctionDeclaration &&
                              style.BreakBeforeFunction) ||
                             (f.kind == SyntaxKind::TaskDeclaration && style.BreakBeforeTask);

    if (shouldBreak) {
        insertBlankLineBefore(f.getFirstToken());
    }

    visitDefault(f.attributes);
    f.prototype->visit(*this);
    emitToken(f.semi);
    visitMembers(f.items, f.end, f.endBlockName);
}

void FormatPrinter::handle(const GenerateBlockSyntax& b) {
    visitDefault(b.attributes);
    if (b.label != nullptr) {
        b.label->visit(*this);
    }
    emitToken(b.begin);
    if (b.beginName != nullptr) {
        b.beginName->visit(*this);
    }
    visitMembers(b.members, b.end, b.endName);
}

void FormatPrinter::handle(const GenerateRegionSyntax& r) {
    visitDefault(r.attributes);
    emitToken(r.keyword);
    visitMembers(r.members, r.endgenerate, nullptr);
}

void FormatPrinter::handle(const HierarchyInstantiationSyntax& inst) {
    visitDefault(inst.attributes);

    emitToken(inst.type);

    if (formatEnabled && inst.parameters != nullptr) {
        bool hasNamedParams = false;
        for (const auto* p : inst.parameters->parameters) {
            if (p->kind == SyntaxKind::NamedParamAssignment) {
                hasNamedParams = true;
                break;
            }
        }
        bool portsExpand = false;
        for (const auto* i : inst.instances) {
            if (i->connections.size() > 1) {
                portsExpand = true;
                break;
            }
        }
        expandNamedParams = hasNamedParams && portsExpand;
        inst.parameters->visit(*this);
        expandNamedParams = false;
    }
    else if (inst.parameters != nullptr) {
        inst.parameters->visit(*this);
    }

    visitDefault(inst.instances);

    emitToken(inst.semi);
}

void FormatPrinter::handle(const IfGenerateSyntax& ig) {
    visitDefault(ig.attributes);

    emitToken(ig.keyword);

    normalizeSpaceBefore(ig.openParen, style.SpaceBeforeParens.ControlStatements, true);

    emitToken(ig.openParen);
    ig.condition->visit(*this);
    emitToken(ig.closeParen);
    ig.block->visit(*this);

    if (ig.elseClause != nullptr) {
        nextIsPrimary = true;
        emitToken(ig.elseClause->elseKeyword);
        ig.elseClause->clause->visit(*this);
    }
}

void FormatPrinter::handle(const LoopGenerateSyntax& lg) {
    visitDefault(lg.attributes);

    emitToken(lg.keyword);

    normalizeSpaceBefore(lg.openParen, style.SpaceBeforeParens.ControlStatements, true);

    emitToken(lg.openParen);
    emitToken(lg.genvar);
    emitToken(lg.identifier);
    beforeOperator = formatEnabled;
    emitToken(lg.equals);
    lg.initialExpr->visit(*this);
    emitToken(lg.semi1);
    lg.stopExpr->visit(*this);
    emitToken(lg.semi2);
    lg.iterationExpr->visit(*this);
    emitToken(lg.closeParen);
    lg.block->visit(*this);
}

void FormatPrinter::handle(const ModuleDeclarationSyntax& module) {
    nextIsPrimary = true;
    module.header->visit(*this);

    depth++;
    const MemberSyntax* prevMember{nullptr};
    for (auto* member : module.members) {
        auto firstToken = member->getFirstToken();
        const bool compact = shouldCompactTimeUnits(prevMember, member);

        if (style.OneStatementPerLine && formatEnabled && !hasLeadingNewline(firstToken)) {
            if (!compact) {
                forceNewline();
            }
        }
        else if (compact && formatEnabled && !hasLeadingComment(firstToken)) {
            triviaSkip = firstToken.trivia().size();
        }

        nextIsPrimary = true;
        member->visit(*this);
        prevMember = member;
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

void FormatPrinter::handle(const PathDeclarationSyntax& pd) {
    visitDefault(pd.attributes);

    pd.desc->visit(*this);
    beforeOperator = formatEnabled;
    emitToken(pd.equals);
    emitToken(pd.openParen);
    visitSeparatedList(pd.delays);
    emitToken(pd.closeParen);
    emitToken(pd.semi);
}

void FormatPrinter::handle(const PathDescriptionSyntax& pd) {
    emitToken(pd.openParen);
    emitToken(pd.edgeIdentifier);
    visitSeparatedList(pd.inputs);
    emitToken(pd.polarityOperator);
    beforeOperator = formatEnabled;
    emitToken(pd.pathOperator);
    pd.suffix->visit(*this);
    emitToken(pd.closeParen);
}

void FormatPrinter::handle(const ProceduralBlockSyntax& proc) {
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
    visitDefault(proc.attributes);

    emitToken(proc.keyword);

    if (isAlwaysBlockKind(proc.kind)) {
        normalizeSpaceBefore(proc.statement->getFirstToken(), style.SpaceAfterAlways, true);
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

void FormatPrinter::handle(const PropertyDeclarationSyntax& decl) {
    visitDefault(decl.attributes);

    emitToken(decl.keyword);
    emitToken(decl.name);
    if (decl.portList != nullptr) {
        decl.portList->visit(*this);
    }

    emitToken(decl.semi);

    depth++;
    for (auto* var : decl.variables) {
        nextIsPrimary = true;
        var->visit(*this);
    }

    nextIsPrimary = true;
    decl.propertySpec->visit(*this);
    emitToken(decl.optionalSemi);

    nextIsPrimary = true;
    emitEndTokenTrivia(decl.end);

    depth--;
    nextIsPrimary = true;
    emitToken(decl.end);
    if (decl.endBlockName != nullptr) {
        decl.endBlockName->visit(*this);
    }
}

void FormatPrinter::handle(const SequenceDeclarationSyntax& decl) {
    visitDefault(decl.attributes);

    emitToken(decl.keyword);
    emitToken(decl.name);
    if (decl.portList != nullptr) {
        decl.portList->visit(*this);
    }

    emitToken(decl.semi);

    depth++;
    for (auto* var : decl.variables) {
        nextIsPrimary = true;
        var->visit(*this);
    }

    nextIsPrimary = true;
    decl.seqExpr->visit(*this);
    emitToken(decl.optionalSemi);

    nextIsPrimary = true;
    emitEndTokenTrivia(decl.end);

    depth--;
    nextIsPrimary = true;
    emitToken(decl.end);
    if (decl.endBlockName != nullptr) {
        decl.endBlockName->visit(*this);
    }
}

void FormatPrinter::handle(const SpecifyBlockSyntax& s) {
    if (style.BreakBeforeSpecifyBlock) {
        insertBlankLineBefore(s.getFirstToken());
    }

    visitDefault(s.attributes);
    emitToken(s.specify);
    visitMembers(s.items, s.endspecify, nullptr);
}

void FormatPrinter::handle(const SpecparamDeclaratorSyntax& sp) {
    emitToken(sp.name);
    beforeOperator = formatEnabled;
    emitToken(sp.equals);
    emitToken(sp.openParen);
    sp.value1->visit(*this);
    emitToken(sp.comma);
    if (sp.value2 != nullptr) {
        sp.value2->visit(*this);
    }
    emitToken(sp.closeParen);
}

void FormatPrinter::handle(const UdpDeclarationSyntax& u) {
    visitDefault(u.attributes);

    emitToken(u.primitive);
    emitToken(u.name);
    u.portList->visit(*this);

    depth++;
    for (size_t i = 0; i < u.body->portDecls.getChildCount(); i++) {
        if (auto* node = u.body->portDecls.childNode(i)) {
            nextIsPrimary = true;
            node->visit(*this);
        }
        else if (auto tok = u.body->portDecls.childToken(i)) {
            emitToken(tok);
        }
    }

    if (u.body->initialStmt != nullptr) {
        depth--;
        nextIsPrimary = true;
        u.body->initialStmt->visit(*this);
        depth++;
    }

    nextIsPrimary = true;
    emitToken(u.body->table);

    visitMembers(u.body->entries, u.body->endtable, nullptr);

    nextIsPrimary = true;
    emitEndTokenTrivia(u.endprimitive);

    depth--;
    nextIsPrimary = true;
    emitToken(u.endprimitive);
    if (u.endBlockName != nullptr) {
        u.endBlockName->visit(*this);
    }
}

void FormatPrinter::handle(const UdpEdgeFieldSyntax& ue) {
    emitToken(ue.openParen);
    suppressSeparator = formatEnabled;
    output += ue.first.rawText();
    output += ue.second.rawText();
    lastTokenKind = ue.second.kind;
    suppressSeparator = formatEnabled;
    emitToken(ue.closeParen);
}

void FormatPrinter::handle(const UdpEntrySyntax& ue) {
    for (auto* field : ue.inputs) {
        field->visit(*this);
        if (formatEnabled) {
            output += ' ';
        }
    }

    ensureSingleSpace();
    emitToken(ue.colon1);
    if (formatEnabled) {
        output += ' ';
    }

    if (ue.current != nullptr) {
        ue.current->visit(*this);
    }

    if (ue.colon2) {
        ensureSingleSpace();
        emitToken(ue.colon2);
        if (formatEnabled) {
            output += ' ';
        }
    }

    if (ue.next != nullptr) {
        ue.next->visit(*this);
    }

    emitToken(ue.semi);
}

void FormatPrinter::handle(const UdpInitialStmtSyntax& u) {
    emitToken(u.initial);
    emitToken(u.name);
    beforeOperator = formatEnabled;
    emitToken(u.equals);
    after.operatorToken = formatEnabled;
    u.value->visit(*this);
    emitToken(u.semi);
}

bool FormatPrinter::shouldBreakAfterProcedural(BlockBreakStyle brkStyle,
                                               const StatementSyntax& body,
                                               bool oneStatementPerLine) {
    if (brkStyle == BlockBreakStyle::Never) {
        return false;
    }

    if (hasLeadingNewline(body.getFirstToken())) {
        return false;
    }

    if (BlockStatementSyntax::isKind(body.kind)) {
        const auto& block = body.as<BlockStatementSyntax>();
        const Token checkTok = block.items.empty() ? block.end
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

bool FormatPrinter::shouldBreakBeforeProcedural(BlockBreakStyle brkStyle,
                                                const StatementSyntax& stmt,
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

bool FormatPrinter::shouldCompactTimeUnits(const MemberSyntax* prev,
                                           const MemberSyntax* curr) const {
    if (!style.CompactTimeUnits || prev == nullptr) {
        return false;
    }

    return prev->kind == SyntaxKind::TimeUnitsDeclaration &&
           curr->kind == SyntaxKind::TimeUnitsDeclaration;
}

void FormatPrinter::visitProceduralBody(const StatementSyntax& stmt, BlockBreakStyle brkStyle) {
    const auto* body = &stmt;
    if (stmt.kind == SyntaxKind::TimingControlStatement) {
        const auto& tcs = stmt.as<TimingControlStatementSyntax>();
        if (tcs.label != nullptr) {
            tcs.label->visit(*this);
        }

        visitDefault(tcs.attributes);

        tcs.timingControl->visit(*this);
        body = tcs.statement;

        if (isEventControl(*tcs.timingControl) && !output.empty() && output.back() == ')') {
            normalizeSpaceBefore(body->getFirstToken(), style.SpaceAfterParens.EventControls, true);
        }
    }

    const bool forceBreak = shouldBreakAfterProcedural(brkStyle, *body, style.OneStatementPerLine);
    if (forceBreak && !hasLeadingNewline(body->getFirstToken())) {
        forceNewline();
        nextIsPrimary = true;
    }

    visitBody(*body);
}

} // namespace slang::format
