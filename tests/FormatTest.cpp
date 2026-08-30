// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "Style.h"
#include "TestHelper.h"

#include <string>

#include <gtest/gtest.h>

using namespace slang::format;

TEST(ApplyEmptyLineLimits, EmptyLinesAtEndOfInput) {
    Style const style;

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
    Style const style;

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
    Style const style;

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
    Style const style;

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
    Style const style;

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
    Style const style;

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

TEST(ApplyIndentation, BlankLinesEmittedWithoutIndent) {
    Style const style;

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
    style.BreakAfterAlways = BreakAfterBlockStyle::Always;

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
    style.BreakAfterAlways = BreakAfterBlockStyle::Always;

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
    style.BreakAfterAlways = BreakAfterBlockStyle::Always;

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
    style.BreakAfterAlways = BreakAfterBlockStyle::Never;
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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
    style.BreakAfterAlways = BreakAfterBlockStyle::OnlyMultiline;
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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
    style.BreakAfterAlways = BreakAfterBlockStyle::OnlyMultiline;
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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
    style.BreakAfterAlways = BreakAfterBlockStyle::OnlyMultiline;

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
    style.BreakAfterAlways = BreakAfterBlockStyle::OnlyMultiline;

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
    style.BreakAfterAlways = BreakAfterBlockStyle::OnlyMultiline;

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
    Style const style;

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
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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
    style.BreakAfterAlways = BreakAfterBlockStyle::Always;

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
    style.BreakAfterInitial = BreakAfterBlockStyle::Never;

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
    style.BreakAfterInitial = BreakAfterBlockStyle::OnlyMultiline;

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
    style.BreakBeforeAlways = BreakAfterBlockStyle::Always;
    style.BreakAfterAlways = BreakAfterBlockStyle::Never;

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
    style.BreakBeforeAlways = BreakAfterBlockStyle::Always;
    style.BreakAfterAlways = BreakAfterBlockStyle::Never;

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
    style.BreakBeforeAlways = BreakAfterBlockStyle::Always;
    style.BreakAfterAlways = BreakAfterBlockStyle::Never;

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
    style.BreakBeforeAlways = BreakAfterBlockStyle::Always;
    style.BreakAfterAlways = BreakAfterBlockStyle::Never;

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

TEST(ApplyIndentation, BreakBeforeAlwaysNever) {
    Style style;
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;
    style.BreakAfterAlways = BreakAfterBlockStyle::Never;

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
    style.BreakBeforeAlways = BreakAfterBlockStyle::OnlyMultiline;
    style.BreakAfterAlways = BreakAfterBlockStyle::Never;

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
    style.BreakBeforeAlways = BreakAfterBlockStyle::OnlyMultiline;
    style.BreakAfterAlways = BreakAfterBlockStyle::Never;

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
    style.BreakBeforeAlways = BreakAfterBlockStyle::OnlyMultiline;
    style.BreakAfterAlways = BreakAfterBlockStyle::Never;

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
    Style const style;

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
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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
    Style const style;

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
    Style const style;

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
    Style const style;

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
    Style const style;

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
    Style const style;

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
    Style const style;

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
    Style const style;

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
    Style const style;

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
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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
    Style const style;

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
    Style const style;

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

// clang-format off
TEST(AlignConsecutiveDeclarations, None) {
    Style style;
    style.AlignConsecutiveDeclarations = {};

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
}

TEST(AlignConsecutiveDeclarations, Consecutive) {
    Style style;
    style.AlignConsecutiveDeclarations.Enabled = true;

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
}

TEST(AlignConsecutiveDeclarations, ConsecutiveEmptyLineBreaksGroup) {
    Style style;
    style.AlignConsecutiveDeclarations.Enabled = true;

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
}

TEST(AlignConsecutiveDeclarations, ConsecutiveCommentBreaksGroup) {
    Style style;
    style.AlignConsecutiveDeclarations.Enabled = true;

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
}

TEST(AlignConsecutiveDeclarations, ConsecutiveNonDeclarationBreaksGroup) {
    Style style;
    style.AlignConsecutiveDeclarations.Enabled = true;

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
}

TEST(AlignConsecutiveDeclarations, ContinuationLines) {
    Style style;
    style.AlignConsecutiveDeclarations.Enabled = true;

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
}

TEST(AlignConsecutiveDeclarations, AcrossEmptyLines) {
    Style style;
    style.AlignConsecutiveDeclarations = {.AcrossEmptyLines = true, .Enabled = true};

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
}

TEST(AlignConsecutiveDeclarations, AcrossComments) {
    Style style;
    style.AlignConsecutiveDeclarations = {.AcrossComments = true, .Enabled = true};

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
}

TEST(AlignConsecutiveDeclarations, AcrossEmptyLinesAndComments) {
    Style style;
    style.AlignConsecutiveDeclarations = {.AcrossComments = true, .AcrossEmptyLines = true, .Enabled = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a;

          // comment
          logic [7:0] b;
          logic [15:0] c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic        a;

          // comment
          logic [7:0]  b;
          logic [15:0] c;
        endmodule
    )"));
}

TEST(AlignConsecutiveDeclarations, AcrossParameterPortList) {
    Style style;
    style.AlignConsecutiveDeclarations = {.AcrossParameterPortList = true, .Enabled = true};

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
}

TEST(AlignConsecutiveDeclarations, PackedDimensions) {
    Style style;
    style.AlignConsecutiveDeclarations.Enabled = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] a;
          logic [15:0] b;
          logic [31:0] c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0]  a;
          logic [15:0] b;
          logic [31:0] c;
        endmodule
    )"));
}

