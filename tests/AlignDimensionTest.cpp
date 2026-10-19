// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "Style.h"
#include "TestHelper.h"

#include <gtest/gtest.h>

using namespace slang::format;

// clang-format off
TEST(AlignConsecutivePackedDimensions, AcrossComments) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.Enabled = true, .AcrossComments = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          bit [7:0] a;
          // comment
          logic [15:0] b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          bit   [7:0] a;
          // comment
          logic [15:0] b;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, AcrossEmptyLines) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.Enabled = true, .AcrossEmptyLines = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          bit [7:0] a;

          logic [15:0] b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          bit   [7:0] a;

          logic [15:0] b;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, AcrossParameterPortList) {
    Style style;
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.Enabled = true, .AcrossParameterPortList = true};
    style.ParameterPortListIndentWidth = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo #(
          parameter bit [7:0] M = 8,
          parameter logic [15:0] N = 16
        ) (
          input bit [7:0] a,
          input logic [31:0] b
        );
        endmodule
    )"), style), dedent(R"(
        module foo #(
          parameter bit   [7:0] M = 8,
          parameter logic [15:0] N = 16
        ) (
          input bit       [7:0] a,
          input logic     [31:0] b
        );
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, AlignColon) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.Enabled = true, .AlignColon = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] a;
          logic [15:12] b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [ 7:0] a;
          logic [15:12] b;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, AlignColonPadLeft) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {
        .Enabled = true, .AlignColon = true, .PadLeft = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] a;
          logic [15:12] b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [ 7: 0] a;
          logic [15:12] b;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, AlignColonPadRight) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {
        .Enabled = true, .AlignColon = true, .PadRight = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] a;
          logic [15:12] b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [ 7:0 ] a;
          logic [15:12] b;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, Aligns) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.Enabled = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          bit [7:0] a;
          logic [15:0] b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          bit   [7:0] a;
          logic [15:0] b;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, BareTypeNarrowerThanDimensions) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.Enabled = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          bit b;
          logic [7:0] a;
          logic [15:0] c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          bit b;
          logic [7:0] a;
          logic [15:0] c;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, BareTypeWiderThanDimensions) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.Enabled = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          integer a;
          logic [7:0] b;
          reg [3:0] c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          integer a;
          logic   [7:0] b;
          reg     [3:0] c;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, CommentBreaksGroup) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.Enabled = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] a;
          // comment
          logic [15:0] b;
          logic [31:0] c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] a;
          // comment
          logic [15:0] b;
          logic [31:0] c;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, DirectionKeywords) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.Enabled = true};
    style.ParameterPortListIndentWidth = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo(
          input bit [7:0] a,
          input logic [15:0] b
        );
        endmodule
    )"), style), dedent(R"(
        module foo (
          input bit   [7:0] a,
          input logic [15:0] b
        );
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, Disabled) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] a;
          logic [15:0] b;
          logic [31:0] c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] a;
          logic [15:0] b;
          logic [31:0] c;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, EmptyLineBreaksGroup) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.Enabled = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] a;

          logic [15:0] b;
          logic [31:0] c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] a;

          logic [15:0] b;
          logic [31:0] c;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, FormatOff) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.Enabled = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          // slang-format off
          bit [7:0] a;
          logic [15:0] b;
          // slang-format on
          bit [7:0] c;
          logic [31:0] d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off
          bit [7:0] a;
          logic [15:0] b;
          // slang-format on
          bit   [7:0] c;
          logic [31:0] d;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, MultipleDimensions) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.Enabled = true, .PadLeft = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0][3:0] a;
          logic [15:0][15:0] b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [ 7:0] [ 3:0] a;
          logic [15:0] [15:0] b;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, PadLeft) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.Enabled = true, .PadLeft = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] a;
          logic [15:0] b;
          logic [127:0] c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [  7:0] a;
          logic [ 15:0] b;
          logic [127:0] c;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, PadLeftRequiresEnabled) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.PadLeft = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] a;
          logic [15:0] b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] a;
          logic [15:0] b;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, PadLeftTakesPrecedence) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {
        .Enabled = true, .PadLeft = true, .PadRight = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] a;
          logic [15:0] b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [ 7:0] a;
          logic [15:0] b;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, PadLeftWithDeclarationAlignment) {
    Style style;
    style.AlignConsecutiveDeclarations = {.Enabled = true};
    style.AlignConsecutivePackedDimensions = {.Enabled = true, .PadLeft = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] a;
          logic [15:0] b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [ 7:0] a;
          logic [15:0] b;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, PadRight) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.Enabled = true, .PadRight = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] a;
          logic [15:0] b;
          logic [127:0] c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0  ] a;
          logic [15:0 ] b;
          logic [127:0] c;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, SingleLine) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.Enabled = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] a;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] a;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, SingleNumberDimensions) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.Enabled = true, .PadLeft = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [8] a;
          logic [16] b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [ 8] a;
          logic [16] b;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, SkipsBareTypes) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.Enabled = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] a;
          logic b;
          logic [15:0] c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] a;
          logic b;
          logic [15:0] c;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, WithDeclarationAlignment) {
    Style style;
    style.AlignConsecutiveDeclarations = {.Enabled = true};
    style.AlignConsecutivePackedDimensions = {.Enabled = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          bit [7:0] a;
          logic [15:0] b;
          logic c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          bit   [7:0]  a;
          logic [15:0] b;
          logic        c;
        endmodule
    )"));
}

TEST(RangeSpacing, MatchSourceNormalizesMultiSpaceRange) {
    Style style;
    style.RangeSpacing = RangeSpacingStyle::MatchSource;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [7   :   0] x;
        logic [15 : 0] y;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7 : 0] x;
          logic [15 : 0] y;
        endmodule
    )"));
}

TEST(RangeSpacing, MatchSourcePreservesPortListAdjacentRange) {
    Style style;
    style.RangeSpacing = RangeSpacingStyle::MatchSource;

    EXPECT_EQ(reformat(dedent(R"(
        module foo #(
            parameter int AW   = 12,
            parameter int DW   = 32
        ) (
            input  logic [AW-1   :    0] addr,
            input  logic [DW-1    :    0] wdata,
            output logic [DW-1    :    0] rdata
        );
        endmodule
    )"), style), dedent(R"(
        module foo #(
            parameter int AW = 12,
            parameter int DW = 32
        ) (
            input logic [AW-1   :    0] addr,
            input logic [DW-1    :    0] wdata,
            output logic [DW-1    :    0] rdata
        );
        endmodule
    )"));
}

TEST(RangeSpacing, MatchSourceRangeExcessActivatesPadding) {
    Style style;
    style.RangeSpacing = RangeSpacingStyle::MatchSource;

    EXPECT_EQ(reformat(dedent(R"(
        module foo #(
            parameter int W = 8
        ) (
            input wire [W-1:0] a,
            input wire [W-1     :    0] b,
            output wire [W-1:0] sum
        );
        endmodule
    )"), style), dedent(R"(
        module foo #(
            parameter int W = 8
        ) (
            input  wire [  W-1:0] a,
            input  wire [W-1 : 0] b,
            output wire [  W-1:0] sum
        );
        endmodule
    )"));
}
// clang-format on
