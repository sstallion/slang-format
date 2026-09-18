// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "Style.h"
#include "TestHelper.h"

#include <string>

#include <gtest/gtest.h>

using namespace slang::format;

TEST(AlignAfterOpenParen, AlignBinPackedFunctionCall) {
    Style style;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie, delta, echo_val);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie,
                     delta, echo_val);
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AlignAfterOpenParen, AlignEmptyArgList) {
    Style style;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            bar();
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            bar();
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AlignAfterOpenParen, AlignExistingBreaks) {
    const Style style;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            obj.method(alpha,
              bravo, charlie);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            obj.method(alpha,
                       bravo, charlie);
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AlignAfterOpenParen, AlignFitsOnOneLine) {
    Style style;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            bar(a, b, c);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            bar(a, b, c);
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AlignAfterOpenParen, AlignMethodCall) {
    Style style;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            obj.method(alpha, bravo, charlie, delta);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            obj.method(alpha, bravo, charlie,
                       delta);
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AlignAfterOpenParen, AlignNestedCalls) {
    Style style;
    style.ColumnLimit = 50; // NOLINT

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display("result: %d", calculate(alpha, bravo, charlie));
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display("result: %d",
                     calculate(alpha, bravo, charlie));
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AlignAfterOpenParen, AlignOnePerLine) {
    Style style;
    style.BinPackArguments = false;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie, delta);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display("alpha",
                     bravo,
                     charlie,
                     delta);
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AlignAfterOpenParen, AlignSingleArgument) {
    Style style;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            bar(x);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            bar(x);
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AlignAfterOpenParen, AlignSystemTask) {
    Style style;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display("format: %d %d", alpha, bravo, charlie);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display("format: %d %d", alpha,
                     bravo, charlie);
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AlignAfterOpenParen, AlwaysBreakBinPacked) {
    Style style;
    style.AlignAfterOpenParen = BracketAlignmentStyle::AlwaysBreak;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie, delta, echo_val);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display(
                "alpha", bravo, charlie, delta,
                echo_val);
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AlignAfterOpenParen, AlwaysBreakFitsOnOneLine) {
    Style style;
    style.AlignAfterOpenParen = BracketAlignmentStyle::AlwaysBreak;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            bar(a, b, c);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            bar(a, b, c);
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AlignAfterOpenParen, AlwaysBreakOnePerLine) {
    Style style;
    style.AlignAfterOpenParen = BracketAlignmentStyle::AlwaysBreak;
    style.BinPackArguments = false;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie, delta);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display(
                "alpha",
                bravo,
                charlie,
                delta);
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AlignAfterOpenParen, BlockIndentBinPacked) {
    Style style;
    style.AlignAfterOpenParen = BracketAlignmentStyle::BlockIndent;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie, delta, echo_val);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display(
                "alpha", bravo, charlie, delta,
                echo_val
            );
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AlignAfterOpenParen, BlockIndentFitsOnOneLine) {
    Style style;
    style.AlignAfterOpenParen = BracketAlignmentStyle::BlockIndent;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            bar(a, b, c);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            bar(a, b, c);
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AlignAfterOpenParen, BlockIndentOnePerLine) {
    Style style;
    style.AlignAfterOpenParen = BracketAlignmentStyle::BlockIndent;
    style.BinPackArguments = false;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie, delta);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display(
                "alpha",
                bravo,
                charlie,
                delta
            );
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AlignAfterOpenParen, DontAlignPreservesSource) {
    Style style;
    style.AlignAfterOpenParen = BracketAlignmentStyle::DontAlign;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie, delta, echo_val);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie, delta, echo_val);
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AlignAfterOpenParen, ElabSystemTask) {
    Style style;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          $fatal(1, "some very long error message here");
        endmodule
    )"), style), dedent(R"(
        module foo;
          $fatal(1,
                 "some very long error message here");
        endmodule
    )"));
    // clang-format on
}

