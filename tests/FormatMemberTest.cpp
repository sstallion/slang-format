// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "Style.h"
#include "TestHelper.h"

#include <gtest/gtest.h>

using namespace slang::format;

// clang-format off
TEST(BreakAfterAlways, AlwaysBareStatement) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Always;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb x = y;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            x = y;
        endmodule
    )"));
}

TEST(BreakAfterAlways, AlwaysInlineBlock) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Always;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always @(clk) begin x = 1; end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always @(clk)
          begin
            x = 1;
          end
        endmodule
    )"));
}

TEST(BreakAfterAlways, AlwaysMultilineBlock) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Always;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin
          x = 1;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            x = 1;
          end
        endmodule
    )"));
}

TEST(BreakAfterAlways, AlwaysTimingControl) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Always;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always @(posedge clk) x <= y;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always @(posedge clk)
            x <= y;
        endmodule
    )"));
}

TEST(BreakAfterAlways, Never) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Never;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin x = 1;
          y = 2;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            x = 1;
            y = 2;
          end
        endmodule
    )"));
}

TEST(BreakAfterAlways, OnlyMultilineBlock) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::OnlyMultiline;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin x = 1;
          y = 2;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
          begin
            x = 1;
            y = 2;
          end
        endmodule
    )"));
}

TEST(BreakAfterAlways, OnlyMultilineConditional) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::OnlyMultiline;
    style.CompactControlStatements = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always @(posedge clk_i) if (!enable_i) state_cs <= STATE_RESET; else state_cs <= state_ns;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always @(posedge clk_i)
            if (!enable_i)
              state_cs <= STATE_RESET;
            else
              state_cs <= state_ns;
        endmodule
    )"));
}

TEST(BreakAfterAlways, OnlyMultilineConditionalBlocks) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::OnlyMultiline;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always @(posedge clk_i) if (a) begin x <= 1; end else begin y <= 2; end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always @(posedge clk_i)
            if (a) begin
              x <= 1;
            end else begin
              y <= 2;
            end
        endmodule
    )"));
}

TEST(BreakAfterAlways, OnlyMultilineSingleLineBlock) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::OnlyMultiline;
    style.OneStatementPerLine = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin x = 1; y = 2; end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            x = 1; y = 2;
          end
        endmodule
    )"));
}

TEST(BreakAfterAlways, OnlyMultilineSingleLineConditional) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::OnlyMultiline;
    style.OneStatementPerLine = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb if (a) x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb if (a) x = 1;
        endmodule
    )"));
}

TEST(BreakAfterAlways, OnlyMultilineSingleStatementBlock) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::OnlyMultiline;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin x = y; end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            x = y;
          end
        endmodule
    )"));
}

TEST(BreakAfterAlways, PreservesNewlines) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::OnlyMultiline;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always @(posedge clk)
        x <= y;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always @(posedge clk)
            x <= y;
        endmodule
    )"));
}

TEST(BreakAfterInitial, Never) {
    Style style;
    style.BreakAfterInitial = BlockBreakStyle::Never;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        initial begin x = 1;
          y = 2;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            x = 1;
            y = 2;
          end
        endmodule
    )"));
}

TEST(BreakAfterInitial, OnlyMultilineBlock) {
    Style style;
    style.BreakAfterInitial = BlockBreakStyle::OnlyMultiline;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        initial begin x = 1;
          y = 2;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial
          begin
            x = 1;
            y = 2;
          end
        endmodule
    )"));
}

TEST(BreakBeforeAlways, Always) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::Always;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin
          x = 1;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;

          always_comb begin
            x = 1;
          end
        endmodule
    )"));
}

TEST(BreakBeforeAlways, AlwaysPreservesBlankLine) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::Always;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;

        always_comb begin
          x = 1;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;

          always_comb begin
            x = 1;
          end
        endmodule
    )"));
}

TEST(BreakBeforeAlways, AlwaysTimingControl) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::Always;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always @(posedge clk) x <= y;
        endmodule
    )"), style), dedent(R"(
        module foo;

          always @(posedge clk) x <= y;
        endmodule
    )"));
}

TEST(BreakBeforeAlways, AlwaysWithComment) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::Always;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        // comment
        always_comb begin
          x = 1;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;

          // comment
          always_comb begin
            x = 1;
          end
        endmodule
    )"));
}

