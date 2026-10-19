// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "Style.h"
#include "TestHelper.h"

#include <gtest/gtest.h>

using namespace slang::format;

// clang-format off
TEST(AlignConsecutiveAssignments, AcrossComments) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true, .AcrossComments = true, .MaxPadding = 0};
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a = 1;
          logic [7:0] b = 2;
          // comment
          logic [15:0] c = 3;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic a        = 1;
          logic [7:0] b  = 2;
          // comment
          logic [15:0] c = 3;
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, AcrossEmptyLines) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true, .AcrossEmptyLines = true, .MaxPadding = 0};
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a = 1;
          logic [7:0] b = 2;

          logic [15:0] c = 3;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic a        = 1;
          logic [7:0] b  = 2;

          logic [15:0] c = 3;
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, AcrossEmptyLinesAndComments) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true, .AcrossComments = true, .AcrossEmptyLines = true};
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a = 1;

          // comment
          logic [7:0] b = 2;
          logic [15:0] c = 3;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic a        = 1;

          // comment
          logic [7:0] b  = 2;
          logic [15:0] c = 3;
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, AcrossParameterPortList) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true, .AcrossParameterPortList = true};
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.ParameterPortListIndentWidth = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo #(
          parameter N = 4,
          parameter logic [7:0] M = 8
        ) (
          input a,
          input logic [7:0] b
        );
          logic c = 1;
          logic [15:0] d = 2;
        endmodule
    )"), style), dedent(R"(
        module foo #(
          parameter N             = 4,
          parameter logic [7:0] M = 8
        ) (
          input a,
          input logic [7:0] b
        );
          logic c                 = 1;
          logic [15:0] d          = 2;
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, Aligns) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true};
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a = 1;
          logic [7:0] b = 2;
          logic [15:0] c = 3;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic a        = 1;
          logic [7:0] b  = 2;
          logic [15:0] c = 3;
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, BeginEndBreaksGroup) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true, .AcrossComments = true, .AcrossEmptyLines = true};
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_comb begin
            a = 1;
            longname = 2;
          end

          always_comb begin
            x = 1;
            verylongname = 2;
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            a        = 1;
            longname = 2;
          end

          always_comb begin
            x            = 1;
            verylongname = 2;
          end
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, BitSelectLHS) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true};
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_comb begin
            x[0] = 1;
            longname[3:0] = 2;
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            x[0]          = 1;
            longname[3:0] = 2;
          end
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, BlockingAssignments) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true};
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_comb begin
            a = 1;
            longname = 2;
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            a        = 1;
            longname = 2;
          end
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, CaseStatementDoesNotBreakGroup) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true};
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_comb begin
            x = 1;
            case (state)
              STATE_A: a = 1;
              STATE_B: a = 2;
            endcase
            longname = 2;
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            x        = 1;
            case (state)
              STATE_A: a = 1;
              STATE_B: a = 2;
            endcase
            longname = 2;
          end
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, CommentBreaksGroup) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true};
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a = 1;
          logic [7:0] b = 2;
          // comment
          logic [15:0] c = 3;
          logic d = 4;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic a       = 1;
          logic [7:0] b = 2;
          // comment
          logic [15:0] c = 3;
          logic d        = 4;
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, CompoundAssignments) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true};
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_comb begin
            a += 1;
            longname -= 2;
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            a        += 1;
            longname -= 2;
          end
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, ContinuationLines) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true};
    style.AlignConsecutiveDeclarations = {};
    style.ContinuationIndentWidth = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          localparam [1:0] A = 1,
            LONG_NAME = 2,
            B = 3;
        endmodule
    )"), style), dedent(R"(
        module foo;
          localparam [1:0] A = 1,
            LONG_NAME        = 2,
            B                = 3;
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, DeclarationWithoutInitializer) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true};
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a = 1;
          logic [7:0] b;
          logic [15:0] c = 3;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic a        = 1;
          logic [7:0] b;
          logic [15:0] c = 3;
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, DepthChangeBreaksGroup) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true, .AcrossComments = true, .AcrossEmptyLines = true};
    style.AlignConsecutiveDeclarations = {};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_comb begin
            a = 1;
            longname = 2;
            begin
              x = 1;
              verylongname = 2;
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            a        = 1;
            longname = 2;
            begin
              x            = 1;
              verylongname = 2;
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveAssignments, Disabled) {
    Style style;
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a = 1;
          logic [7:0] b = 2;
          logic [15:0] c = 3;
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          logic a = 1;
          logic [7:0] b = 2;
          logic [15:0] c = 3;
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, EmptyLineBreaksGroup) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true};
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a = 1;
          logic [7:0] b = 2;

          logic [15:0] c = 3;
          logic d = 4;
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          logic a       = 1;
          logic [7:0] b = 2;

          logic [15:0] c = 3;
          logic d        = 4;
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, ForkJoinDoesNotCrossAlign) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true};
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            fork
              begin
                enable_i <= 1'b1;
                @(posedge clk_i);
              end
              begin
                data_i <= 1'b0;
                @(posedge dclk_i);
              end
            join
          end
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          initial begin
            fork
              begin
                enable_i <= 1'b1;
                @(posedge clk_i);
              end
              begin
                data_i <= 1'b0;
                @(posedge dclk_i);
              end
            join
          end
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, FormatOff) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true};
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          // slang-format off
          logic a = 1;
          logic [15:0] b = 2;
          // slang-format on
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          // slang-format off
          logic a = 1;
          logic [15:0] b = 2;
          // slang-format on
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, HierarchicalLHS) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true};
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_comb begin
            x.y = 1;
            x.longname = 2;
          end
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          always_comb begin
            x.y        = 1;
            x.longname = 2;
          end
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, IfStatementDoesNotBreakGroup) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true};
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_comb begin
            x = 1;
            if (cond) begin
              y = 2;
            end
            else begin
              y = 3;
            end
            longname = 4;
          end
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          always_comb begin
            x        = 1;
            if (cond) begin
              y = 2;
            end
            else begin
              y = 3;
            end
            longname = 4;
          end
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, MixedDeclarationsAndAssignments) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true};
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a = 1;
          assign x = 2;
          logic [7:0] b = 3;
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          logic a       = 1;
          assign x      = 2;
          logic [7:0] b = 3;
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, NonblockingAssignments) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true};
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_ff @(posedge clk) begin
            a <= 1;
            longname <= 2;
          end
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          always_ff @(posedge clk) begin
            a        <= 1;
            longname <= 2;
          end
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, ShiftAssignments) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true};
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_comb begin
            a <<= 1;
            longname >>= 2;
          end
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          always_comb begin
            a        <<= 1;
            longname >>= 2;
          end
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, SingleLine) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true};
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a = 1;
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          logic a = 1;
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, WithDeclarationAlignment) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true};
    style.AlignConsecutiveDeclarations = {.Enabled = true};
    style.AlignConsecutivePackedDimensions = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a = 1;
          logic [7:0] b = 2;
          logic [15:0] c = 3;
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          logic        a = 1;
          logic [7:0]  b = 2;
          logic [15:0] c = 3;
        endmodule
    )"));
}

