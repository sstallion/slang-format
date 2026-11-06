// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "slang/format/Format.h"
#include "slang/format/Style.h"
#include "slang/format/styles/Default.h"
#include "slang/util/Dedent.h"

#include <gtest/gtest.h>

using namespace slang::format;
using slang::dedent;

TEST(BreakAfterAlways, AlwaysBareStatement) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::Always;

    // clang-format off
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
    // clang-format on
}

TEST(BreakAfterAlways, AlwaysInlineBlock) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::Always;

    // clang-format off
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
    // clang-format on
}

TEST(BreakAfterAlways, AlwaysMultilineBlock) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::Always;

    // clang-format off
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
    // clang-format on
}

TEST(BreakAfterAlways, AlwaysTimingControl) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::Always;

    // clang-format off
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
    // clang-format on
}

TEST(BreakAfterAlways, Never) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::Never;

    // clang-format off
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
    // clang-format on
}

TEST(BreakAfterAlways, OnlyMultilineBlock) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::OnlyMultiline;

    // clang-format off
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
    // clang-format on
}

TEST(BreakAfterAlways, OnlyMultilineConditional) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::OnlyMultiline;
    style.CompactControlStatements = false;

    // clang-format off
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
    // clang-format on
}

TEST(BreakAfterAlways, OnlyMultilineConditionalBlocks) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::OnlyMultiline;

    // clang-format off
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
    // clang-format on
}

TEST(BreakAfterAlways, OnlyMultilineSingleLineBlock) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::OnlyMultiline;
    style.OneStatementPerLine = false;

    // clang-format off
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
    // clang-format on
}

TEST(BreakAfterAlways, OnlyMultilineSingleLineConditional) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::OnlyMultiline;
    style.OneStatementPerLine = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb if (a) x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb if (a) x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(BreakAfterAlways, OnlyMultilineSingleStatementBlock) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::OnlyMultiline;

    // clang-format off
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
    // clang-format on
}

TEST(BreakAfterAlways, PreservesNewlines) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::OnlyMultiline;

    // clang-format off
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
    // clang-format on
}

TEST(BreakAfterInitial, Never) {
    auto style = getDefaultStyle();
    style.BreakAfterInitial = BlockBreakStyle::Never;

    // clang-format off
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
    // clang-format on
}

TEST(BreakAfterInitial, OnlyMultilineBlock) {
    auto style = getDefaultStyle();
    style.BreakAfterInitial = BlockBreakStyle::OnlyMultiline;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeAlways, Always) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::Always;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeAlways, AlwaysPreservesBlankLine) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::Always;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeAlways, AlwaysTimingControl) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::Always;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always @(posedge clk) x <= y;
        endmodule
    )"), style), dedent(R"(
        module foo;

          always @(posedge clk) x <= y;
        endmodule
    )"));
    // clang-format on
}

TEST(BreakBeforeAlways, AlwaysWithComment) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::Always;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeAlways, AlwaysWithTrailingComment) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::Always;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeAlways, Never) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::Never;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeAlways, OnlyMultilineBlock) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::OnlyMultiline;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeAlways, OnlyMultilineConditional) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::OnlyMultiline;
    style.CompactControlStatements = false;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeAlways, OnlyMultilineNestedBlock) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::OnlyMultiline;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeAlways, OnlyMultilineSingleStatementBlock) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::OnlyMultiline;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeFunction, Disabled) {
    auto style = getDefaultStyle();
    style.BreakBeforeFunction = false;
    style.ContinuationIndentWidth = 2;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeFunction, Inserts) {
    auto style = getDefaultStyle();
    style.BreakBeforeFunction = true;
    style.ContinuationIndentWidth = 2;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeFunction, PreservesBlankLine) {
    auto style = getDefaultStyle();
    style.BreakBeforeFunction = true;
    style.ContinuationIndentWidth = 2;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeFunction, WithComment) {
    auto style = getDefaultStyle();
    style.BreakBeforeFunction = true;
    style.ContinuationIndentWidth = 2;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeFunction, WithTrailingComment) {
    auto style = getDefaultStyle();
    style.BreakBeforeFunction = true;
    style.ContinuationIndentWidth = 2;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeInitial, Always) {
    auto style = getDefaultStyle();
    style.BreakAfterInitial = BlockBreakStyle::Never;
    style.BreakBeforeInitial = BlockBreakStyle::Always;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeInitial, AlwaysPreservesBlankLine) {
    auto style = getDefaultStyle();
    style.BreakAfterInitial = BlockBreakStyle::Never;
    style.BreakBeforeInitial = BlockBreakStyle::Always;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeInitial, AlwaysTimingControl) {
    auto style = getDefaultStyle();
    style.BreakAfterInitial = BlockBreakStyle::Never;
    style.BreakBeforeInitial = BlockBreakStyle::Always;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        initial #10 x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial #10 x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(BreakBeforeInitial, AlwaysWithComment) {
    auto style = getDefaultStyle();
    style.BreakAfterInitial = BlockBreakStyle::Never;
    style.BreakBeforeInitial = BlockBreakStyle::Always;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeInitial, AlwaysWithTrailingComment) {
    auto style = getDefaultStyle();
    style.BreakBeforeInitial = BlockBreakStyle::Always;
    style.BreakAfterInitial = BlockBreakStyle::Never;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeInitial, Never) {
    auto style = getDefaultStyle();
    style.BreakAfterInitial = BlockBreakStyle::Never;
    style.BreakBeforeInitial = BlockBreakStyle::Never;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeInitial, OnlyMultilineBlock) {
    auto style = getDefaultStyle();
    style.BreakAfterInitial = BlockBreakStyle::Never;
    style.BreakBeforeInitial = BlockBreakStyle::OnlyMultiline;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeInitial, OnlyMultilineNestedBlock) {
    auto style = getDefaultStyle();
    style.BreakAfterInitial = BlockBreakStyle::Never;
    style.BreakBeforeInitial = BlockBreakStyle::OnlyMultiline;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeInitial, OnlyMultilineSingleStatementBlock) {
    auto style = getDefaultStyle();
    style.BreakAfterInitial = BlockBreakStyle::Never;
    style.BreakBeforeInitial = BlockBreakStyle::OnlyMultiline;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeSpecifyBlock, Disabled) {
    auto style = getDefaultStyle();
    style.BreakBeforeSpecifyBlock = false;
    style.ContinuationIndentWidth = 2;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeSpecifyBlock, Inserts) {
    auto style = getDefaultStyle();
    style.BreakBeforeSpecifyBlock = true;
    style.ContinuationIndentWidth = 2;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeSpecifyBlock, PreservesBlankLine) {
    auto style = getDefaultStyle();
    style.BreakBeforeSpecifyBlock = true;
    style.ContinuationIndentWidth = 2;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeSpecifyBlock, WithComment) {
    auto style = getDefaultStyle();
    style.BreakBeforeSpecifyBlock = true;
    style.ContinuationIndentWidth = 2;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeSpecifyBlock, WithTrailingComment) {
    auto style = getDefaultStyle();
    style.BreakBeforeSpecifyBlock = true;
    style.ContinuationIndentWidth = 2;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeTask, Disabled) {
    auto style = getDefaultStyle();
    style.BreakBeforeTask = false;
    style.ContinuationIndentWidth = 2;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeTask, Inserts) {
    auto style = getDefaultStyle();
    style.BreakBeforeTask = true;
    style.ContinuationIndentWidth = 2;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeTask, PreservesBlankLine) {
    auto style = getDefaultStyle();
    style.BreakBeforeTask = true;
    style.ContinuationIndentWidth = 2;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeTask, WithComment) {
    auto style = getDefaultStyle();
    style.BreakBeforeTask = true;
    style.ContinuationIndentWidth = 2;

    // clang-format off
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
    // clang-format on
}