TEST(BreakBeforeAlways, AlwaysWithTrailingComment) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::Always;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always #5 clk = ~clk; // 100 MHz
        always_comb begin
          x = 1;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;

          always #5 clk = ~clk;  // 100 MHz

          always_comb begin
            x = 1;
          end
        endmodule
    )"));
}

TEST(BreakBeforeAlways, Never) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::Never;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin
          x = 1;
          y = 2;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            x = 1;
            y = 2;
          end
        endmodule
    )"));
}

TEST(BreakBeforeAlways, OnlyMultilineBlock) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::OnlyMultiline;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin x = 1;
          y = 2;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;

          always_comb begin
            x = 1;
            y = 2;
          end
        endmodule
    )"));
}

TEST(BreakBeforeAlways, OnlyMultilineConditional) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::OnlyMultiline;
    style.CompactControlStatements = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;
        always @(posedge clk_i) if (!enable_i) state_cs <= STATE_RESET; else state_cs <= state_ns;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;

          always @(posedge clk_i) if (!enable_i)
              state_cs <= STATE_RESET;
            else
              state_cs <= state_ns;
        endmodule
    )"));
}

TEST(BreakBeforeAlways, OnlyMultilineNestedBlock) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::OnlyMultiline;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin
          x = 1;
          y = 2;
        end
        always_ff @(posedge clk_i) begin
          if (x == 0) begin
            y <= 1;
          end
        end
        endmodule
    )"), style), dedent(R"(
        module foo;

          always_comb begin
            x = 1;
            y = 2;
          end

          always_ff @(posedge clk_i) begin
            if (x == 0) begin
              y <= 1;
            end
          end
        endmodule
    )"));
}

TEST(BreakBeforeAlways, OnlyMultilineSingleStatementBlock) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::OnlyMultiline;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin x = y; end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            x = y;
          end
        endmodule
    )"));
}

TEST(BreakBeforeFunction, Disabled) {
    Style style;
    style.BreakBeforeFunction = false;
    style.ContinuationIndentWidth = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;
        function void bar;
          x = 1;
        endfunction
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;
          function void bar;
            x = 1;
          endfunction
        endmodule
    )"));
}

TEST(BreakBeforeFunction, Inserts) {
    Style style;
    style.BreakBeforeFunction = true;
    style.ContinuationIndentWidth = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;
        function void bar;
          x = 1;
        endfunction
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;

          function void bar;
            x = 1;
          endfunction
        endmodule
    )"));
}

TEST(BreakBeforeFunction, PreservesBlankLine) {
    Style style;
    style.BreakBeforeFunction = true;
    style.ContinuationIndentWidth = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;

        function void bar;
          x = 1;
        endfunction
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;

          function void bar;
            x = 1;
          endfunction
        endmodule
    )"));
}

TEST(BreakBeforeFunction, WithComment) {
    Style style;
    style.BreakBeforeFunction = true;
    style.ContinuationIndentWidth = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;
        // comment
        function void bar;
          x = 1;
        endfunction
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;

          // comment
          function void bar;
            x = 1;
          endfunction
        endmodule
    )"));
}

TEST(BreakBeforeFunction, WithTrailingComment) {
    Style style;
    style.BreakBeforeFunction = true;
    style.ContinuationIndentWidth = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1; // comment
        function void bar;
          x = 1;
        endfunction
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;  // comment

          function void bar;
            x = 1;
          endfunction
        endmodule
    )"));
}

TEST(BreakBeforeInitial, Always) {
    Style style;
    style.BreakAfterInitial = BlockBreakStyle::Never;
    style.BreakBeforeInitial = BlockBreakStyle::Always;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        initial begin
          x = 1;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            x = 1;
          end
        endmodule
    )"));
}

TEST(BreakBeforeInitial, AlwaysPreservesBlankLine) {
    Style style;
    style.BreakAfterInitial = BlockBreakStyle::Never;
    style.BreakBeforeInitial = BlockBreakStyle::Always;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;

        initial begin
          x = 1;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            x = 1;
          end
        endmodule
    )"));
}

TEST(BreakBeforeInitial, AlwaysTimingControl) {
    Style style;
    style.BreakAfterInitial = BlockBreakStyle::Never;
    style.BreakBeforeInitial = BlockBreakStyle::Always;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        initial #10 x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial #10 x = 1;
        endmodule
    )"));
}

TEST(BreakBeforeInitial, AlwaysWithComment) {
    Style style;
    style.BreakAfterInitial = BlockBreakStyle::Never;
    style.BreakBeforeInitial = BlockBreakStyle::Always;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        // comment
        initial begin
          x = 1;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;

          // comment
          initial begin
            x = 1;
          end
        endmodule
    )"));
}

