// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "slang/format/Format.h"
#include "slang/format/Style.h"
#include "slang/format/styles/Default.h"
#include "slang/util/Dedent.h"

#include <gtest/gtest.h>

using namespace slang::format;
using slang::dedent;

TEST(AlignConsecutiveDeclarations, AcrossComments) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutiveDeclarations.Enabled = true;
    style.AlignConsecutiveDeclarations.AcrossComments = true;
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a;
          logic [7:0] b;
          // comment
          logic [15:0] c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic        a;
          logic [7:0]  b;
          // comment
          logic [15:0] c;
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveDeclarations, AcrossEmptyLines) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutiveDeclarations.Enabled = true;
    style.AlignConsecutiveDeclarations.AcrossEmptyLines = true;
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a;
          logic [7:0] b;

          logic [15:0] c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic        a;
          logic [7:0]  b;

          logic [15:0] c;
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveDeclarations, AcrossParameterPortList) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutiveDeclarations.Enabled = true;
    style.AlignConsecutiveDeclarations.AcrossParameterPortList = true;
    style.AlignConsecutivePackedDimensions = {};
    style.ParameterPortListIndentWidth = 2;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo #(
          parameter N = 4,
          parameter logic [7:0] M = 8
        ) (
          input a,
          input logic [7:0] b
        );
          logic c;
          logic [15:0] d;
        endmodule
    )"), style), dedent(R"(
        module foo #(
          parameter             N = 4,
          parameter logic [7:0] M = 8
        ) (
          input                 a,
          input logic [7:0]     b
        );
          logic                 c;
          logic [15:0]          d;
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveDeclarations, Aligns) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutiveDeclarations.Enabled = true;
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a;
          logic [7:0] b;
          logic [15:0] c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic        a;
          logic [7:0]  b;
          logic [15:0] c;
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveDeclarations, CommentBreaksGroup) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutiveDeclarations.Enabled = true;
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a;
          logic [7:0] b;
          // comment
          logic [15:0] c;
          logic d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic       a;
          logic [7:0] b;
          // comment
          logic [15:0] c;
          logic        d;
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveDeclarations, ContinuationLines) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutiveDeclarations.Enabled = true;
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          localparam [1:0] STATE_0 = 1,
            STATE_1 = 2,
            STATE_2 = 3;
        endmodule
    )"), style), dedent(R"(
        module foo;
          localparam [1:0] STATE_0 = 1,
                           STATE_1 = 2,
                           STATE_2 = 3;
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveDeclarations, DirectionKeywords) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutiveDeclarations.Enabled = true;
    style.AlignConsecutivePackedDimensions = {};
    style.ParameterPortListIndentWidth = 2;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo(
          input a,
          input logic [7:0] b,
          output c
        );
        endmodule
    )"), style), dedent(R"(
        module foo (
          input             a,
          input logic [7:0] b,
          output            c
        );
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveDeclarations, Disabled) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a;
          logic [7:0] b;
          logic [15:0] c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic a;
          logic [7:0] b;
          logic [15:0] c;
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveDeclarations, EmptyLineBreaksGroup) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutiveDeclarations.Enabled = true;
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a;
          logic [7:0] b;

          logic [15:0] c;
          logic d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic       a;
          logic [7:0] b;

          logic [15:0] c;
          logic        d;
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveDeclarations, FormatOff) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutiveDeclarations.Enabled = true;
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          // slang-format off
          logic a;
          logic [7:0] b;
          // slang-format on
          logic c;
          logic [15:0] d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off
          logic a;
          logic [7:0] b;
          // slang-format on
          logic        c;
          logic [15:0] d;
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveDeclarations, ParameterDeclarations) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutiveDeclarations.Enabled = true;
    style.AlignConsecutivePackedDimensions = {};
    style.ParameterPortListIndentWidth = 2;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo #(
          parameter N = 4,
          parameter logic [7:0] M = 8
        );
        endmodule
    )"), style), dedent(R"(
        module foo #(
          parameter             N = 4,
          parameter logic [7:0] M = 8
        );
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveDeclarations, ParameterDimensionRightJustified) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutiveDeclarations.Enabled = true;
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        package foo;
          parameter int unsigned DEFAULT_WIDTH = 32;
          localparam int unsigned ADDR_W = 12;
          localparam bit [7:0] MAGIC = 8'hA5;
          localparam string BANNER = "tort";
        endpackage
    )"), style), dedent(R"(
        package foo;
          parameter  int unsigned DEFAULT_WIDTH = 32;
          localparam int unsigned ADDR_W        = 12;
          localparam bit    [7:0] MAGIC         = 8'hA5;
          localparam string       BANNER        = "tort";
        endpackage
    )"));
    // clang-format on
}

