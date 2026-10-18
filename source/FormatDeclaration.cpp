// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Align.h"
#include "FormatPrinter.h"
#include "TokenHelper.h"

#include <tuple>
#include <utility>

#include <slang/parsing/TokenKind.h>
#include <slang/syntax/AllSyntax.h>
#include <slang/syntax/SyntaxNode.h>

using namespace slang;
using namespace slang::format;
using namespace slang::parsing;
using namespace slang::syntax;

namespace slang::format {

void FormatPrinter::handle(const BindDirectiveSyntax& bind) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(bind.attributes);

    emitToken(bind.bind);
    bind.target->visit(*this);
    if (bind.targetInstances != nullptr) {
        bind.targetInstances->visit(*this);
    }

    bind.instantiation->visit(*this);
}

void FormatPrinter::handle(const ContinuousAssignSyntax& assign) {
    nextLineKind = LineMetadata::Kind::Assignment;
    visitDefault(assign.attributes);

    emitToken(assign.assign);
    if (assign.strength != nullptr) {
        assign.strength->visit(*this);
    }

    if (assign.delay != nullptr) {
        assign.delay->visit(*this);
    }

    visitDefault(assign.assignments);

    emitToken(assign.semi);
}

void FormatPrinter::handle(const DPIExportSyntax& decl) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(decl.attributes);

    emitToken(decl.keyword);
    emitToken(decl.specString);
    emitToken(decl.c_identifier);
    emitToken(decl.equals);
    emitToken(decl.functionOrTask);
    emitToken(decl.name);
    emitToken(decl.semi);
}

void FormatPrinter::handle(const DPIImportSyntax& decl) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(decl.attributes);

    emitToken(decl.keyword);
    emitToken(decl.specString);
    emitToken(decl.property);
    emitToken(decl.c_identifier);
    emitToken(decl.equals);

    const auto& proto = decl.method->as<FunctionPrototypeSyntax>();
    emitToken(proto.keyword);
    visitDefault(proto.specifiers);
    emitToken(proto.lifetime);
    proto.returnType->visit(*this);
    proto.name->visit(*this);

    if (formatEnabled && proto.portList != nullptr && proto.portList->ports.size() >= 2) {
        emitToken(proto.portList->openParen);

        depth++;
        for (const auto& elem : proto.portList->ports.elems()) {
            if (elem.isNode()) {
                if (!atLineStart && !hasLeadingNewline(elem.node()->getFirstToken())) {
                    forceNewline();
                }
                nextIsPrimary = true;
                elem.node()->visit(*this);
            }
            else if (elem.isToken()) {
                emitToken(elem.token());
            }
        }

        depth--;
        breakBeforeClose(proto.portList->closeParen);
        nextIsPrimary = true;
        emitToken(proto.portList->closeParen);
    }
    else if (proto.portList != nullptr) {
        proto.portList->visit(*this);
    }

    emitToken(decl.semi);
}

void FormatPrinter::handle(const DataDeclarationSyntax& decl) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(decl.attributes);

    visitDefault(decl.modifiers);

    decl.type->visit(*this);
    visitDeclarators(decl.declarators);
    emitToken(decl.semi);
}

void FormatPrinter::handle(const DefParamSyntax& decl) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(decl.attributes);

    emitToken(decl.defparam);
    visitSeparatedList(decl.assignments);
    emitToken(decl.semi);
}

void FormatPrinter::handle(const DefaultClockingReferenceSyntax& decl) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(decl.attributes);

    emitToken(decl.defaultKeyword);
    emitToken(decl.clocking);
    emitToken(decl.name);
    emitToken(decl.semi);
}

void FormatPrinter::handle(const DefaultDisableDeclarationSyntax& decl) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(decl.attributes);

    emitToken(decl.defaultKeyword);
    emitToken(decl.disableKeyword);
    emitToken(decl.iffKeyword);
    decl.expr->visit(*this);
    emitToken(decl.semi);
}

void FormatPrinter::handle(const ElabSystemTaskSyntax& task) {
    visitDefault(task.attributes);

    emitToken(task.name);
    if (task.arguments != nullptr) {
        visitArgumentList(*task.arguments);
    }

    emitToken(task.semi);
}

void FormatPrinter::handle(const EnumTypeSyntax& e) {
    emitToken(e.keyword);
    if (e.baseType != nullptr) {
        e.baseType->visit(*this);
    }
    emitToken(e.openBrace);

    depth++;
    for (const auto& elem : e.members.elems()) {
        if (elem.isNode()) {
            if (formatEnabled && !atLineStart && !hasLeadingNewline(elem.node()->getFirstToken())) {
                forceNewline();
            }
            nextIsPrimary = true;
            elem.node()->visit(*this);
        }
        else if (elem.isToken()) {
            emitToken(elem.token());
        }
    }

    depth--;
    breakBeforeClose(e.closeBrace);
    emitEndTokenTrivia(e.closeBrace);
    nextIsPrimary = true;
    emitToken(e.closeBrace);

    visitDefault(e.dimensions);
}

void FormatPrinter::handle(const EqualsTypeClauseSyntax& clause) {
    beforeOperator = formatEnabled;
    emitToken(clause.equals);
    clause.type->visit(*this);
}