TEST(BreakBeforeInitial, AlwaysWithTrailingComment) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Always;
    style.BreakAfterInitial = BlockBreakStyle::Never;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always #20 dclk = ~dclk; //  25 MHz
        initial begin
          x = 1;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always #20 dclk = ~dclk;  //  25 MHz

          initial begin
            x = 1;
          end
        endmodule
    )"));
}

TEST(BreakBeforeInitial, Never) {
    Style style;
    style.BreakAfterInitial = BlockBreakStyle::Never;
    style.BreakBeforeInitial = BlockBreakStyle::Never;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        initial begin
          x = 1;
          y = 2;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            x = 1;
            y = 2;
          end
        endmodule
    )"));
}

TEST(BreakBeforeInitial, OnlyMultilineBlock) {
    Style style;
    style.BreakAfterInitial = BlockBreakStyle::Never;
    style.BreakBeforeInitial = BlockBreakStyle::OnlyMultiline;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        initial begin x = 1;
          y = 2;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            x = 1;
            y = 2;
          end
        endmodule
    )"));
}

TEST(BreakBeforeInitial, OnlyMultilineNestedBlock) {
    Style style;
    style.BreakAfterInitial = BlockBreakStyle::Never;
    style.BreakBeforeInitial = BlockBreakStyle::OnlyMultiline;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        initial begin
          x = 1;
          y = 2;
        end
        initial begin
          if (x == 0) begin
            y = 1;
          end
        end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            x = 1;
            y = 2;
          end

          initial begin
            if (x == 0) begin
              y = 1;
            end
          end
        endmodule
    )"));
}

TEST(BreakBeforeInitial, OnlyMultilineSingleStatementBlock) {
    Style style;
    style.BreakAfterInitial = BlockBreakStyle::Never;
    style.BreakBeforeInitial = BlockBreakStyle::OnlyMultiline;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        initial begin x = y; end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            x = y;
          end
        endmodule
    )"));
}

TEST(BreakBeforeSpecifyBlock, Disabled) {
    Style style;
    style.BreakBeforeSpecifyBlock = false;
    style.ContinuationIndentWidth = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;
        specify
          $setup(posedge clk, data, 10);
        endspecify
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;
          specify
            $setup(posedge clk, data, 10);
          endspecify
        endmodule
    )"));
}

TEST(BreakBeforeSpecifyBlock, Inserts) {
    Style style;
    style.BreakBeforeSpecifyBlock = true;
    style.ContinuationIndentWidth = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;
        specify
          $setup(posedge clk, data, 10);
        endspecify
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;

          specify
            $setup(posedge clk, data, 10);
          endspecify
        endmodule
    )"));
}

TEST(BreakBeforeSpecifyBlock, PreservesBlankLine) {
    Style style;
    style.BreakBeforeSpecifyBlock = true;
    style.ContinuationIndentWidth = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;

        specify
          $setup(posedge clk, data, 10);
        endspecify
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;

          specify
            $setup(posedge clk, data, 10);
          endspecify
        endmodule
    )"));
}

TEST(BreakBeforeSpecifyBlock, WithComment) {
    Style style;
    style.BreakBeforeSpecifyBlock = true;
    style.ContinuationIndentWidth = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;
        // comment
        specify
          $setup(posedge clk, data, 10);
        endspecify
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;

          // comment
          specify
            $setup(posedge clk, data, 10);
          endspecify
        endmodule
    )"));
}

TEST(BreakBeforeSpecifyBlock, WithTrailingComment) {
    Style style;
    style.BreakBeforeSpecifyBlock = true;
    style.ContinuationIndentWidth = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1; // comment
        specify
          $setup(posedge clk, data, 10);
        endspecify
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;  // comment

          specify
            $setup(posedge clk, data, 10);
          endspecify
        endmodule
    )"));
}

TEST(BreakBeforeTask, Disabled) {
    Style style;
    style.BreakBeforeTask = false;
    style.ContinuationIndentWidth = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;
        task bar;
          x = 1;
        endtask
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;
          task bar;
            x = 1;
          endtask
        endmodule
    )"));
}

TEST(BreakBeforeTask, Inserts) {
    Style style;
    style.BreakBeforeTask = true;
    style.ContinuationIndentWidth = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;
        task bar;
          x = 1;
        endtask
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;

          task bar;
            x = 1;
          endtask
        endmodule
    )"));
}

