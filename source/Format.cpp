// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Align.h"
#include "Format.h"
#include "FormatPrinter.h"
#include "Rewriter.h"
#include "slang/format/Style.h"

#include <cstddef>
#include <istream>
#include <iterator>
#include <regex>
#include <string>
#include <string_view>

#include <slang/syntax/SyntaxTree.h>
#include <slang/text/SourceManager.h>

using namespace slang;
using namespace slang::format;
using namespace slang::syntax;

namespace {

std::string applyOneLineFormatOff(const std::string& output, const std::regex& re) {
    std::string result;
    result.reserve(output.size());
    size_t pos{0};
    while (pos < output.size()) {
        size_t const lineEnd = output.find('\n', pos);
        size_t const nextPos = (lineEnd == std::string::npos) ? output.size() : lineEnd + 1;
        const std::string_view line = std::string_view(output).substr(pos, nextPos - pos);

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

LineEndingStyle resolveLineEnding(std::string_view text, LineEndingStyle lineEnding) {
    if (lineEnding == LineEndingStyle::LF || lineEnding == LineEndingStyle::CRLF) {
        return lineEnding;
    }

    size_t crlfCount = 0;
    size_t lfCount = 0;
    for (size_t i = 0; i < text.size(); i++) {
        if (text[i] == '\n') {
            if (i > 0 && text[i - 1] == '\r') {
                crlfCount++;
            }
            else {
                lfCount++;
            }
        }
    }

    if (lineEnding == LineEndingStyle::DeriveCRLF) {
        return lfCount > crlfCount ? LineEndingStyle::LF : LineEndingStyle::CRLF;
    }
    return crlfCount > lfCount ? LineEndingStyle::CRLF : LineEndingStyle::LF;
}

std::string stripCarriageReturns(std::string_view text) {
    std::string result;
    result.reserve(text.size());
    for (auto c : text) {
        if (c != '\r') {
            result += c;
        }
    }
    return result;
}

std::string applyLineEnding(const std::string& text) {
    std::string result;
    result.reserve(text.size() * 2);
    for (auto c : text) {
        if (c == '\n') {
            result += '\r';
        }
        result += c;
    }
    return result;
}

} // namespace

namespace slang::format {

std::string reformat(std::string_view text, const Style& style) {
    auto resolved = resolveLineEnding(text, style.LineEnding);

    std::string cleaned;
    auto parseInput = text;
    if (text.find('\r') != std::string_view::npos) {
        cleaned = stripCarriageReturns(text);
        parseInput = cleaned;
    }

    SourceManager sm;
    auto tree = SyntaxTree::fromText(parseInput, sm, "source");
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

    if (resolved == LineEndingStyle::CRLF) {
        result = applyLineEnding(result);
    }

    return result;
}

std::string reformat(std::istream& stream, const Style& style) {
    const std::string text{std::istreambuf_iterator<char>(stream), {}};
    return reformat(text, style);
}

} // namespace slang::format
