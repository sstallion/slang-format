// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Align.h"
#include "FormatPrinter.h"
#include "TokenHelper.h"

#include <slang/syntax/AllSyntax.h>
#include <slang/syntax/SyntaxNode.h>
#include <slang/util/Util.h>

using namespace slang;
using namespace slang::format;
using namespace slang::parsing;
using namespace slang::syntax;

namespace slang::format {

void FormatPrinter::handle(const AnsiPortListSyntax& p) {
    normalizeSpaceBefore(p.openParen, style.SpaceBeforeParens.PortList, true);
    emitToken(p.openParen);

    depth++;
    visitSeparatedList(p.ports);

    emitPortListEndTrivia(p.closeParen);

    depth--;
    emitPortListClose(p.closeParen);
}

void FormatPrinter::handle(const FunctionPortListSyntax& pl) {
    if (!formatEnabled || pl.ports.empty()) {
        visitDefault(pl);
        return;
    }

    emitToken(pl.openParen);

    if (!style.BinPackParameters) {
        depth++;
        visitWrappedSeparatedList(pl.ports);
        depth--;
        breakBeforeClose(pl.closeParen);
        nextIsPrimary = true;
        emitToken(pl.closeParen);
        return;
    }

    const auto limit = style.ColumnLimit;
    const auto savedOutput = output.size();

    visitSeparatedList(pl.ports);

    const auto endCol = output.size() - lineStart;
    if (limit == 0 || endCol + 2 <= limit) {
        emitToken(pl.closeParen);
        return;
    }

    output.resize(savedOutput);
    atLineStart = (savedOutput == lineStart);
    forceNewline();
    emitBinPackedWrapped(pl.ports, limit);
    emitToken(pl.closeParen);
}

void FormatPrinter::handle(const HierarchicalInstanceSyntax& inst) {
    if (inst.decl != nullptr) {
        if (formatEnabled && !atLineStart && !output.empty() && output.back() == ')') {
            output += ' ';
        }
        inst.decl->visit(*this);
    }

    normalizeSpaceBefore(inst.openParen, style.SpaceBeforeParens.PortList, true);
    emitToken(inst.openParen);

    depth++;
    const bool wrapConnections = !style.BinPackPortConnections && formatEnabled &&
                                 inst.connections.size() > 1;
    if (wrapConnections) {
        visitWrappedSeparatedList(inst.connections);
    }
    else {
        visitSeparatedList(inst.connections);
    }

    depth--;
    if (wrapConnections) {
        breakBeforeClose(inst.closeParen);
    }

    emitPortListEndTrivia(inst.closeParen);
    emitPortListClose(inst.closeParen);
}

void FormatPrinter::handle(const ModportItemSyntax& m) {
    emitToken(m.name);
    if (formatEnabled && !output.empty()) {
        stripTrailingSpaces();
    }

    auto& p = *m.ports;
    emitToken(p.openParen);

    depth++;
    visitSeparatedList(p.ports);

    emitPortListEndTrivia(p.closeParen);

    depth--;
    emitPortListClose(p.closeParen);
}

void FormatPrinter::handle(const NamedParamAssignmentSyntax& param) {
    nextLineKind = LineMetadata::Kind::PortConnection;
    visitDefault(param);
}

void FormatPrinter::handle(const NamedPortConnectionSyntax& conn) {
    nextLineKind = LineMetadata::Kind::PortConnection;
    visitDefault(conn);
}

void FormatPrinter::handle(const NonAnsiPortListSyntax& p) {
    normalizeSpaceBefore(p.openParen, style.SpaceBeforeParens.PortList, true);
    emitToken(p.openParen);

    depth++;
    const bool wrapPorts = !style.BinPackPortConnections && formatEnabled && p.ports.size() > 1;
    if (wrapPorts) {
        visitWrappedSeparatedList(p.ports);
    }
    else {
        visitSeparatedList(p.ports);
    }

    depth--;
    if (wrapPorts) {
        breakBeforeClose(p.closeParen);
    }

    emitPortListEndTrivia(p.closeParen);
    emitPortListClose(p.closeParen);
}

void FormatPrinter::handle(const ParameterPortListSyntax& p) {
    normalizeSpaceBefore(p.hash, style.SpaceBeforeParens.ParameterList, true);
    emitToken(p.hash);
    emitToken(p.openParen);

    depth++;
    visitSeparatedList(p.declarations);

    emitPortListEndTrivia(p.closeParen);

    depth--;
    emitPortListClose(p.closeParen);
}

void FormatPrinter::handle(const ParameterValueAssignmentSyntax& p) {
    normalizeSpaceBefore(p.hash, style.SpaceBeforeParens.ParameterList, true);
    emitToken(p.hash);
    emitToken(p.openParen);

    depth++;
    if (expandNamedParams) {
        visitWrappedSeparatedList(p.parameters);
    }
    else {
        visitSeparatedList(p.parameters);
    }

    emitPortListEndTrivia(p.closeParen);

    depth--;
    if (expandNamedParams) {
        breakBeforeClose(p.closeParen);
    }

    emitPortListClose(p.closeParen);
}

void FormatPrinter::handle(const WildcardPortListSyntax& p) {
    normalizeSpaceBefore(p.openParen, style.SpaceBeforeParens.PortList, true);
    emitToken(p.openParen);
    emitToken(p.dot);
    emitToken(p.star);
    emitToken(p.closeParen);
}

void FormatPrinter::emitBinPackedWrapped(const SeparatedSyntaxList<FunctionPortBaseSyntax>& ports,
                                         size_t limit) {
    for (const auto& elem : ports.elems()) {
        if (elem.isNode()) {
            const auto colBefore = output.size() - lineStart;
            const auto w = flatWidth(*elem.node());
            if (colBefore > 0 && limit > 0 && colBefore + w + 2 > limit) {
                forceNewline();
            }
            triviaSkip = elem.node()->getFirstToken().trivia().size();
            elem.node()->visit(*this);
        }
        else if (elem.isToken()) {
            emitToken(elem.token());
        }
    }
}

void FormatPrinter::emitPortListClose(Token closeParen) {
    nextIsPrimary = true;
    nextLineKind = LineMetadata::Kind::PortListBoundary;
    emitToken(closeParen);
}

void FormatPrinter::emitPortListEndTrivia(Token closeParen) {
    portItemNextIndent = true;
    nextIsPrimary = true;
    emitEndTokenTrivia(closeParen);
    portItemNextIndent = false;
}

} // namespace slang::format
