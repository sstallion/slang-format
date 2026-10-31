// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "slang/format/Format.h"
#include "slang/format/Style.h"
#include "slang/format/styles/Default.h"
#include "slang/util/Dedent.h"

#include <gtest/gtest.h>

using namespace slang::format;
using slang::dedent;

TEST(AlignTrailingComments, AcrossComments) {
    auto style = getDefaultStyle();
    style.AlignTrailingComments = {};
    style.AlignTrailingComments.Enabled = true;
    style.AlignTrailingComments.AcrossComments = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1; // first
          assign longname = 2; // second
          // standalone
          assign a = 3; // third
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;         // first
          assign longname = 2;  // second
          // standalone
          assign a = 3;         // third
        endmodule
    )"));
    // clang-format on
}

TEST(AlignTrailingComments, AcrossEmptyLines) {
    auto style = getDefaultStyle();
    style.AlignTrailingComments = {};
    style.AlignTrailingComments.Enabled = true;
    style.AlignTrailingComments.AcrossEmptyLines = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1; // first
          assign longname = 2; // second

          assign a = 3; // third
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;         // first
          assign longname = 2;  // second

          assign a = 3;         // third
        endmodule
    )"));
    // clang-format on
}

TEST(AlignTrailingComments, AcrossParameterPortList) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignTrailingComments = {};
    style.AlignTrailingComments.Enabled = true;
    style.AlignTrailingComments.AcrossParameterPortList = true;
    style.ParameterPortListIndentWidth = 2;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo #(
          parameter N = 4 // width
        ) (
          input a // port a
        );
          assign x = 1; // body
        endmodule
    )"), style), dedent(R"(
        module foo #(
          parameter N = 4  // width
        ) (
          input a          // port a
        );
          assign x = 1;    // body
        endmodule
    )"));
    // clang-format on
}

TEST(AlignTrailingComments, Aligns) {
    auto style = getDefaultStyle();
    style.AlignTrailingComments = {};
    style.AlignTrailingComments.Enabled = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1; // first
          assign longname = 2; // second
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          assign x = 1;         // first
          assign longname = 2;  // second
        endmodule
    )"));
    // clang-format on
}

TEST(AlignTrailingComments, CommentBreaksGroup) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignTrailingComments = {};
    style.AlignTrailingComments.Enabled = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1; // first
          assign longname = 2; // second
          // standalone
          assign a = 3; // third
          assign bc = 4; // fourth
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          assign x = 1;         // first
          assign longname = 2;  // second
          // standalone
          assign a = 3;   // third
          assign bc = 4;  // fourth
        endmodule
    )"));
    // clang-format on
}

TEST(AlignTrailingComments, Disabled) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1; // first
          assign longname = 2; // second
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          assign x = 1;  // first
          assign longname = 2;  // second
        endmodule
    )"));
    // clang-format on
}

TEST(AlignTrailingComments, EmptyLineBreaksGroup) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignTrailingComments = {};
    style.AlignTrailingComments.Enabled = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1; // first
          assign longname = 2; // second

          assign a = 3; // third
          assign bc = 4; // fourth
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          assign x = 1;         // first
          assign longname = 2;  // second

          assign a = 3;   // third
          assign bc = 4;  // fourth
        endmodule
    )"));
    // clang-format on
}

TEST(AlignTrailingComments, FormatOff) {
    auto style = getDefaultStyle();
    style.AlignTrailingComments = {};
    style.AlignTrailingComments.Enabled = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          // slang-format off
          assign x = 1; // first
          assign longname = 2; // second
          // slang-format on
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          // slang-format off
          assign x = 1; // first
          assign longname = 2; // second
          // slang-format on
        endmodule
    )"));
    // clang-format on
}

TEST(AlignTrailingComments, LineWithoutCommentBreaksGroup) {
    auto style = getDefaultStyle();
    style.AlignTrailingComments = {};
    style.AlignTrailingComments.Enabled = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1; // first
          assign longname = 2;
          assign a = 3; // third
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          assign x = 1;  // first
          assign longname = 2;
          assign a = 3;  // third
        endmodule
    )"));
    // clang-format on
}

TEST(AlignTrailingComments, MixedLineAndBlockComments) {
    auto style = getDefaultStyle();
    style.AlignTrailingComments = {};
    style.AlignTrailingComments.Enabled = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1; // line comment
          assign longname = 2; /* block comment */
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          assign x = 1;         // line comment
          assign longname = 2;  /* block comment */
        endmodule
    )"));
    // clang-format on
}

TEST(AlignTrailingComments, SingleLine) {
    auto style = getDefaultStyle();
    style.AlignTrailingComments = {};
    style.AlignTrailingComments.Enabled = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1; // only one
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          assign x = 1;  // only one
        endmodule
    )"));
    // clang-format on
}

TEST(AlignTrailingComments, WithAssignmentAlignment) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveAssignments.MaxPadding = 0;
    style.AlignConsecutiveDeclarations = {};
    style.AlignTrailingComments = {};
    style.AlignTrailingComments.Enabled = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1; // first
          assign longname = 2; // second
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          assign x        = 1;  // first
          assign longname = 2;  // second
        endmodule
    )"));
    // clang-format on
}
