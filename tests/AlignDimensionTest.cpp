// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "slang/format/Format.h"
#include "slang/format/Style.h"
#include "slang/format/styles/Default.h"
#include "slang/util/Dedent.h"

#include <gtest/gtest.h>

using namespace slang::format;
using slang::dedent;

TEST(AlignConsecutivePackedDimensions, AcrossComments) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;
    style.AlignConsecutivePackedDimensions.AcrossComments = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, AcrossEmptyLines) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;
    style.AlignConsecutivePackedDimensions.AcrossEmptyLines = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, AcrossParameterPortList) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;
    style.AlignConsecutivePackedDimensions.AcrossParameterPortList = true;
    style.ParameterPortListIndentWidth = 2;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, AlignColon) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;
    style.AlignConsecutivePackedDimensions.AlignColon = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, AlignColonPadLeft) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;
    style.AlignConsecutivePackedDimensions.AlignColon = true;
    style.AlignConsecutivePackedDimensions.PadLeft = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, AlignColonPadRight) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;
    style.AlignConsecutivePackedDimensions.AlignColon = true;
    style.AlignConsecutivePackedDimensions.PadRight = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, Aligns) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, BareTypeNarrowerThanDimensions) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, BareTypeWiderThanDimensions) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, CommentBreaksGroup) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, DirectionKeywords) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;
    style.ParameterPortListIndentWidth = 2;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, Disabled) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, EmptyLineBreaksGroup) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, FormatOff) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, MultipleDimensions) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;
    style.AlignConsecutivePackedDimensions.PadLeft = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, PadLeft) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;
    style.AlignConsecutivePackedDimensions.PadLeft = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, PadLeftRequiresEnabled) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.PadLeft = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, PadLeftTakesPrecedence) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;
    style.AlignConsecutivePackedDimensions.PadLeft = true;
    style.AlignConsecutivePackedDimensions.PadRight = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, PadLeftWithDeclarationAlignment) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutiveDeclarations.Enabled = true;
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;
    style.AlignConsecutivePackedDimensions.PadLeft = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, PadRight) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;
    style.AlignConsecutivePackedDimensions.PadRight = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, SingleLine) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;

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

TEST(AlignConsecutivePackedDimensions, SingleNumberDimensions) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;
    style.AlignConsecutivePackedDimensions.PadLeft = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, SkipsBareTypes) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePackedDimensions, WithDeclarationAlignment) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutiveDeclarations.Enabled = true;
    style.AlignConsecutivePackedDimensions = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;

    // clang-format off
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
    // clang-format on
}

TEST(RangeSpacing, MatchSourceNormalizesMultiSpaceRange) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::MatchSource;

    // clang-format off
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
    // clang-format on
}

TEST(RangeSpacing, MatchSourcePreservesPortListAdjacentRange) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::MatchSource;

    // clang-format off
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
    // clang-format on
}

TEST(RangeSpacing, MatchSourceRangeExcessActivatesPadding) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::MatchSource;

    // clang-format off
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
    // clang-format on
}