TEST(BreakBeforeTask, PreservesBlankLine) {
    Style style;
    style.BreakBeforeTask = true;
    style.ContinuationIndentWidth = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;

        task bar;
          x = 1;
        endtask
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;

          task bar;
            x = 1;
          endtask
        endmodule
    )"));
}

TEST(BreakBeforeTask, WithComment) {
    Style style;
    style.BreakBeforeTask = true;
    style.ContinuationIndentWidth = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;
        // comment
        task bar;
          x = 1;
        endtask
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;

          // comment
          task bar;
            x = 1;
          endtask
        endmodule
    )"));
}

TEST(BreakBeforeTask, WithTrailingComment) {
    Style style;
    style.BreakBeforeTask = true;
    style.ContinuationIndentWidth = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1; // comment
        task bar;
          x = 1;
        endtask
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;  // comment

          task bar;
            x = 1;
          endtask
        endmodule
    )"));
}

TEST(CheckerDeclaration, Members) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        checker foo(logic clk, req, ack);
        a1: assert property(p);
        endchecker
    )"), style), dedent(R"(
        checker foo(logic clk, req, ack);
          a1 :
          assert property (p);
        endchecker
    )"));
}

TEST(ClockingDeclaration, Members) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        clocking cb@(posedge clk);
        default input#1step output#0;
        input data;
        output valid;
        endclocking
        endmodule
    )"), style), dedent(R"(
        module foo;
          clocking cb @(posedge clk);
            default input #1step output #0;
            input data;
            output valid;
          endclocking
        endmodule
    )"));
}

TEST(CompactTimeUnits, CommentPreventsJoin) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          timeunit 1ns;
          // precision follows
          timeprecision 1ps;
        endmodule
    )"), style), dedent(R"(
        module foo;
          timeunit 1ns;
          // precision follows
          timeprecision 1ps;
        endmodule
    )"));
}

TEST(CompactTimeUnits, Disabled) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.CompactTimeUnits = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          timeunit 1ns; timeprecision 1ps;
        endmodule
    )"), style), dedent(R"(
        module foo;
          timeunit 1ns;
          timeprecision 1ps;
        endmodule
    )"));
}

TEST(CompactTimeUnits, Enabled) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.CompactTimeUnits = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          timeunit 1ns; timeprecision 1ps;
        endmodule
    )"), style), dedent(R"(
        module foo;
          timeunit 1ns; timeprecision 1ps;
        endmodule
    )"));
}

TEST(CompactTimeUnits, JoinsAcrossBlankLine) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          timeunit 1ns;

          timeprecision 1ps;
        endmodule
    )"), style), dedent(R"(
        module foo;
          timeunit 1ns; timeprecision 1ps;
        endmodule
    )"));
}

TEST(CompactTimeUnits, JoinsSeparateLines) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          timeunit 1ns;
          timeprecision 1ps;
        endmodule
    )"), style), dedent(R"(
        module foo;
          timeunit 1ns; timeprecision 1ps;
        endmodule
    )"));
}

TEST(CompilationUnit, Members) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        endmodule

        // comment
        module bar;
        endmodule
    )"), style), dedent(R"(
        module foo;
        endmodule

        // comment
        module bar;
        endmodule
    )"));
}

TEST(ConfigDeclaration, Members) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        config foo;
        design work.top;
          default liblist work;
        instance top.u_dut liblist work;
             cell dff use work.dff;
        endconfig
    )"), style), dedent(R"(
        config foo;
          design work.top;
          default liblist work;
          instance top.u_dut liblist work;
          cell dff use work.dff;
        endconfig
    )"));
}

TEST(FunctionDeclaration, Body) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        package foo;
        function int add(int a, int b);
        return a + b;
        endfunction
        endpackage
    )"), style), dedent(R"(
        package foo;
          function int add(int a, int b);
            return a + b;
          endfunction
        endpackage
    )"));
}

TEST(PropertyDeclaration, SequenceMatchItem) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          property p;
            logic [7:0] v;
            @(posedge clk) disable iff (!rst_n) (vld_i, v = data_i) |=> (payload_q == v);
          endproperty
        endmodule
    )"), style), dedent(R"(
        module foo;
          property p;
            logic [7:0] v;
            @(posedge clk) disable iff (!rst_n) (vld_i, v = data_i) |=> (payload_q == v);
          endproperty
        endmodule
    )"));
}