TEST(AlignConsecutiveDeclarations, DirectionKeywords) {
    Style style;
    style.AlignConsecutiveDeclarations.Enabled = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo(
          input a,
          input logic [7:0] b,
          output c
        );
        endmodule
    )"), style), dedent(R"(
        module foo(
          input             a,
          input logic [7:0] b,
          output            c
        );
        endmodule
    )"));
}

TEST(AlignConsecutiveDeclarations, ParameterDeclarations) {
    Style style;
    style.AlignConsecutiveDeclarations.Enabled = true;

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
}

TEST(AlignConsecutiveDeclarations, SingleDeclarationNotAligned) {
    Style const style;

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

TEST(AlignConsecutiveDeclarations, FormatOffRegionSkipped) {
    Style style;
    style.AlignConsecutiveDeclarations.Enabled = true;

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
}

TEST(AlignConsecutiveDeclarations, IndentLevelBreaksGroup) {
    Style style;
    style.AlignConsecutiveDeclarations.Enabled = true;

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
}
// clang-format on

// clang-format off
TEST(AlignConsecutivePackedDimensions, None) {
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

TEST(AlignConsecutivePackedDimensions, Consecutive) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;

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

TEST(AlignConsecutivePackedDimensions, SkipsBareTypes) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;

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

TEST(AlignConsecutivePackedDimensions, GroupBrokenByEmptyLine) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;

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

TEST(AlignConsecutivePackedDimensions, GroupBrokenByComment) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;

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

TEST(AlignConsecutivePackedDimensions, AcrossEmptyLines) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.AcrossEmptyLines = true, .Enabled = true};

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

TEST(AlignConsecutivePackedDimensions, AcrossComments) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.AcrossComments = true, .Enabled = true};

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

TEST(AlignConsecutivePackedDimensions, AcrossEmptyLinesAndComments) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.AcrossComments = true, .AcrossEmptyLines = true, .Enabled = true};

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

TEST(AlignConsecutivePackedDimensions, AcrossParameterPortList) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.AcrossParameterPortList = true, .Enabled = true};

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

TEST(AlignConsecutivePackedDimensions, DirectionKeywords) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo(
          input bit [7:0] a,
          input logic [15:0] b
        );
        endmodule
    )"), style), dedent(R"(
        module foo(
          input bit   [7:0] a,
          input logic [15:0] b
        );
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, WithAlignConsecutiveDeclarations) {
    Style style;
    style.AlignConsecutiveDeclarations.Enabled = true;
    style.AlignConsecutivePackedDimensions.Enabled = true;

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

TEST(AlignConsecutivePackedDimensions, FormatOff) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;

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

TEST(AlignConsecutivePackedDimensions, SingleDeclaration) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;

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

TEST(AlignConsecutivePackedDimensions, AlignColon) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.AlignColon = true, .Enabled = true};

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
        .AlignColon = true, .Enabled = true, .PadLeft = true};

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
        .AlignColon = true, .Enabled = true, .PadRight = true};

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