TEST(AlignAfterOpenParen, NewClassExpression) {
    Style style;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            obj = new(alpha, bravo, charlie, delta);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            obj = new(alpha, bravo, charlie,
                      delta);
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AlignAfterOpenParen, NoColumnLimitDisablesWrapping) {
    Style style;
    style.ColumnLimit = 0;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie, delta, echo_val, foxtrot, golf, hotel, india);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie, delta, echo_val, foxtrot, golf, hotel, india);
          end
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyEmptyLineLimits, EmptyLinesAtEndOfInput) {
    Style style;
    style.RemoveEmptyLines = true;

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
    Style style;
    style.RemoveEmptyLines = true;

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
    style.RemoveEmptyLines = true;

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
    style.RemoveEmptyLines = true;

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
    Style style;
    style.RemoveEmptyLines = true;

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

TEST(ApplyIndentation, BreakAfterAlwaysOnlyMultilineConditionalBare) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::Never;
    style.CompactConditionals = false;

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

TEST(ApplyIndentation, BreakAfterAlwaysOnlyMultilineSingleLine) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::OnlyMultiline;
    style.BreakAfterBegin = true;
    style.BreakBeforeEnd = true;
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

          always #5 clk = ~clk;  // 100 MHz

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

TEST(ApplyIndentation, BreakBeforeAlwaysOnlyMultilineConditionalBare) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Never;
    style.BreakBeforeAlways = BlockBreakStyle::OnlyMultiline;
    style.CompactConditionals = false;

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
          assign x = 1;  // comment

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

          initial#10 x = 1;
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
          always #20 dclk = ~dclk;  //  25 MHz

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
          assign x = 1;  // comment

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
          assign x = 1;  // comment

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

TEST(ApplyIndentation, EndTokenBlockComment) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        /* comment */
        endmodule
    )"), style), dedent(R"(
        module foo;
          /* comment */
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, EndTokenLineComment) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        // comment
        endmodule
    )"), style), dedent(R"(
        module foo;
          // comment
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, EndTokenLineCommentAfterMember) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;
        // comment
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;
          // comment
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, EndTokenLineCommentInBlock) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin
        // comment
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            // comment
          end
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, EndTokenLineCommentInFunction) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        function void bar;
        // comment
        endfunction
        endmodule
    )"), style), dedent(R"(
        module foo;
          function void bar;
            // comment
          endfunction
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

TEST(ApplyIndentation, HierarchyInstantiationMultilineNamedPorts) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        dff #(
        .W(8),
        .RST_VAL(8'hFF)
        ) u_dff (
        .clk(clk),
        .rst_n(rst_n),
        .d(data_i),
        .q(dff_q)
        );
        endmodule
    )"), style), dedent(R"(
        module foo;
          dff #(
              .W(8),
              .RST_VAL(8'hFF)
          ) u_dff (
              .clk(clk),
              .rst_n(rst_n),
              .d(data_i),
              .q(dff_q)
          );
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, HierarchyInstantiationNoParameters) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar u_bar (
        .clk(clk),
        .rst_n(rst_n)
        );
        endmodule
    )"), style), dedent(R"(
        module foo;
          bar u_bar (
              .clk  (clk),
              .rst_n(rst_n)
          );
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, HierarchyInstantiationParameterPortListIndentWidthCustom) {
    Style style;
    style.ParameterPortListIndentWidth = 3;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        dff #(
        .W(8)
        ) u_dff (
        .clk(clk)
        );
        endmodule
    )"), style), dedent(R"(
        module foo;
          dff #(
             .W(8)
          ) u_dff (
             .clk(clk)
          );
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, HierarchyInstantiationPositionalPorts) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        dff #(8, 8'h00) u_dff (
        clk,
        rst_n,
        en,
        d,
        q
        );
        endmodule
    )"), style), dedent(R"(
        module foo;
          dff #(8, 8'h00) u_dff (
              clk,
              rst_n,
              en,
              d,
              q
          );
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, HierarchyInstantiationSingleLine) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        adder #(.W(DW)) u_adder (.*);
        endmodule
    )"), style), dedent(R"(
        module foo;
          adder #(.W(DW)) u_adder (.*);
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, HierarchyInstantiationSpaceBeforeParensParameterList) {
    Style style;
    style.SpaceBeforeParens.ParameterList = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        dff#(
        .W(8)
        ) u_dff (
        .clk(clk)
        );
        endmodule
    )"), style), dedent(R"(
        module foo;
          dff #(
              .W(8)
          ) u_dff (
              .clk(clk)
          );
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, HierarchyInstantiationSpaceBeforeParensParameterListFalse) {
    Style style;
    style.SpaceBeforeParens.ParameterList = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        dff #(
        .W(8)
        ) u_dff (
        .clk(clk)
        );
        endmodule
    )"), style), dedent(R"(
        module foo;
          dff#(
              .W(8)
          ) u_dff (
              .clk(clk)
          );
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, HierarchyInstantiationSpaceBeforeParensPortList) {
    Style style;
    style.SpaceBeforeParens.PortList = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar u_bar(
        .clk(clk)
        );
        endmodule
    )"), style), dedent(R"(
        module foo;
          bar u_bar (
              .clk(clk)
          );
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, HierarchyInstantiationSpaceBeforeParensPortListFalse) {
    Style style;
    style.SpaceBeforeParens.PortList = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar u_bar (
        .clk(clk)
        );
        endmodule
    )"), style), dedent(R"(
        module foo;
          bar u_bar(
              .clk(clk)
          );
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, HierarchyInstantiationTopLevel) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        bind tort_top handshake_chk u_chk (
        .clk(clk),
        .rst_n(rst_n),
        .req(req_int),
        .gnt(gnt_int)
        );
    )"), style), dedent(R"(
        bind tort_top handshake_chk u_chk (
            .clk  (clk),
            .rst_n(rst_n),
            .req  (req_int),
            .gnt  (gnt_int)
        );
    )"));
    // clang-format on
}