TEST(BreakBeforeTask, WithTrailingComment) {
    auto style = getDefaultStyle();
    style.BreakBeforeTask = true;
    style.ContinuationIndentWidth = 2;

    // clang-format off
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
    // clang-format on
}

TEST(CheckerDeclaration, Members) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(ClockingDeclaration, Members) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
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
    // clang-format on
}

TEST(CompactTimeUnits, CommentPreventsJoin) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(CompactTimeUnits, Disabled) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.CompactTimeUnits = false;

    // clang-format off
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
    // clang-format on
}

TEST(CompactTimeUnits, Enabled) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.CompactTimeUnits = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          timeunit 1ns; timeprecision 1ps;
        endmodule
    )"), style), dedent(R"(
        module foo;
          timeunit 1ns; timeprecision 1ps;
        endmodule
    )"));
    // clang-format on
}

TEST(CompactTimeUnits, JoinsAcrossBlankLine) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
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
    // clang-format on
}

TEST(CompactTimeUnits, JoinsSeparateLines) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
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
    // clang-format on
}

TEST(CompilationUnit, Members) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(ConfigDeclaration, Members) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(FunctionDeclaration, Body) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(PropertyDeclaration, SequenceMatchItem) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(SpaceAfterAlways, Collapses) {
    auto style = getDefaultStyle();
    style.SpaceAfterAlways = true;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceAfterAlways, Disabled) {
    auto style = getDefaultStyle();
    style.SpaceAfterAlways = false;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceAfterAlways, FormatOff) {
    auto style = getDefaultStyle();
    style.SpaceAfterAlways = true;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceAfterAlways, Inserts) {
    auto style = getDefaultStyle();
    style.SpaceAfterAlways = true;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceAfterAlways, PreservesNewlines) {
    auto style = getDefaultStyle();
    style.SpaceAfterAlways = true;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceAfterAlways, Removes) {
    auto style = getDefaultStyle();
    style.SpaceAfterAlways = false;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceAfterParensEventControls, Collapses) {
    auto style = getDefaultStyle();
    style.SpaceAfterParens.EventControls = true;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceAfterParensEventControls, FormatOff) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(SpaceAfterParensEventControls, IgnoresBareImplicitEvent) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always @* x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always @* x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterParensEventControls, IgnoresDelay) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(SpaceAfterParensEventControls, Inserts) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(SpaceAfterParensEventControls, InsertsStatement) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(SpaceAfterParensEventControls, PreservesNewlines) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(SpaceAfterParensEventControls, Removes) {
    auto style = getDefaultStyle();
    style.SpaceAfterParens.EventControls = false;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceAfterParensEventControls, RemovesImplicitEvent) {
    auto style = getDefaultStyle();
    style.SpaceAfterParens.EventControls = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always @(*) x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always @(*)x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(UdpDeclaration, Combinational) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
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
    // clang-format on
}

TEST(UdpDeclaration, Sequential) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
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
    // clang-format on
}