TEST(AlignConsecutivePackedDimensions, BareTypeNarrowerThanDimensions) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions.Enabled = true;

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
    style.AlignConsecutivePackedDimensions.Enabled = true;

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

TEST(AlignConsecutivePackedDimensions, PadLeftPrecedenceOverPadRight) {
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

TEST(AlignConsecutivePackedDimensions, MultipleDimensions) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {.Enabled = true, .PadLeft = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0][3:0] a;
          logic [15:0][7:0] b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [ 7:0][3:0] a;
          logic [15:0][7:0] b;
        endmodule
    )"));
}

TEST(AlignConsecutivePackedDimensions, MultipleDimensionsDifferentWidths) {
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
          logic [ 7:0][ 3:0] a;
          logic [15:0][15:0] b;
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

TEST(AlignConsecutivePackedDimensions, ContentAlignDisabledWhenNotEnabled) {
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

TEST(AlignConsecutivePackedDimensions, PadLeftWithDeclarationAlignment) {
    Style style;
    style.AlignConsecutiveDeclarations.Enabled = true;
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

TEST(AlignConsecutiveTimingControls, Consecutive) {
    Style style;
    style.AlignConsecutiveTimingControls.Enabled = true;

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
}

TEST(AlignConsecutiveTimingControls, ConsecutiveEmptyLineBreaksGroup) {
    Style style;
    style.AlignConsecutiveTimingControls.Enabled = true;

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
}

TEST(AlignConsecutiveTimingControls, AcrossEmptyLines) {
    Style style;
    style.AlignConsecutiveTimingControls.Enabled = true;
    style.AlignConsecutiveTimingControls.AcrossEmptyLines = true;

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
}

TEST(AlignConsecutiveTimingControls, ConsecutiveCommentBreaksGroup) {
    Style style;
    style.AlignConsecutiveTimingControls.Enabled = true;

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
}

TEST(AlignConsecutiveTimingControls, AcrossComments) {
    Style style;
    style.AlignConsecutiveTimingControls.Enabled = true;
    style.AlignConsecutiveTimingControls.AcrossComments = true;

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
}

TEST(AlignConsecutiveTimingControls, None) {
    Style style;
    style.AlignConsecutiveTimingControls = {};

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
}

TEST(AlignConsecutiveTimingControls, ParenthesizedDelay) {
    Style style;
    style.AlignConsecutiveTimingControls.Enabled = true;

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
}

TEST(AlignConsecutiveTimingControls, SingleLineNotAligned) {
    Style style;
    style.AlignConsecutiveTimingControls.Enabled = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always #5 clk_i = ~clk_i;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always #5 clk_i = ~clk_i;
        endmodule
    )"));
}
// clang-format on

// clang-format off
TEST(AlignConsecutiveAssignments, None) {
    Style style;
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a = 1;
          logic [7:0] b = 2;
          logic [15:0] c = 3;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic a = 1;
          logic [7:0] b = 2;
          logic [15:0] c = 3;
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, Consecutive) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
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

TEST(AlignConsecutiveAssignments, ConsecutiveEmptyLineBreaksGroup) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a = 1;
          logic [7:0] b = 2;

          logic [15:0] c = 3;
          logic d = 4;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic a       = 1;
          logic [7:0] b = 2;

          logic [15:0] c = 3;
          logic d        = 4;
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, ConsecutiveCommentBreaksGroup) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
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

TEST(AlignConsecutiveAssignments, ConsecutiveNonDeclarationBreaksGroup) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a = 1;
          logic [7:0] b = 2;
          assign x = 1;
          logic [15:0] c = 3;
          logic d = 4;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic a        = 1;
          logic [7:0] b  = 2;
          assign x       = 1;
          logic [15:0] c = 3;
          logic d        = 4;
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, ContinuationLines) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          localparam [1:0] STATE_0 = 1,
            STATE_1 = 2,
            STATE_2 = 3;
        endmodule
    )"), style), dedent(R"(
        module foo;
          localparam [1:0] STATE_0 = 1,
            STATE_1                = 2,
            STATE_2                = 3;
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, ContinuationLinesVariableLength) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

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