TEST(ApplyIndentation, HierarchyInstantiationWildcardPorts) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        adder #(
        .W(DW)
        ) u_adder (.*);
        endmodule
    )"), style), dedent(R"(
        module foo;
          adder #(
              .W(DW)
          ) u_adder (.*);
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyIndentation, IfBodyWithoutBegin) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::Never;

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

TEST(ApplyIndentation, ParameterPortListIndentWidthComment) {
    Style style;
    style.ParameterPortListIndentWidth = 4;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo #(
        // comment
        parameter N = 4
        ) (
        // comment
        input a
        );
        endmodule
    )"), style), dedent(R"(
        module foo #(
            // comment
            parameter N = 4
        ) (
            // comment
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
        module foo (
          input a,
          input b
        );
        endmodule
    )"));
    // clang-format on
}

TEST(CompactConditionals, BasicIf) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::Never;

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

TEST(CompactConditionals, BlockBodyNotCompacted) {
    Style style;
    style.BreakAfterBegin = true;
    style.BreakBeforeAlways = BlockBreakStyle::Never;
    style.BreakBeforeEnd = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb if (a) begin x = 1; end else begin y = 2; end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            if (a) begin
              x = 1;
            end else begin
              y = 2;
            end
        endmodule
    )"));
    // clang-format on
}

TEST(CompactConditionals, Disabled) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::Never;
    style.CompactConditionals = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb if (a) x = 1; else y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            if (a)
              x = 1;
            else
              y = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(CompactConditionals, ExceedsColumnLimit) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Always;
    style.BreakBeforeAlways = BlockBreakStyle::Never;
    style.ColumnLimit = 30; // NOLINT

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb if (some_cond) some_long_target = some_long_value;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            if (some_cond)
              some_long_target = some_long_value;
        endmodule
    )"));
    // clang-format on
}

TEST(CompactConditionals, IfElse) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::Never;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb if (a) x = 1; else y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            if (a) x = 1;
            else y = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(CompactConditionals, IfElseIfElse) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::Never;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb if (a) x = 1; else if (b) y = 2; else z = 3;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            if (a) x = 1;
            else if (b) y = 2;
            else z = 3;
        endmodule
    )"));
    // clang-format on
}

