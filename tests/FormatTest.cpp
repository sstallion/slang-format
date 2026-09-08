// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "Style.h"
#include "TestHelper.h"

#include <string>

#include <gtest/gtest.h>

using namespace slang::format;

TEST(ApplyEmptyLineLimits, EmptyLinesAtEndOfInput) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;


        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;

        endmodule
    )"));
    // clang-format on
}

TEST(ApplyEmptyLineLimits, FormatOffSkipsCollapse) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign a = 1;
        // slang-format off



        assign b = 2;
        // slang-format on



        assign c = 3;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign a = 1;
          // slang-format off



        assign b = 2;
        // slang-format on

          assign c = 3;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyEmptyLineLimits, LimitZeroRemovesAllEmptyLines) {
    Style style;
    style.MaxEmptyLinesToKeep = 0;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;


        assign y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;
          assign y = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyEmptyLineLimits, NoEmptyLines) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;
        assign y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;
          assign y = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyEmptyLineLimits, SingleEmptyLinePreserved) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;

        assign y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;

          assign y = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyEmptyLineLimits, ThreeEmptyLinesCollapsedToTwo) {
    Style style;
    style.MaxEmptyLinesToKeep = 2;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;



        assign y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;


          assign y = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyEmptyLineLimits, TwoEmptyLinesCollapsedToOne) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;


        assign y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;

          assign y = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, AlwaysBodyWithoutBegin) {
    Style const style{};

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

TEST(ApplyIndentation, BeginEndBlock) {
    Style const style{};

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

TEST(ApplyIndentation, BlankLinesEmittedWithoutIndent) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;

        assign y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;

          assign y = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, BreakAfterAlwaysAlwaysBeginNewline) {
    Style style;
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

TEST(ApplyIndentation, BreakAfterAlwaysAlwaysSimpleStatement) {
    Style style;
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

TEST(ApplyIndentation, BreakAfterAlwaysAlwaysTimingControl) {
    Style style;
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

TEST(ApplyIndentation, BreakAfterAlwaysNone) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakAfterBegin = true;
    style.BreakBeforeAlways = BlockBreakStyle::Never;

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

TEST(ApplyIndentation, BreakAfterAlwaysOnlyMultilineConditionalWithBlocks) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::OnlyMultiline;
    style.BreakAfterBegin = true;
    style.BreakBeforeAlways = BlockBreakStyle::Never;
    style.BreakBeforeEnd = true;

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

TEST(ApplyIndentation, BreakAfterAlwaysOnlyMultilineMultiStatement) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::OnlyMultiline;
    style.BreakAfterBegin = true;
    style.BreakBeforeAlways = BlockBreakStyle::Never;

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

TEST(ApplyIndentation, BreakAfterAlwaysOnlyMultilineSimpleIf) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::OnlyMultiline;

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

TEST(ApplyIndentation, BreakAfterAlwaysOnlyMultilineSingleLine) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::OnlyMultiline;
    style.BreakAfterBegin = true;
    style.BreakBeforeEnd = true;

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

TEST(ApplyIndentation, BreakAfterAlwaysOnlyMultilineSingleStatement) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::OnlyMultiline;
    style.BreakAfterBegin = true;
    style.BreakBeforeEnd = true;

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

TEST(ApplyIndentation, BreakAfterBeginAlreadyNewline) {
    Style const style{};

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

TEST(ApplyIndentation, BreakAfterBeginDisabled) {
    Style style;
    style.BreakAfterBegin = false;
    style.BreakBeforeEnd = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin x = 1; end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin x = 1; end
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, BreakAfterBeginInlineBlock) {
    Style style;
    style.BreakAfterBegin = true;
    style.BreakBeforeEnd = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin x = 1; end
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

TEST(ApplyIndentation, BreakAfterBeginNested) {
    Style style;
    style.BreakAfterBegin = true;
    style.BreakBeforeAlways = BlockBreakStyle::Never;
    style.BreakBeforeEnd = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin if (a) begin x = 1; end end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            if (a) begin
              x = 1;
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, BreakAfterBeginOnly) {
    Style style;
    style.BreakAfterBegin = true;
    style.BreakBeforeEnd = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin x = 1; end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            x = 1; end
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, BreakAfterBeginWithAlways) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Always;
    style.BreakAfterBegin = true;
    style.BreakBeforeEnd = true;

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

TEST(ApplyIndentation, BreakAfterInitialNever) {
    Style style;
    style.BreakAfterBegin = true;
    style.BreakAfterInitial = BlockBreakStyle::Never;
    style.BreakBeforeInitial = BlockBreakStyle::Never;

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

TEST(ApplyIndentation, BreakAfterInitialOnlyMultilineMultiStatement) {
    Style style;
    style.BreakAfterBegin = true;
    style.BreakAfterInitial = BlockBreakStyle::OnlyMultiline;
    style.BreakBeforeInitial = BlockBreakStyle::Never;

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

TEST(ApplyIndentation, BreakBeforeAlwaysAlways) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::Always;
    style.BreakAfterAlways = BlockBreakStyle::Never;

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

TEST(ApplyIndentation, BreakBeforeAlwaysAlwaysExistingBlankLine) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::Always;
    style.BreakAfterAlways = BlockBreakStyle::Never;

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

TEST(ApplyIndentation, BreakBeforeAlwaysAlwaysTimingControl) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::Always;
    style.BreakAfterAlways = BlockBreakStyle::Never;

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

TEST(ApplyIndentation, BreakBeforeAlwaysAlwaysWithComment) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::Always;
    style.BreakAfterAlways = BlockBreakStyle::Never;

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

TEST(ApplyIndentation, BreakBeforeAlwaysAlwaysWithTrailingComment) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::Always;
    style.BreakAfterAlways = BlockBreakStyle::Never;

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

          always #5 clk = ~clk; // 100 MHz

          always_comb begin
            x = 1;
          end
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, BreakBeforeAlwaysNever) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::Never;
    style.BreakAfterAlways = BlockBreakStyle::Never;

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

TEST(ApplyIndentation, BreakBeforeAlwaysOnlyMultilineMultiStatement) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakAfterBegin = true;
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

TEST(ApplyIndentation, BreakBeforeAlwaysOnlyMultilineSingleStatement) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakAfterBegin = true;
    style.BreakBeforeAlways = BlockBreakStyle::OnlyMultiline;
    style.BreakBeforeEnd = true;

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

TEST(ApplyIndentation, BreakBeforeAlwaysOnlyMultilineSingleItemWithBlock) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::OnlyMultiline;
    style.BreakAfterAlways = BlockBreakStyle::Never;

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

TEST(ApplyIndentation, BreakBeforeFunctionDefault) {
    Style style;
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

TEST(ApplyIndentation, BreakBeforeFunctionExistingBlankLine) {
    Style style;
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

TEST(ApplyIndentation, BreakBeforeFunctionFalse) {
    Style style;
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

TEST(ApplyIndentation, BreakBeforeFunctionWithComment) {
    Style style;
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

TEST(ApplyIndentation, BreakBeforeFunctionWithTrailingComment) {
    Style style;
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
          assign x = 1; // comment

          function void bar;
            x = 1;
          endfunction
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, BreakBeforeInitialAlways) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Always;
    style.BreakAfterInitial = BlockBreakStyle::Never;

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

TEST(ApplyIndentation, BreakBeforeInitialAlwaysExistingBlankLine) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Always;
    style.BreakAfterInitial = BlockBreakStyle::Never;

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

TEST(ApplyIndentation, BreakBeforeInitialAlwaysTimingControl) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Always;
    style.BreakAfterInitial = BlockBreakStyle::Never;

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

TEST(ApplyIndentation, BreakBeforeInitialAlwaysWithComment) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Always;
    style.BreakAfterInitial = BlockBreakStyle::Never;

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

TEST(ApplyIndentation, BreakBeforeInitialAlwaysWithTrailingComment) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Always;
    style.BreakAfterInitial = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::Never;

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
          always #20 dclk = ~dclk; //  25 MHz

          initial begin
            x = 1;
          end
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, BreakBeforeInitialNever) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Never;
    style.BreakAfterInitial = BlockBreakStyle::Never;

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

TEST(ApplyIndentation, BreakBeforeInitialOnlyMultilineMultiStatement) {
    Style style;
    style.BreakAfterBegin = true;
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

TEST(ApplyIndentation, BreakBeforeInitialOnlyMultilineSingleStatement) {
    Style style;
    style.BreakAfterBegin = true;
    style.BreakAfterInitial = BlockBreakStyle::Never;
    style.BreakBeforeEnd = true;
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

TEST(ApplyIndentation, BreakBeforeInitialOnlyMultilineSingleItemWithBlock) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::OnlyMultiline;
    style.BreakAfterInitial = BlockBreakStyle::Never;

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

TEST(ApplyIndentation, BreakBeforeSpecifyBlockDefault) {
    Style style;
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

TEST(ApplyIndentation, BreakBeforeSpecifyBlockExistingBlankLine) {
    Style style;
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

TEST(ApplyIndentation, BreakBeforeSpecifyBlockFalse) {
    Style style;
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

TEST(ApplyIndentation, BreakBeforeSpecifyBlockWithComment) {
    Style style;
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

TEST(ApplyIndentation, BreakBeforeSpecifyBlockWithTrailingComment) {
    Style style;
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
          assign x = 1; // comment

          specify
            $setup(posedge clk, data, 10);
          endspecify
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, BreakBeforeTaskDefault) {
    Style style;
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

TEST(ApplyIndentation, BreakBeforeTaskExistingBlankLine) {
    Style style;
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

TEST(ApplyIndentation, BreakBeforeTaskFalse) {
    Style style;
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

TEST(ApplyIndentation, BreakBeforeTaskWithComment) {
    Style style;
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

TEST(ApplyIndentation, BreakBeforeTaskWithTrailingComment) {
    Style style;
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
          assign x = 1; // comment

          task bar;
            x = 1;
          endtask
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, BreakBeforeEndOnly) {
    Style style;
    style.BreakAfterBegin = false;
    style.BreakBeforeEnd = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin x = 1; end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin x = 1;
          end
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, CaseItemNextLineIndented) {
    Style style;
    style.IndentCaseItem = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        case (x)
        2'b00:
        y = 0;
        default: y = 1;
        endcase
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            case (x)
              2'b00:
                y = 0;
              default: y = 1;
            endcase
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, CaseItemNextLineNotIndented) {
    Style style;
    style.IndentCaseItem = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        case (x)
        2'b00:
        y = 0;
        default: y = 1;
        endcase
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            case (x)
              2'b00:
              y = 0;
              default: y = 1;
            endcase
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, CaseItemSameLineBeginUnchanged) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::Never;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        case (x)
        2'b00: begin
        y = 0;
        end
        endcase
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            case (x)
              2'b00: begin
                y = 0;
              end
            endcase
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, CaseItems) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        case (x)
        2'b00: y = 0;
        default: y = 1;
        endcase
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            case (x)
              2'b00: y = 0;
              default: y = 1;
            endcase
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, ContinuationIndentWidthCustom) {
    Style style;
    style.ContinuationIndentWidth = 4;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = a +
          b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = a +
              b;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, ContinuationLineIndented) {
    Style style;
    style.ContinuationIndentWidth = 2;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = a +
          b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = a +
            b;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, ForLoopWithoutBegin) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        initial
        for (int i = 0; i < 4; i++)
        x = i;
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial
            for (int i = 0; i < 4; i++)
              x = i;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, FormatOffBlockComment) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        /* slang-format off */
        assign x = 1;
        /* slang-format on */
        assign y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          /* slang-format off */
        assign x = 1;
        /* slang-format on */
          assign y = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, FormatOffSkipsReindent) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        // slang-format off
        assign x = 1;
        // slang-format on
        assign y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off
        assign x = 1;
        // slang-format on
          assign y = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, IfBodyWithoutBegin) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        if (a)
        x = 1;
        else
        x = 0;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            if (a)
              x = 1;
            else
              x = 0;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, IndentWidthFour) {
    Style style;
    style.IndentWidth = 4;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
            assign x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, LineCommentIndented) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin
        // comment
        x = 1;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            // comment
            x = 1;
          end
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, ModuleMembersAndEndmodule) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, NestedBeginEnd) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::Never;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin
        if (a) begin
        x = 1;
        end
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            if (a) begin
              x = 1;
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, OneLineFormatOffRegexSkipsLine) {
    Style style;
    style.OneLineFormatOffRegex = ".*STATE_.*";

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = STATE_RESET;
        assign y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
        assign x = STATE_RESET;
          assign y = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, ParameterPortListIndented) {
    Style style;
    style.ParameterPortListIndentWidth = 2;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo #(
          parameter N = 4
        ) (
          input a
        );
        endmodule
    )"), style), dedent(R"(
        module foo #(
          parameter N = 4
        ) (
          input a
        );
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, ParameterPortListIndentWidthCustom) {
    Style style;
    style.ParameterPortListIndentWidth = 4;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo #(
          parameter N = 4
        ) (
          input a
        );
        endmodule
    )"), style), dedent(R"(
        module foo #(
            parameter N = 4
        ) (
            input a
        );
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, PortListIndented) {
    Style style;
    style.ParameterPortListIndentWidth = 2;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo(
          input a,
          input b
        );
        endmodule
    )"), style), dedent(R"(
        module foo(
          input a,
          input b
        );
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterComma, Collapses) {
    Style style;
    style.SpaceAfterComma = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign {a,  b,  c} = d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign {a, b, c} = d;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterComma, Disabled) {
    Style style;
    style.SpaceAfterComma = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign {a,b,c} = d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign {a,b,c} = d;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterComma, FormatOff) {
    Style style;
    style.SpaceAfterComma = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          // slang-format off
          assign {a,b,c} = d;
          // slang-format on
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off
          assign {a,b,c} = d;
          // slang-format on
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterComma, Inserts) {
    Style style;
    style.SpaceAfterComma = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign {a,b,c} = d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign {a, b, c} = d;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterComma, NormalizesWhenDisabled) {
    Style style;
    style.SpaceAfterComma = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign {a , b , c} = d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign {a,b,c} = d;
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign {a,  b,  c} = d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign {a,b,c} = d;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterComma, NormalizesAround) {
    Style style;
    style.SpaceAfterComma = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign {a  ,  b  ,  c} = d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign {a, b, c} = d;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterComma, PreservesNewlines) {
    Style style;
    style.ParameterPortListIndentWidth = 2;
    style.SpaceAfterComma = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo(
          input a,
          input b
        );
        endmodule
    )"), style), dedent(R"(
        module foo(
          input a,
          input b
        );
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterComma, RemovesBefore) {
    Style style;
    style.SpaceAfterComma = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign {a , b , c} = d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign {a, b, c} = d;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterSemicolon, Collapses) {
    Style style;
    style.SpaceAfterSemicolon = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            for (int i = 0;  i < 4;  i++) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            for (int i = 0; i < 4; i++) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterSemicolon, Disabled) {
    Style style;
    style.SpaceAfterSemicolon = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            for (int i = 0;i < 4;i++) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            for (int i = 0;i < 4;i++) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterSemicolon, FormatOff) {
    Style style;
    style.SpaceAfterSemicolon = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            // slang-format off
            for (int i = 0;i < 4;i++) begin
            end
            // slang-format on
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            // slang-format off
            for (int i = 0;i < 4;i++) begin
            end
            // slang-format on
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterSemicolon, Inserts) {
    Style style;
    style.SpaceAfterSemicolon = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            for (int i = 0;i < 4;i++) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            for (int i = 0; i < 4; i++) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterSemicolon, NormalizesAround) {
    Style style;
    style.SpaceAfterSemicolon = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            for (int i = 0  ;  i < 4  ;  i++) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            for (int i = 0; i < 4; i++) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterSemicolon, NormalizesWhenDisabled) {
    Style style;
    style.SpaceAfterSemicolon = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            for (int i = 0 ; i < 4 ; i++) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            for (int i = 0;i < 4;i++) begin
            end
          end
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            for (int i = 0;  i < 4;  i++) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            for (int i = 0;i < 4;i++) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterSemicolon, PreservesNewlines) {
    Style style;
    style.SpaceAfterSemicolon = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign a = 1;
          assign b = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign a = 1;
          assign b = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterSemicolon, RemovesBefore) {
    Style style;
    style.SpaceAfterSemicolon = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            for (int i = 0 ; i < 4 ; i++) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            for (int i = 0; i < 4; i++) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAroundOperators, Collapses) {
    Style style;
    style.SpaceAroundOperators = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x  =  a  +  b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = a + b;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAroundOperators, Disabled) {
    Style style;
    style.SpaceAroundOperators = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x=a+b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x=a+b;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAroundOperators, FormatOff) {
    Style style;
    style.SpaceAroundOperators = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          // slang-format off
          assign x=a+b;
          // slang-format on
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off
          assign x=a+b;
          // slang-format on
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAroundOperators, Inserts) {
    Style style;
    style.SpaceAroundOperators = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x=a+b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = a + b;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAroundOperators, NormalizesAround) {
    Style style;
    style.SpaceAroundOperators = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x  =  a  +  b  ==  c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = a + b == c;
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            x  <=  a  &&  b;
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            x <= a && b;
          end
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign y  =  a  ?  b  :  c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign y = a ? b : c;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAroundOperators, NormalizesWhenDisabled) {
    Style style;
    style.SpaceAroundOperators = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = a + b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x=a+b;
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign y = a ? b : c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign y=a?b:c;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAroundOperators, PreservesNewlines) {
    Style style;
    style.SpaceAroundOperators = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x =
            a + b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x =
          a + b;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAroundOperators, RemovesBefore) {
    Style style;
    style.SpaceAroundOperators = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x  =  a  +  b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = a + b;
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBraces, Collapses) {
    Style style;
    style.SpacesInBraces = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = {  a,  b,  c  };
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = { a, b, c };
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBraces, Disabled) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = {a, b, c};
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = {a, b, c};
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBraces, FormatOff) {
    Style style;
    style.SpacesInBraces = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          // slang-format off
          assign x = {a, b, c};
          // slang-format on
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off
          assign x = {a, b, c};
          // slang-format on
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBraces, Inserts) {
    Style style;
    style.SpacesInBraces = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = {a, b, c};
          assign y = '{a, b};
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = { a, b, c };
          assign y = '{ a, b };
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBraces, NormalizesAround) {
    Style style;
    style.SpacesInBraces = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = {  a  ,  b  ,  c  };
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = { a, b, c };
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBraces, NormalizesWhenDisabled) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = { a, b, c };
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = {a, b, c};
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = {  a,  b,  c  };
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = {a, b, c};
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBraces, PreservesNewlines) {
    Style style;
    style.SpacesInBraces = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = {
          a,
          b
          };
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = {
          a,
          b
          };
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBraces, RemovesBefore) {
    Style style;
    style.SpacesInBraces = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = {  a, b, c  };
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = { a, b, c };
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBrackets, Collapses) {
    Style style;
    style.SpacesInBrackets = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [  7:0  ] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [ 7:0 ] data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBrackets, Disabled) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBrackets, FormatOff) {
    Style style;
    style.SpacesInBrackets = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          // slang-format off
          logic [7:0] data;
          // slang-format on
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off
          logic [7:0] data;
          // slang-format on
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBrackets, Inserts) {
    Style style;
    style.SpacesInBrackets = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] data;
          logic data [0:255];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [ 7:0 ] data;
          logic data [ 0:255 ];
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBrackets, NormalizesAround) {
    Style style;
    style.SpacesInBrackets = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            x = data[  i  ];
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            x = data[ i ];
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBrackets, NormalizesWhenDisabled) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [ 7:0 ] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] data;
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic data [ 0:255 ];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic data [0:255];
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBrackets, PreservesNewlines) {
    Style style;
    style.SpacesInBrackets = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [
          7:0
          ] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [
          7:0
          ] data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBrackets, RemovesBefore) {
    Style style;
    style.SpacesInBrackets = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [  7:0  ] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [ 7:0 ] data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInParens, Collapses) {
    Style style;
    style.SpacesInParens = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            if (  a  ) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            if ( a ) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInParens, Disabled) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            if (a) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            if (a) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInParens, FormatOff) {
    Style style;
    style.SpacesInParens = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            // slang-format off
            if (a) begin
            end
            // slang-format on
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            // slang-format off
            if (a) begin
            end
            // slang-format on
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInParens, Inserts) {
    Style style;
    style.SpacesInParens = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            if (a) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            if ( a ) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInParens, NormalizesAround) {
    Style style;
    style.SpacesInParens = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            for (int i = 0  ;  i < 4  ;  i++) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            for ( int i = 0; i < 4; i++ ) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInParens, NormalizesWhenDisabled) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            if ( a ) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            if (a) begin
            end
          end
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            if (  a  ) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            if (a) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInParens, PreservesNewlines) {
    Style style;
    style.ParameterPortListIndentWidth = 2;
    style.SpacesInParens = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo(
          input a,
          input b
        );
        endmodule
    )"), style), dedent(R"(
        module foo(
          input a,
          input b
        );
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInParens, RemovesBefore) {
    Style style;
    style.SpacesInParens = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            if (  a  ) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            if ( a ) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}