TEST(AlignConsecutiveAssignments, ContinuationLinesWithDeclarationAlignment) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations.Enabled = true;

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
}

TEST(AlignConsecutiveAssignments, AcrossEmptyLines) {
    Style style;
    style.AlignConsecutiveAssignments = {.AcrossEmptyLines = true, .Enabled = true};
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

TEST(AlignConsecutiveAssignments, AcrossComments) {
    Style style;
    style.AlignConsecutiveAssignments = {.AcrossComments = true, .Enabled = true};
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

TEST(AlignConsecutiveAssignments, AcrossEmptyLinesAndComments) {
    Style style;
    style.AlignConsecutiveAssignments = {.AcrossComments = true, .AcrossEmptyLines = true, .Enabled = true};
    style.AlignConsecutiveDeclarations = {};

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
    style.AlignConsecutiveAssignments = {.AcrossParameterPortList = true, .Enabled = true};
    style.AlignConsecutiveDeclarations = {};

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

TEST(AlignConsecutiveAssignments, DeclarationWithoutInitializer) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

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

TEST(AlignConsecutiveAssignments, StandaloneAssignmentAligned) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1;
          assign longname = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x        = 1;
          assign longname = 2;
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, SingleDeclarationNotAligned) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic a = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic a = 1;
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, DepthChangeBreaksGroup) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveAssignments.AcrossEmptyLines = true;
    style.AlignConsecutiveAssignments.AcrossComments = true;
    style.AlignConsecutiveDeclarations = {};
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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

TEST(AlignConsecutiveAssignments, FormatOffRegionSkipped) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
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

TEST(AlignConsecutiveAssignments, InteractionWithAlignConsecutiveDeclarations) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations.Enabled = true;

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

TEST(AlignConsecutiveAssignments, BlockingAssignments) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_comb begin
            a = 1;
            longname = 2;
          end
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          always_comb begin
            a        = 1;
            longname = 2;
          end
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, NonblockingAssignments) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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

TEST(AlignConsecutiveAssignments, CaseStatementDoesNotBreakGroup) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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

TEST(AlignConsecutiveAssignments, CompoundAssignments) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_comb begin
            a += 1;
            longname -= 2;
          end
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          always_comb begin
            a        += 1;
            longname -= 2;
          end
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, ShiftAssignments) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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

TEST(AlignConsecutiveAssignments, HierarchicalLHS) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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

TEST(AlignConsecutiveAssignments, BitSelectLHS) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_comb begin
            x[0] = 1;
            longname[3:0] = 2;
          end
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          always_comb begin
            x[0]          = 1;
            longname[3:0] = 2;
          end
        endmodule
    )"));
}

TEST(AlignConsecutiveAssignments, IfStatementDoesNotBreakGroup) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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
    )"), style), dedent(R"(
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
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
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

TEST(AlignConsecutiveAssignments, BeginEndBreaksGroup) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveAssignments.AcrossEmptyLines = true;
    style.AlignConsecutiveAssignments.AcrossComments = true;
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;
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
    )"),
                       style),
              dedent(R"(
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

TEST(AlignConsecutiveAssignments, ControlFlowDoesNotBreakGroup) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_comb begin
            a = 1;
            if (x == 1) begin
              b = 2;
            end
            longname = 3;
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            a        = 1;
            if (x == 1) begin
              b = 2;
            end
            longname = 3;
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AlignConsecutiveAssignments, SingleAssignmentNotAligned) {
    Style style;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_comb begin
            a = 1;
          end
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          always_comb begin
            a = 1;
          end
        endmodule
    )"));
}

TEST(AlignTrailingComments, None) {
    Style const style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1; // first
          assign longname = 2; // second
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          assign x = 1; // first
          assign longname = 2; // second
        endmodule
    )"));
}