TEST(CompactConditionals, NoColumnLimit) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::Never;
    style.ColumnLimit = 0;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb if (some_very_long_condition) some_very_long_target = some_very_long_value;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb if (some_very_long_condition) some_very_long_target = some_very_long_value;
        endmodule
    )"));
    // clang-format on
}

TEST(CompactConditionals, PartialCompaction) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::Never;
    style.ColumnLimit = 30; // NOLINT

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb if (a) x = 1; else some_long_target = some_long_value;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            if (a) x = 1;
            else
              some_long_target = some_long_value;
        endmodule
    )"));
    // clang-format on
}

TEST(CompactConditionals, PreservesExistingNewlines) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::Never;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
          if (a)
            x = 1;
          else
            y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            if (a)
              x = 1;
            else
              y = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(Directive, DefaultNettypeAtFileScope) {
    const Style style;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        `default_nettype none
        module foo;
          assign a = 1;
        endmodule
    )"), style), dedent(R"(
        `default_nettype none
        module foo;
          assign a = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(Directive, DirectiveAfterModule) {
    const Style style;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign a = 1;
        endmodule
        `default_nettype wire
    )"), style), dedent(R"(
        module foo;
          assign a = 1;
        endmodule
        `default_nettype wire
    )"));
    // clang-format on
}

TEST(Directive, IncludeDirective) {
    const Style style;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        `include "file.svh"
        module foo;
          assign a = 1;
        endmodule
    )"), style), dedent(R"(
        `include "file.svh"
        module foo;
          assign a = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(Directive, MultipleDirectives) {
    const Style style;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        `timescale 1ns / 1ps
        `default_nettype none
        module foo;
          assign a = 1;
        endmodule
    )"), style), dedent(R"(
        `timescale 1ns / 1ps
        `default_nettype none
        module foo;
          assign a = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(Directive, TimescaleAtFileScope) {
    const Style style;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        `timescale 1ns / 1ps
        module foo;
          assign a = 1;
        endmodule
    )"), style), dedent(R"(
        `timescale 1ns / 1ps
        module foo;
          assign a = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(OneStatementPerLine, AlreadySeparateLines) {
    Style style;
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

TEST(OneStatementPerLine, DisabledPreservesLayout) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::Never;
    style.OneStatementPerLine = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_comb begin
            x = 1; y = 2; z = 3;
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            x = 1; y = 2; z = 3;
          end
        endmodule
    )"));
    // clang-format on
}

TEST(OneStatementPerLine, MixedSameAndSeparate) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::Never;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_comb begin
            x = 1; y = 2;
            z = 3;
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            x = 1;
            y = 2;
            z = 3;
          end
        endmodule
    )"));
    // clang-format on
}

TEST(OneStatementPerLine, ModuleMembers) {
    Style const style;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo; wire a; wire b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          wire a;
          wire b;
        endmodule
    )"));
    // clang-format on
}

TEST(OneStatementPerLine, MultipleStatementsInBlock) {
    Style style;
    style.BreakBeforeAlways = BlockBreakStyle::Never;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_comb begin
            x = 1; y = 2; z = 3;
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            x = 1;
            y = 2;
            z = 3;
          end
        endmodule
    )"));
    // clang-format on
}