TEST(SpaceAfterAlways, Collapses) {
    Style style;
    style.SpaceAfterAlways = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_ff   @(posedge clk)
        q <= d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_ff @(posedge clk)
            q <= d;
        endmodule
    )"));
}

TEST(SpaceAfterAlways, Disabled) {
    Style style;
    style.SpaceAfterAlways = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin
        x = 1;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            x = 1;
          end
        endmodule
    )"));
}

TEST(SpaceAfterAlways, FormatOff) {
    Style style;
    style.SpaceAfterAlways = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          // slang-format off
          always_ff@(posedge clk)
            q <= d;
          // slang-format on
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off
          always_ff@(posedge clk)
            q <= d;
          // slang-format on
        endmodule
    )"));
}

TEST(SpaceAfterAlways, Inserts) {
    Style style;
    style.SpaceAfterAlways = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_ff@(posedge clk)
        q <= d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_ff @(posedge clk)
            q <= d;
        endmodule
    )"));
}

TEST(SpaceAfterAlways, PreservesNewlines) {
    Style style;
    style.SpaceAfterAlways = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        begin
        x = 1;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
          begin
            x = 1;
          end
        endmodule
    )"));
}

TEST(SpaceAfterAlways, Removes) {
    Style style;
    style.SpaceAfterAlways = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_ff @(posedge clk)
        q <= d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_ff@(posedge clk)
            q <= d;
        endmodule
    )"));
}

TEST(SpaceAfterParensEventControls, Collapses) {
    Style style;
    style.SpaceAfterParens.EventControls = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_ff @(posedge clk)   begin
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_ff @(posedge clk) begin
          end
        endmodule
    )"));
}

TEST(SpaceAfterParensEventControls, FormatOff) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          // slang-format off
          always_ff @(posedge clk)begin
          end
          // slang-format on
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off
          always_ff @(posedge clk)begin
          end
          // slang-format on
        endmodule
    )"));
}

TEST(SpaceAfterParensEventControls, IgnoresBareImplicitEvent) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always @* x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always @* x = 1;
        endmodule
    )"));
}

TEST(SpaceAfterParensEventControls, IgnoresDelay) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial
            #(5)x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial
              #(5) x = 1;
        endmodule
    )"));
}

TEST(SpaceAfterParensEventControls, Inserts) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_ff @(posedge clk)begin
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_ff @(posedge clk) begin
          end
        endmodule
    )"));
}

TEST(SpaceAfterParensEventControls, InsertsStatement) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            @(posedge clk)x <= 1;
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            @(posedge clk) x <= 1;
          end
        endmodule
    )"));
}

TEST(SpaceAfterParensEventControls, PreservesNewlines) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_ff @(posedge clk)
            x <= 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_ff @(posedge clk)
            x <= 1;
        endmodule
    )"));
}

TEST(SpaceAfterParensEventControls, Removes) {
    Style style;
    style.SpaceAfterParens.EventControls = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_ff @(posedge clk) begin
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_ff @(posedge clk)begin
          end
        endmodule
    )"));
}

TEST(SpaceAfterParensEventControls, RemovesImplicitEvent) {
    Style style;
    style.SpaceAfterParens.EventControls = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always @(*) x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always @(*)x = 1;
        endmodule
    )"));
}

TEST(UdpDeclaration, Combinational) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    EXPECT_EQ(reformat(dedent(R"(
        primitive foo(out, a, b, sel);
        output out;
        input a, b, sel;
        table
        0?0:0;
        1?0:1;
        ?0 1:0;
        ?1 1:1;
        endtable
        endprimitive
    )"), style), dedent(R"(
        primitive foo(out, a, b, sel);
          output out;
          input a, b, sel;
          table
            0 ? 0 : 0;
            1 ? 0 : 1;
            ? 0 1 : 0;
            ? 1 1 : 1;
          endtable
        endprimitive
    )"));
}

TEST(UdpDeclaration, Sequential) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    EXPECT_EQ(reformat(dedent(R"(
        primitive foo(q, clk, d);
        output q;
        reg q;
        input clk, d;
        table
        (01)0:?:0;
        (01)1:?:1;
        (?0)?:?:-;
        ?*:?:-;
        endtable
        endprimitive
    )"), style), dedent(R"(
        primitive foo(q, clk, d);
          output q;
          reg q;
          input clk, d;
          table
            (01) 0 : ? : 0;
            (01) 1 : ? : 1;
            (?0) ? : ? : -;
            ? * : ? : -;
          endtable
        endprimitive
    )"));
}
// clang-format on
