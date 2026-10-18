// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Align.h"
#include "FormatPrinter.h"
#include "Style.h"
#include "TokenHelper.h"

#include <algorithm>

#include <slang/syntax/AllSyntax.h>
#include <slang/syntax/SyntaxNode.h>
#include <slang/util/Util.h>

using namespace slang;
using namespace slang::format;
using namespace slang::parsing;
using namespace slang::syntax;

namespace slang::format {

void FormatPrinter::handle(const AssignmentPatternItemSyntax& item) {
    item.key->visit(*this);
    emitToken(item.colon);
    ensureSpace();
    item.expr->visit(*this);
}

void FormatPrinter::handle(const AttributeInstanceSyntax& a) {
    emitToken(a.openParen);
    emitToken(a.openStar);
    ensureSpace();
    visitSeparatedList(a.specs);
    ensureSpace();
    emitToken(a.closeStar);
    emitToken(a.closeParen);
    nextIsPrimary = true;
}

void FormatPrinter::handle(const BinaryExpressionSyntax& expr) {
    expr.left->visit(*this);
    beforeOperator = formatEnabled;
    emitToken(expr.operatorToken);

    visitDefault(expr.attributes);

    expr.right->visit(*this);
}

void FormatPrinter::handle(const BinaryPropertyExprSyntax& expr) {
    expr.left->visit(*this);
    beforeOperator = formatEnabled;
    emitToken(expr.op);
    expr.right->visit(*this);
}

void FormatPrinter::handle(const BinarySequenceExprSyntax& expr) {
    expr.left->visit(*this);
    beforeOperator = formatEnabled;
    emitToken(expr.op);
    expr.right->visit(*this);
}

void FormatPrinter::handle(const ConditionalExpressionSyntax& expr) {
    expr.predicate->visit(*this);
    beforeOperator = formatEnabled;
    emitToken(expr.question);

    visitDefault(expr.attributes);

    expr.left->visit(*this);
    beforeOperator = formatEnabled;
    emitToken(expr.colon);
    expr.right->visit(*this);
}

void FormatPrinter::handle(const DelayedSequenceElementSyntax& elem) {
    emitToken(elem.doubleHash);
    if (elem.delayVal != nullptr) {
        elem.delayVal->visit(*this);
    }
    emitToken(elem.openBracket);
    emitToken(elem.op);
    if (elem.range != nullptr) {
        elem.range->visit(*this);
    }
    emitToken(elem.closeBracket);
    ensureSpace();
    elem.expr->visit(*this);
}

void FormatPrinter::handle(const InvocationExpressionSyntax& expr) {
    expr.left->visit(*this);
    visitDefault(expr.attributes);

    if (expr.arguments != nullptr) {
        visitArgumentList(*expr.arguments);
    }
}

void FormatPrinter::handle(const MultipleConcatenationExpressionSyntax& expr) {
    emitToken(expr.openBrace);
    expr.expression->visit(*this);
    suppressSeparator = formatEnabled;
    expr.concatenation->visit(*this);
    emitToken(expr.closeBrace);
}

void FormatPrinter::handle(const NewClassExpressionSyntax& expr) {
    expr.scopedNew->visit(*this);
    if (expr.argList != nullptr) {
        visitArgumentList(*expr.argList);
    }
}

void FormatPrinter::handle(const RangeSelectSyntax& sel) {
    sel.left->visit(*this);
    emitRangeOperator(sel.range, sel.right->getFirstToken());
    sel.right->visit(*this);
}

void FormatPrinter::handle(const SequenceRepetitionSyntax& rep) {
    ensureSpace();

    emitToken(rep.openBracket);
    emitToken(rep.op);
    if (rep.selector != nullptr) {
        ensureSpace();

        if (RangeSelectSyntax::isKind(rep.selector->kind)) {
            const auto& range = rep.selector->as<RangeSelectSyntax>();
            range.left->visit(*this);
            emitToken(range.range);
            ensureSpace();
            range.right->visit(*this);
        }
        else {
            rep.selector->visit(*this);
        }
    }
    emitToken(rep.closeBracket);
}

void FormatPrinter::handle(const ValueRangeExpressionSyntax& vr) {
    emitToken(vr.openBracket);
    vr.left->visit(*this);
    emitRangeOperator(vr.op, vr.right->getFirstToken());
    vr.right->visit(*this);
    emitToken(vr.closeBracket);
}

size_t FormatPrinter::argumentListFlatWidth(const SeparatedSyntaxList<ArgumentSyntax>& params) {
    size_t totalWidth = 0;
    for (size_t i = 0; i < params.size(); i++) {
        auto w = flatWidth(*params[i]);
        if (w == 0) {
            return 0;
        }

        totalWidth += w;
        if (i + 1 < params.size()) {
            totalWidth += 2;
        }
    }
    return totalWidth;
}

void FormatPrinter::emitRangeOperator(Token op, Token rightFirst) { // NOLINT
    size_t leftSpaces = 0;
    size_t rightSpaces = 0;
    switch (style.RangeSpacing) {
        case RangeSpacingStyle::Always:
            leftSpaces = 1;
            rightSpaces = 1;
            break;
        case RangeSpacingStyle::MatchSource:
            leftSpaces = leadingWhitespaceLength(op);
            rightSpaces = leadingWhitespaceLength(rightFirst);
            currentLineMeta.rangeSpacings.push_back(
                {.sourceLeftSpaces = static_cast<unsigned>(leftSpaces),
                 .sourceRightSpaces = static_cast<unsigned>(rightSpaces)});
            break;
        case RangeSpacingStyle::Never:
            break;
        case RangeSpacingStyle::Preserve:
            leftSpaces = std::min(leadingWhitespaceLength(op), size_t{1});
            rightSpaces = std::min(leadingWhitespaceLength(rightFirst), size_t{1});
            break;
    }

    if (formatEnabled && leftSpaces != 0) {
        stripTrailingSpaces();
        output.append(leftSpaces, ' ');
    }
    skipSpacingCapture = true;
    beforeOperator = false;
    emitToken(op);
    if (formatEnabled && rightSpaces != 0 && !atLineStart) {
        output.append(rightSpaces, ' ');
    }
    skipSpacingCapture = true;
    beforeOperator = false;
}

void FormatPrinter::emitWrappedArgs(const SeparatedSyntaxList<ArgumentSyntax>& params) {
    for (size_t i = 0; i < params.size(); i++) {
        const auto* arg = params[i];

        if (i > 0 && shouldBreakBeforeArg(*arg, i + 1 == params.size()) &&
            !hasLeadingNewline(arg->getFirstToken())) {
            forceNewline();
        }

        arg->visit(*this);

        auto elems = params.elems();
        const auto sepIdx = (i * 2) + 1;
        if (sepIdx < elems.size() && elems[sepIdx].isToken()) {
            emitToken(elems[sepIdx].token());
        }
    }
}

bool FormatPrinter::shouldBreakBeforeArg(const ArgumentSyntax& arg, bool isLast) const {
    auto w = flatWidth(arg);
    if (w == 0) {
        return true;
    }

    if (!style.BinPackArguments) {
        return true;
    }

    auto currentCol = output.size() - lineStart;
    const size_t separator = style.SpaceAfterComma ? 1 : 0;
    const size_t needed = separator + w + (isLast ? 1 : 2);
    return style.ColumnLimit > 0 && currentCol + needed > style.ColumnLimit;
}

void FormatPrinter::visitArgumentList(const ArgumentListSyntax& argList) {
    auto savedAlignColumn = parenAlignColumn;
    parenAlignColumn = 0;

    auto mode = style.AlignAfterOpenParen;
    emitToken(argList.openParen);

    const auto& params = argList.parameters;
    if (params.empty()) {
        parenAlignColumn = savedAlignColumn;
        emitToken(argList.closeParen);
        return;
    }

    if (mode == BracketAlignmentStyle::DontAlign || !formatEnabled || style.ColumnLimit == 0) {
        visitDefault(params);
        parenAlignColumn = savedAlignColumn;
        emitToken(argList.closeParen);
        return;
    }

    auto totalWidth = argumentListFlatWidth(params);
    auto currentCol = output.size() - lineStart;
    const bool fitsOneLine = totalWidth > 0 && (currentCol + totalWidth + 1 <= style.ColumnLimit);

    if (fitsOneLine) {
        visitDefault(params);
    }
    else {
        if (mode == BracketAlignmentStyle::Align) {
            parenAlignColumn = static_cast<unsigned>(output.size() - lineStart);
        }
        else {
            forceNewline();
        }

        emitWrappedArgs(params);
    }

    if (mode == BracketAlignmentStyle::BlockIndent && !fitsOneLine) {
        forceNewline();
        nextIsPrimary = true;
        spaceBeforeCloseParenPending = false;
    }

    parenAlignColumn = savedAlignColumn;
    emitToken(argList.closeParen);
}

} // namespace slang::format