TEST(OneStatementPerLine, SingleStatementInBlock) {
    Style const style;

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

TEST(RemoveEmptyLines, DefaultPreservesAll) {
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

TEST(RemoveEmptyLines, EnabledCollapsesEmptyLines) {
    Style style;
    style.RemoveEmptyLines = true;

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

TEST(RemoveEmptyLines, FormatOffPreservesEmptyLines) {
    Style style;
    style.RemoveEmptyLines = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        // slang-format off



        assign x = 1;
        // slang-format on
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off



        assign x = 1;
        // slang-format on
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterAlways, Collapses) {
    Style style;
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
    Style style;
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

TEST(SpaceAfterAlways, DisabledPreventsMerge) {
    Style style;
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
    Style style;
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
    Style style;
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

TEST(SpaceAfterAlways, NormalizesWhenDisabled) {
    Style style;
    style.SpaceAfterAlways = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_ff   @(posedge clk)
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

TEST(SpaceAfterAlways, PreservesNewlines) {
    Style style;
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

TEST(SpaceAfterBrackets, Collapses) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0]   data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterBrackets, Disabled) {
    Style style;
    style.SpaceAfterBrackets.PackedDimensions = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0]data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterBrackets, FormatOff) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          // slang-format off
          logic [7:0]data;
          // slang-format on
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off
          logic [7:0]data;
          // slang-format on
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterBrackets, IgnoresExpressions) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            x = data[i];
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            x = data[i];
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterBrackets, Inserts) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0]data;
          logic data[0:255];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] data;
          logic data[0:255];
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterBrackets, NormalizesAround) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0]  data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterBrackets, NormalizesWhenDisabled) {
    Style style;
    style.SpaceAfterBrackets.PackedDimensions = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0]  data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0]data;
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic data [0:255] ;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic data[0:255];
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterBrackets, PackedDimensions) {
    Style style;
    style.SpaceAfterBrackets.PackedDimensions = true;
    style.SpaceAfterBrackets.UnpackedDimensions = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0]data;
          logic data[0:255];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] data;
          logic data[0:255];
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterBrackets, PreservesNewlines) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0]
          data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0]
              data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterBrackets, RemovesBefore) {
    Style style;
    style.SpaceAfterBrackets.PackedDimensions = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0]   data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0]data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterBrackets, UnpackedDimensions) {
    Style style;
    style.SpaceAfterBrackets.PackedDimensions = false;
    style.SpaceAfterBrackets.UnpackedDimensions = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0]data[0:255][0:127];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0]data[0:255] [0:127];
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterCaseColon, Collapses) {
    Style style;
    style.SpaceAfterCaseColon = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        case (x)
        2'b00:   y = 0;
        default:   y = 1;
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

TEST(SpaceAfterCaseColon, Disabled) {
    Style style;
    style.SpaceAfterCaseColon = false;

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
              2'b00:y = 0;
              default:y = 1;
            endcase
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterCaseColon, FormatOff) {
    Style style;
    style.SpaceAfterCaseColon = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_comb
            // slang-format off
            case (x)
              2'b00:y = 0;
              default:y = 1;
            endcase
            // slang-format on
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            // slang-format off
            case (x)
              2'b00:y = 0;
              default:y = 1;
            endcase
            // slang-format on
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterCaseColon, Inserts) {
    Style style;
    style.SpaceAfterCaseColon = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        case (x)
        2'b00:y = 0;
        default:y = 1;
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

TEST(SpaceAfterCaseColon, NormalizesWhenDisabled) {
    Style style;
    style.SpaceAfterCaseColon = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        case (x)
        2'b00:   y = 0;
        default:   y = 1;
        endcase
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            case (x)
              2'b00:y = 0;
              default:y = 1;
            endcase
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterCaseColon, PreservesNewlines) {
    Style style;
    style.SpaceAfterCaseColon = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        case (x)
        2'b00:
        y = 0;
        default:
        y = 1;
        endcase
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            case (x)
              2'b00:
              y = 0;
              default:
              y = 1;
            endcase
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
          assign{a, b, c} = d;
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
          assign{a,b,c} = d;
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
          assign{a, b, c} = d;
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
          assign{a,b,c} = d;
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign {a,  b,  c} = d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign{a,b,c} = d;
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
          assign{a, b, c} = d;
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
        module foo (
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
          assign{a, b, c} = d;
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

TEST(SpaceAfterParensControlStatements, Collapses) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Never;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            if (a)   begin
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

TEST(SpaceAfterParensControlStatements, DisabledCase) {
    Style style;
    style.SpaceAfterParens.ControlStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            case (x) matches
              default:;
            endcase
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            case (x)matches
              default:;
            endcase
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterParensControlStatements, DisabledFor) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Never;
    style.SpaceAfterParens.ControlStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            for (int i = 0; i < 4; i++) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            for (int i = 0; i < 4; i++)begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterParensControlStatements, DisabledForeach) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Never;
    style.SpaceAfterParens.ControlStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            foreach (arr[i]) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            foreach (arr[i])begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterParensControlStatements, DisabledIf) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Never;
    style.SpaceAfterParens.ControlStatements = false;

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
            if (a)begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterParensControlStatements, DisabledWhile) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Never;
    style.SpaceAfterParens.ControlStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            while (a) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            while (a)begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterParensControlStatements, FormatOff) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Never;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            // slang-format off
            if (a)begin
            end
            // slang-format on
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            // slang-format off
            if (a)begin
            end
            // slang-format on
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterParensControlStatements, Inserts) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Never;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            if (a)begin
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

