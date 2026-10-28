// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "Style.h"
#include "TestHelper.h"

#include <gtest/gtest.h>

using namespace slang::format;

// clang-format off
TEST(AlignTrailingComments, AcrossComments) {
    Style style;
    style.AlignTrailingComments = {.Enabled = true, .AcrossComments = true};

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
}

TEST(AlignTrailingComments, AcrossEmptyLines) {
    Style style;
    style.AlignTrailingComments = {.Enabled = true, .AcrossEmptyLines = true};

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
}

TEST(AlignTrailingComments, AcrossParameterPortList) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignTrailingComments = {.Enabled = true, .AcrossParameterPortList = true};
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
    Style style;
    style.AlignTrailingComments = {.Enabled = true};

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
}

TEST(AlignTrailingComments, CommentBreaksGroup) {
    Style style;
    style.AlignConsecutiveAssignments = {};
    style.AlignTrailingComments = {.Enabled = true};

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
}

TEST(AlignTrailingComments, Disabled) {
    const Style style;

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
}

TEST(AlignTrailingComments, EmptyLineBreaksGroup) {
    Style style;
    style.AlignConsecutiveAssignments = {};
    style.AlignTrailingComments = {.Enabled = true};

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
}

TEST(AlignTrailingComments, FormatOff) {
    Style style;
    style.AlignTrailingComments = {.Enabled = true};

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
}

TEST(AlignTrailingComments, LineWithoutCommentBreaksGroup) {
    Style style;
    style.AlignTrailingComments = {.Enabled = true};

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
}

TEST(AlignTrailingComments, MixedLineAndBlockComments) {
    Style style;
    style.AlignTrailingComments = {.Enabled = true};

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
}

TEST(AlignTrailingComments, SingleLine) {
    Style style;
    style.AlignTrailingComments = {.Enabled = true};

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
}

TEST(AlignTrailingComments, WithAssignmentAlignment) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true, .MaxPadding = 0};
    style.AlignConsecutiveDeclarations = {};
    style.AlignTrailingComments = {.Enabled = true};

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
}
// clang-format on