TEST(AlignConsecutivePortConnections, Aligns) {
    Style style;
    style.AlignConsecutivePortConnections = {.Enabled = true, .MaxPadding = 0};
    style.BinPackPortConnections = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar u_bar (
        .clk(clk),
        .rst_n(rst_n),
        .data_out(data_out)
        );
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          bar u_bar (
              .clk     (clk),
              .rst_n   (rst_n),
              .data_out(data_out)
          );
        endmodule
    )"));
}

TEST(AlignConsecutivePortConnections, Disabled) {
    Style style;
    style.AlignConsecutivePortConnections = {};
    style.BinPackPortConnections = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar u_bar (
        .clk(clk),
        .rst_n(rst_n),
        .req(req_int),
        .gnt(gnt_int)
        );
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          bar u_bar (
              .clk(clk),
              .rst_n(rst_n),
              .req(req_int),
              .gnt(gnt_int)
          );
        endmodule
    )"));
}

TEST(AlignConsecutivePortConnections, MaxPaddingExceeded) {
    Style style;
    style.AlignConsecutivePortConnections = {.Enabled = true, .MaxPadding = 2};
    style.BinPackPortConnections = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar u_bar (
        .clk(clk),
        .rst_n(rst_n),
        .data_out(data_out)
        );
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          bar u_bar (
              .clk(clk),
              .rst_n(rst_n),
              .data_out(data_out)
          );
        endmodule
    )"));
}

TEST(AlignConsecutivePortConnections, PortListBoundaryBreaksGroup) {
    Style style;
    style.AlignConsecutivePortConnections = {.Enabled = true, .MaxPadding = 1};
    style.BinPackPortConnections = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar #(
        .W(8),
        .DW(16)
        ) u_bar (
        .clk(clk),
        .rst_n(rst_n)
        );
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          bar #(
              .W (8),
              .DW(16)
          ) u_bar (
              .clk(clk),
              .rst_n(rst_n)
          );
        endmodule
    )"));
}

TEST(AlignConsecutivePortConnections, SingleLine) {
    Style style;
    style.AlignConsecutivePortConnections = {.Enabled = true, .MaxPadding = 2};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar u_bar (
        .clk(clk)
        );
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          bar u_bar (
              .clk(clk)
          );
        endmodule
    )"));
}
// clang-format on