TEST(SpaceAfterParensControlStatements, NormalizesWhenDisabled) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Never;
    style.SpaceAfterParens.ControlStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            if (a)   begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            if (a)begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterParensControlStatements, PreservesNewlines) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            if (a)
              x = 1;
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            if (a)
              x = 1;
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterParensEventControls, Collapses) {
    Style const style{};

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

TEST(SpaceAfterParensEventControls, Disabled) {
    Style style;
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

TEST(SpaceAfterParensEventControls, DisabledImplicitEvent) {
    Style style;
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

TEST(SpaceAfterParensEventControls, FormatOff) {
    Style const style{};

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

TEST(SpaceAfterParensEventControls, Inserts) {
    Style const style{};

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

TEST(SpaceAfterParensEventControls, NormalizesWhenDisabled) {
    Style style;
    style.SpaceAfterParens.EventControls = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_ff @(posedge clk)   begin
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

TEST(SpaceAfterParensEventControls, PreservesNewlines) {
    Style const style{};

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

TEST(SpaceAfterParensEventControls, SkipsAtStar) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always @* x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always @*x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterParensEventControls, SkipsDelay) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial
            #(5)x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial
              #(5)x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterParensEventControls, StandaloneTimingControl) {
    Style const style{};

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

TEST(SpaceBeforeBrackets, Collapses) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic   [7:0] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeBrackets, Disabled) {
    Style style;
    style.SpaceBeforeBrackets.PackedDimensions = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic[7:0] data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeBrackets, FormatOff) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          // slang-format off
          logic[7:0] data;
          // slang-format on
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off
          logic[7:0] data;
          // slang-format on
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeBrackets, IgnoresExpressions) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            x = data[i];
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            x = data[i];
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeBrackets, Inserts) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic[7:0] data;
          logic data[0:255];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] data;
          logic data[0:255];
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeBrackets, NormalizesAround) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic  [7:0] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeBrackets, NormalizesWhenDisabled) {
    Style style;
    style.SpaceBeforeBrackets.PackedDimensions = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic[7:0] data;
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic data [0:255];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic data[0:255];
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeBrackets, PackedDimensions) {
    Style style;
    style.SpaceBeforeBrackets.PackedDimensions = true;
    style.SpaceBeforeBrackets.UnpackedDimensions = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic[7:0] data[0:255];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] data[0:255];
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeBrackets, PreservesNewlines) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic
          [7:0] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic
              [7:0] data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeBrackets, RemovesBefore) {
    Style style;
    style.SpaceBeforeBrackets.PackedDimensions = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic   [7:0] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic[7:0] data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeBrackets, UnpackedDimensions) {
    Style style;
    style.SpaceBeforeBrackets.PackedDimensions = false;
    style.SpaceBeforeBrackets.UnpackedDimensions = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] data[0:255];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic[7:0] data [0:255];
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeCaseColon, Collapses) {
    Style style;
    style.SpaceBeforeCaseColon = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        case (x)
        2'b00   : y = 0;
        default   : y = 1;
        endcase
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            case (x)
              2'b00 : y = 0;
              default : y = 1;
            endcase
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeCaseColon, Disabled) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        case (x)
        2'b00 : y = 0;
        default : y = 1;
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