TEST(AlignConsecutiveDeclarations, ParameterPortListBreaksGroup) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutiveDeclarations.Enabled = true;
    style.AlignConsecutivePackedDimensions = {};
    style.ParameterPortListIndentWidth = 2;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo #(
          parameter N = 4,
          parameter logic [7:0] M = 8
        ) (
          input a,
          input logic [7:0] b
        );
          logic c;
          logic [15:0] d;
        endmodule
    )"), style), dedent(R"(
        module foo #(
          parameter             N = 4,
          parameter logic [7:0] M = 8
        ) (
          input             a,
          input logic [7:0] b
        );
          logic        c;
          logic [15:0] d;
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveDeclarations, SingleLine) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutiveDeclarations.Enabled = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] a;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] a;
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveDeclarations, StatementBreaksGroup) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutiveDeclarations.Enabled = true;
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a;
          logic [7:0] b;
          assign x = 1;
          logic [15:0] c;
          logic d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic       a;
          logic [7:0] b;
          assign x = 1;
          logic [15:0] c;
          logic        d;
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveTimingControls, AcrossComments) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveTimingControls = {};
    style.AlignConsecutiveTimingControls.Enabled = true;
    style.AlignConsecutiveTimingControls.AcrossComments = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always #5 clk_i = ~clk_i;
        // comment
        always #20 dclk_i = ~dclk_i;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always #5  clk_i  = ~clk_i;
          // comment
          always #20 dclk_i = ~dclk_i;
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveTimingControls, AcrossEmptyLines) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveTimingControls = {};
    style.AlignConsecutiveTimingControls.Enabled = true;
    style.AlignConsecutiveTimingControls.AcrossEmptyLines = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always #5 clk_i = ~clk_i;

        always #20 dclk_i = ~dclk_i;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always #5  clk_i  = ~clk_i;

          always #20 dclk_i = ~dclk_i;
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveTimingControls, Aligns) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveTimingControls = {};
    style.AlignConsecutiveTimingControls.Enabled = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always #5 clk_i = ~clk_i;
        always #20 dclk_i = ~dclk_i;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always #5  clk_i  = ~clk_i;
          always #20 dclk_i = ~dclk_i;
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveTimingControls, CommentBreaksGroup) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveTimingControls = {};
    style.AlignConsecutiveTimingControls.Enabled = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always #5 clk_i = ~clk_i;
        // comment
        always #20 dclk_i = ~dclk_i;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always #5 clk_i = ~clk_i;
          // comment
          always #20 dclk_i = ~dclk_i;
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveTimingControls, Disabled) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveTimingControls = {};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always #5 clk_i = ~clk_i;
        always #20 dclk_i = ~dclk_i;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always #5 clk_i = ~clk_i;
          always #20 dclk_i = ~dclk_i;
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveTimingControls, EmptyLineBreaksGroup) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveTimingControls = {};
    style.AlignConsecutiveTimingControls.Enabled = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always #5 clk_i = ~clk_i;

        always #20 dclk_i = ~dclk_i;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always #5 clk_i = ~clk_i;

          always #20 dclk_i = ~dclk_i;
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveTimingControls, ParenthesizedDelay) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveTimingControls = {};
    style.AlignConsecutiveTimingControls.Enabled = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always #(CLK_PERIOD / 2) clk_i = ~clk_i;
        always #20 dclk_i = ~dclk_i;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always #(CLK_PERIOD / 2) clk_i  = ~clk_i;
          always #20               dclk_i = ~dclk_i;
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveTimingControls, SingleLine) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveTimingControls = {};
    style.AlignConsecutiveTimingControls.Enabled = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always #5 clk_i = ~clk_i;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always #5 clk_i = ~clk_i;
        endmodule
    )"));
    // clang-format on
}