TEST(AlignTrailingComments, Consecutive) {
    Style style;
    style.AlignTrailingComments.Enabled = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1; // first
          assign longname = 2; // second
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          assign x = 1;        // first
          assign longname = 2; // second
        endmodule
    )"));
}

TEST(AlignTrailingComments, ConsecutiveBlockComment) {
    Style style;
    style.AlignTrailingComments.Enabled = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1; /* first */
          assign longname = 2; /* second */
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          assign x = 1;        /* first */
          assign longname = 2; /* second */
        endmodule
    )"));
}

TEST(AlignTrailingComments, ConsecutiveEmptyLineBreaksGroup) {
    Style style;
    style.AlignTrailingComments.Enabled = true;

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
          assign x = 1;        // first
          assign longname = 2; // second

          assign a = 3;  // third
          assign bc = 4; // fourth
        endmodule
    )"));
}

TEST(AlignTrailingComments, ConsecutiveCommentBreaksGroup) {
    Style style;
    style.AlignTrailingComments.Enabled = true;

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
          assign x = 1;        // first
          assign longname = 2; // second
          // standalone
          assign a = 3;  // third
          assign bc = 4; // fourth
        endmodule
    )"));
}

TEST(AlignTrailingComments, AcrossEmptyLines) {
    Style style;
    style.AlignTrailingComments = {.AcrossEmptyLines = true, .Enabled = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1; // first
          assign longname = 2; // second

          assign a = 3; // third
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          assign x = 1;        // first
          assign longname = 2; // second

          assign a = 3;        // third
        endmodule
    )"));
}

TEST(AlignTrailingComments, AcrossComments) {
    Style style;
    style.AlignTrailingComments = {.AcrossComments = true, .Enabled = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1; // first
          assign longname = 2; // second
          // standalone
          assign a = 3; // third
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          assign x = 1;        // first
          assign longname = 2; // second
          // standalone
          assign a = 3;        // third
        endmodule
    )"));
}

TEST(AlignTrailingComments, AcrossEmptyLinesAndComments) {
    Style style;
    style.AlignTrailingComments = {.AcrossComments = true,
                                   .AcrossEmptyLines = true,
                                   .Enabled = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1; // first

          // standalone
          assign longname = 2; // second
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          assign x = 1;        // first

          // standalone
          assign longname = 2; // second
        endmodule
    )"));
}

TEST(AlignTrailingComments, AcrossParameterPortList) {
    Style style;
    style.AlignTrailingComments = {.AcrossParameterPortList = true, .Enabled = true};

    EXPECT_EQ(reformat(dedent(R"(
        module foo #(
          parameter N = 4 // width
        ) (
          input a // port a
        );
          assign x = 1; // body
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo #(
          parameter N = 4 // width
        ) (
          input a         // port a
        );
          assign x = 1;   // body
        endmodule
    )"));
}

TEST(AlignTrailingComments, SingleLineNotAligned) {
    Style style;
    style.AlignTrailingComments.Enabled = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1; // only one
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          assign x = 1; // only one
        endmodule
    )"));
}

TEST(AlignTrailingComments, NoTrailingComment) {
    Style style;
    style.AlignTrailingComments.Enabled = true;

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
          assign x = 1; // first
          assign longname = 2;
          assign a = 3; // third
        endmodule
    )"));
}

TEST(AlignTrailingComments, MixedLineAndBlockComments) {
    Style style;
    style.AlignTrailingComments.Enabled = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1; // line comment
          assign longname = 2; /* block comment */
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          assign x = 1;        // line comment
          assign longname = 2; /* block comment */
        endmodule
    )"));
}

TEST(AlignTrailingComments, InteractionWithAlignConsecutiveAssignments) {
    Style style;
    style.AlignTrailingComments.Enabled = true;
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveDeclarations = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1; // first
          assign longname = 2; // second
        endmodule
    )"),
                       style),
              dedent(R"(
        module foo;
          assign x = 1;        // first
          assign longname = 2; // second
        endmodule
    )"));
}

TEST(AlignTrailingComments, FormatOffRegionSkipped) {
    Style style;
    style.AlignTrailingComments.Enabled = true;

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
// clang-format on