TEST(SpaceBeforeCaseColon, FormatOff) {
    Style style;
    style.SpaceBeforeCaseColon = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_comb
            // slang-format off
            case (x)
              2'b00:y = 0;
              default:y = 1;
            endcase
            // slang-format on
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            // slang-format off
            case (x)
              2'b00:y = 0;
              default:y = 1;
            endcase
            // slang-format on
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeCaseColon, Inserts) {
    Style style;
    style.SpaceBeforeCaseColon = true;

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
              2'b00 : y = 0;
              default : y = 1;
            endcase
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeCaseColon, NormalizesWhenDisabled) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        case (x)
        2'b00   : y = 0;
        default   : y = 1;
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

TEST(SpaceBeforeCaseColon, PreservesNewlines) {
    Style style;
    style.SpaceBeforeCaseColon = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        case (x)
        2'b00
        : y = 0;
        default
        : y = 1;
        endcase
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            case (x)
              2'b00
                  : y = 0;
              default
                  : y = 1;
            endcase
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeParensControlStatements, Collapses) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Never;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            if   (a)begin
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

TEST(SpaceBeforeParensControlStatements, DisabledCase) {
    Style style;
    style.SpaceBeforeParens.ControlStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            case (x)
              default:;
            endcase
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            case(x)
              default:;
            endcase
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeParensControlStatements, DisabledDoWhile) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Never;
    style.SpaceBeforeParens.ControlStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            do begin
            end while (a);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            do begin
            end while(a);
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeParensControlStatements, DisabledFor) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Never;
    style.SpaceBeforeParens.ControlStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            for (int i = 0; i < 4; i++)begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            for(int i = 0; i < 4; i++) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeParensControlStatements, DisabledForeach) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Never;
    style.SpaceBeforeParens.ControlStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            foreach (arr[i])begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            foreach(arr[i]) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeParensControlStatements, DisabledIf) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Never;
    style.SpaceBeforeParens.ControlStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            if (a)begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            if(a) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeParensControlStatements, DisabledWhile) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Never;
    style.SpaceBeforeParens.ControlStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            while (a)begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            while(a) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeParensControlStatements, FormatOff) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Never;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            // slang-format off
            if(a)begin
            end
            // slang-format on
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            // slang-format off
            if(a)begin
            end
            // slang-format on
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeParensControlStatements, Inserts) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Never;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            if(a)begin
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

TEST(SpaceBeforeParensControlStatements, NormalizesWhenDisabled) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Never;
    style.SpaceBeforeParens.ControlStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            if   (a)begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            if(a) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeParensControlStatements, PreservesNewlines) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Never;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            if
            (a)begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            if
                (a) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeParensControlStatements, UniqueIf) {
    Style style;
    style.BreakBeforeInitial = BlockBreakStyle::Never;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            unique if(a)begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            unique if (a) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeParensParameterList, Collapses) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo   #(
          parameter N = 4
        )(
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

TEST(SpaceBeforeParensParameterList, Disabled) {
    Style style;
    style.SpaceBeforeParens.ParameterList = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo #(
          parameter N = 4
        )(
          input a
        );
        endmodule
    )"), style), dedent(R"(
        module foo#(
            parameter N = 4
        ) (
            input a
        );
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeParensParameterList, FormatOff) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        // slang-format off
        module foo#(
          parameter N = 4
        )(
          input a
        );
        endmodule
        // slang-format on
    )"), style), dedent(R"(
        // slang-format off
        module foo#(
          parameter N = 4
        )(
          input a
        );
        endmodule
        // slang-format on
    )"));
    // clang-format on
}