void FormatPrinter::handle(const EqualsValueClauseSyntax& clause) {
    beforeOperator = formatEnabled;
    emitToken(clause.equals);
    clause.expr->visit(*this);
}

void FormatPrinter::handle(const ForwardTypedefDeclarationSyntax& decl) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(decl.attributes);

    emitToken(decl.typedefKeyword);
    if (decl.typeRestriction != nullptr) {
        decl.typeRestriction->visit(*this);
    }

    emitToken(decl.name);
    emitToken(decl.semi);
}

void FormatPrinter::handle(const GenvarDeclarationSyntax& decl) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(decl.attributes);

    emitToken(decl.keyword);
    visitSeparatedList(decl.identifiers);
    emitToken(decl.semi);
}

void FormatPrinter::handle(const LetDeclarationSyntax& decl) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(decl.attributes);

    emitToken(decl.let);
    emitToken(decl.identifier);
    if (decl.portList != nullptr) {
        decl.portList->visit(*this);
    }

    beforeOperator = formatEnabled;
    emitToken(decl.equals);
    decl.expr->visit(*this);
    emitToken(decl.semi);
}

void FormatPrinter::handle(const NetAliasSyntax& alias) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(alias.attributes);

    emitToken(alias.keyword);
    for (const auto& elem : alias.nets.elems()) {
        if (elem.isNode()) {
            elem.node()->visit(*this);
        }
        else if (elem.isToken()) {
            beforeOperator = formatEnabled;
            emitToken(elem.token());
        }
    }
    emitToken(alias.semi);
}

void FormatPrinter::handle(const NetDeclarationSyntax& decl) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(decl.attributes);

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

void FormatPrinter::handle(const NetTypeDeclarationSyntax& decl) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(decl.attributes);

    emitToken(decl.keyword);
    decl.type->visit(*this);
    emitToken(decl.name);
    if (decl.withFunction != nullptr) {
        decl.withFunction->visit(*this);
    }

    emitToken(decl.semi);
}

void FormatPrinter::handle(const PackageExportAllDeclarationSyntax& decl) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(decl.attributes);

    emitToken(decl.keyword);
    emitToken(decl.star1);
    emitToken(decl.doubleColon);
    emitToken(decl.star2);
    emitToken(decl.semi);
}

void FormatPrinter::handle(const PackageExportDeclarationSyntax& decl) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(decl.attributes);

    emitToken(decl.keyword);
    visitSeparatedList(decl.items);
    emitToken(decl.semi);
}

void FormatPrinter::handle(const PackageImportDeclarationSyntax& decl) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(decl.attributes);

    emitToken(decl.keyword);
    visitSeparatedList(decl.items);
    emitToken(decl.semi);
}

void FormatPrinter::handle(const ParameterDeclarationStatementSyntax& decl) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(decl.attributes);

    decl.parameter->visit(*this);
    emitToken(decl.semi);
}

void FormatPrinter::handle(const PortDeclarationSyntax& decl) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(decl.attributes);

    decl.header->visit(*this);
    visitDeclarators(decl.declarators);
    emitToken(decl.semi);
}

void FormatPrinter::handle(const StructUnionMemberSyntax& member) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(member.attributes);
    emitToken(member.randomQualifier);
    member.type->visit(*this);
    visitDeclarators(member.declarators);
    emitToken(member.semi);
}

void FormatPrinter::handle(const StructUnionTypeSyntax& s) {
    emitToken(s.keyword);
    emitToken(s.taggedOrSoft);
    emitToken(s.packed);
    emitToken(s.signing);
    emitToken(s.openBrace);

    visitMembers(s.members, s.closeBrace, nullptr);

    visitDefault(s.dimensions);
}

void FormatPrinter::handle(const TimeUnitsDeclarationSyntax& decl) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(decl.attributes);

    emitToken(decl.keyword);
    emitToken(decl.time);
    if (decl.divider != nullptr) {
        decl.divider->visit(*this);
    }

    emitToken(decl.semi);
}

void FormatPrinter::handle(const TypedefDeclarationSyntax& decl) {
    visitDefault(decl.attributes);

    nextIsPrimary = true;
    emitToken(decl.typedefKeyword);
    decl.type->visit(*this);
    emitToken(decl.name);
    visitDefault(decl.dimensions);

    emitToken(decl.semi);
}

void FormatPrinter::handle(const UserDefinedNetDeclarationSyntax& decl) {
    nextLineKind = LineMetadata::Kind::Declaration;
    visitDefault(decl.attributes);

    emitToken(decl.netType);
    decl.delay->visit(*this);
    visitDeclarators(decl.declarators);
    emitToken(decl.semi);
}

void FormatPrinter::handle(const VariableDimensionSyntax& dim) {
    auto saved = std::pair{inPackedDimension, inUnpackedDimension};
    if (lastTokenKind == TokenKind::Identifier) {
        inUnpackedDimension = true;
    }
    else if (lastTokenKind == TokenKind::CloseBracket) {
        inPackedDimension = afterPackedDimCloseBracket;
        inUnpackedDimension = afterUnpackedDimCloseBracket;
    }
    else {
        inPackedDimension = true;
    }
    visitDefault(dim);
    std::tie(inPackedDimension, inUnpackedDimension) = saved;
}

void FormatPrinter::visitDeclarators(const SeparatedSyntaxList<DeclaratorSyntax>& list) {
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

} // namespace slang::format
