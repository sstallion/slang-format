// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "slang/format/Format.h"
#include "slang/format/Style.h"
#include "slang/format/styles/Default.h"
#include "slang/util/Dedent.h"

#include <gtest/gtest.h>

using namespace slang::format;
using slang::dedent;

TEST(AlignConsecutiveAssignments, AcrossComments) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveAssignments.AcrossComments = true;
    style.AlignConsecutiveAssignments.MaxPadding = 0;
    style.AlignConsecutiveDeclarations = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, AcrossEmptyLines) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveAssignments.AcrossEmptyLines = true;
    style.AlignConsecutiveAssignments.MaxPadding = 0;
    style.AlignConsecutiveDeclarations = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, AcrossEmptyLinesAndComments) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveAssignments.AcrossComments = true;
    style.AlignConsecutiveAssignments.AcrossEmptyLines = true;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, AcrossParameterPortList) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveAssignments.AcrossParameterPortList = true;
    style.AlignConsecutiveDeclarations = {};
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, Aligns) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, BeginEndBreaksGroup) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveAssignments.AcrossComments = true;
    style.AlignConsecutiveAssignments.AcrossEmptyLines = true;
    style.AlignConsecutiveDeclarations = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, BitSelectLHS) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, BlockingAssignments) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, CaseStatementDoesNotBreakGroup) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, CommentBreaksGroup) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, CompoundAssignments) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, ContinuationLines) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};
    style.ContinuationIndentWidth = 2;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, DeclarationWithoutInitializer) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, DepthChangeBreaksGroup) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveAssignments.AcrossComments = true;
    style.AlignConsecutiveAssignments.AcrossEmptyLines = true;
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
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, EmptyLineBreaksGroup) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, ForkJoinDoesNotCrossAlign) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, FormatOff) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, HierarchicalLHS) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, IfStatementDoesNotBreakGroup) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, MixedDeclarationsAndAssignments) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, NonblockingAssignments) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, ShiftAssignments) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, SingleLine) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutiveAssignments, WithDeclarationAlignment) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutiveDeclarations.Enabled = true;
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePortConnections, Aligns) {
    auto style = getDefaultStyle();
    style.AlignConsecutivePortConnections = {};
    style.AlignConsecutivePortConnections.Enabled = true;
    style.AlignConsecutivePortConnections.MaxPadding = 0;
    style.BinPackPortConnections = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePortConnections, Disabled) {
    auto style = getDefaultStyle();
    style.AlignConsecutivePortConnections = {};
    style.BinPackPortConnections = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePortConnections, MaxPaddingExceeded) {
    auto style = getDefaultStyle();
    style.AlignConsecutivePortConnections = {};
    style.AlignConsecutivePortConnections.Enabled = true;
    style.AlignConsecutivePortConnections.MaxPadding = 2;
    style.BinPackPortConnections = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePortConnections, PortListBoundaryBreaksGroup) {
    auto style = getDefaultStyle();
    style.AlignConsecutivePortConnections = {};
    style.AlignConsecutivePortConnections.Enabled = true;
    style.AlignConsecutivePortConnections.MaxPadding = 1;
    style.BinPackPortConnections = true;

    // clang-format off
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
    // clang-format on
}

TEST(AlignConsecutivePortConnections, SingleLine) {
    auto style = getDefaultStyle();
    style.AlignConsecutivePortConnections = {};
    style.AlignConsecutivePortConnections.Enabled = true;
    style.AlignConsecutivePortConnections.MaxPadding = 2;

    // clang-format off
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
    // clang-format on
}