TEST(SpaceBeforeParensParameterList, Inserts) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo#(
          parameter N = 4
        )(
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

TEST(SpaceBeforeParensParameterList, NormalizesWhenDisabled) {
    Style style;
    style.SpaceBeforeParens.ParameterList = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo   #(
          parameter N = 4
        )(
          input a
        );
        endmodule
    )"), style), dedent(R"(
        module foo#(
            parameter N = 4
        ) (
            input a
        );
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeParensParameterList, PreservesNewlines) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo
        #(
          parameter N = 4
        )(
          input a
        );
        endmodule
    )"), style), dedent(R"(
        module foo
            #(
            parameter N = 4
        ) (
            input a
        );
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeParensPortList, Collapses) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo   (
          input a
        );
        endmodule
    )"), style), dedent(R"(
        module foo (
            input a
        );
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeParensPortList, Disabled) {
    Style style;
    style.SpaceBeforeParens.PortList = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo (
          input a
        );
        endmodule
    )"), style), dedent(R"(
        module foo(
            input a
        );
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeParensPortList, FormatOff) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        // slang-format off
        module foo(
          input a
        );
        endmodule
        // slang-format on
    )"), style), dedent(R"(
        // slang-format off
        module foo(
          input a
        );
        endmodule
        // slang-format on
    )"));
    // clang-format on
}

TEST(SpaceBeforeParensPortList, Inserts) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo(
          input a
        );
        endmodule
    )"), style), dedent(R"(
        module foo (
            input a
        );
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeParensPortList, NormalizesWhenDisabled) {
    Style style;
    style.SpaceBeforeParens.PortList = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo   (
          input a
        );
        endmodule
    )"), style), dedent(R"(
        module foo(
            input a
        );
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeParensPortList, PreservesNewlines) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo
        (
          input a
        );
        endmodule
    )"), style), dedent(R"(
        module foo
            (
            input a
        );
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

TEST(SpaceAroundOperators, DeclaratorInserts) {
    Style style;
    style.SpaceAroundOperators = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          localparam CLK_COUNT_MAX=CLKS_PER_BIT - 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          localparam CLK_COUNT_MAX = CLKS_PER_BIT - 1;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAroundOperators, DeclaratorNormalizesWhenDisabled) {
    Style style;
    style.SpaceAroundOperators = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          localparam CLK_COUNT_MAX = CLKS_PER_BIT - 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          localparam CLK_COUNT_MAX=CLKS_PER_BIT-1;
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

TEST(SpacesBeforeTrailingComments, BlockComment) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1; /* comment */
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;  /* comment */
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesBeforeTrailingComments, Custom) {
    Style style;
    style.SpacesBeforeTrailingComments = 4;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1; // comment
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;    // comment
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesBeforeTrailingComments, Default) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1; // comment
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;  // comment
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesBeforeTrailingComments, FormatOff) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        // slang-format off
        assign x = 1; // comment
        // slang-format on
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off
        assign x = 1; // comment
        // slang-format on
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesBeforeTrailingComments, One) {
    Style style;
    style.SpacesBeforeTrailingComments = 1;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1; // comment
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1; // comment
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesBeforeTrailingComments, StandaloneUnaffected) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        // standalone comment
        assign x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          // standalone comment
          assign x = 1;
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

TEST(SpacesInBrackets, IgnoresExpressions) {
    Style style;
    style.SpacesInBrackets = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            x = data[i];
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            x = data[i];
          end
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
          logic data[ 0:255 ];
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
            x = data[i];
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            x = data[i];
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
          logic data[0:255];
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
        module foo (
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

TEST(WhitespaceNormalization, IntegerVectorLiteral) {
    Style const style{};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign a = 4'b0000;
        assign b = 8'hFF;
        assign c = 4'd15;
        assign d = 3'o7;
        assign e = 4'sb1010;
        assign f = 4'bxxxx;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign a = 4'b0000;
          assign b = 8'hFF;
          assign c = 4'd15;
          assign d = 3'o7;
          assign e = 4'sb1010;
          assign f = 4'bxxxx;
        endmodule
    )"));
    // clang-format on
}
