// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "AlignTrailingComment.h"
#include "Align.h"
#include "AlignClassifier.h"
#include "AlignGroup.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

using namespace slang::format;

namespace {

using Kind = LineMetadata::Kind;

} // namespace

namespace slang::format {

void alignGroupTrailingComments(std::string& result, const std::vector<std::string_view>& lines,
                                const std::vector<LineInfo>& infos, GroupRange range,
                                unsigned minSpaces) {
    size_t maxCommentCol = 0;
    size_t commentCount = 0;
    for (auto i = range.start; i < range.end; i++) {
        if (infos[i].kind == Kind::Comment || infos[i].kind == Kind::Empty) {
            continue;
        }

        auto commentPos = findTrailingComment(lines[i]);
        if (commentPos == std::string_view::npos) {
            continue;
        }

        commentCount++;

        auto codeEnd = trimTrailingSpaces(lines[i], commentPos);

        maxCommentCol = std::max(maxCommentCol, codeEnd + minSpaces);
    }

    for (auto i = range.start; i < range.end; i++) {
        if (infos[i].kind == Kind::Comment || infos[i].kind == Kind::Empty) {
            result.append(lines[i]);
            result += '\n';
            continue;
        }

        auto commentPos = findTrailingComment(lines[i]);
        if (commentPos == std::string_view::npos || commentCount < 2) {
            result.append(lines[i]);
            result += '\n';
            continue;
        }

        auto codeEnd = trimTrailingSpaces(lines[i], commentPos);

        result.append(lines[i].substr(0, codeEnd));
        if (maxCommentCol > codeEnd) {
            result.append(maxCommentCol - codeEnd, ' ');
        }
        else {
            result.append(minSpaces, ' ');
        }
        result.append(lines[i].substr(commentPos));
        result += '\n';
    }
}

} // namespace slang::format
