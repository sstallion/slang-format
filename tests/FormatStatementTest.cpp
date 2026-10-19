// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "Style.h"
#include "TestHelper.h"

#include <gtest/gtest.h>

using namespace slang::format;

// clang-format off
TEST(BreakAfterAssertionLabel, ConcurrentAssertion) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        a1: assert property (p);
        endmodule
    )"), style), dedent(R"(
        module foo;
          a1 :
          assert property (p);
        endmodule
    )"));
}

TEST(BreakAfterAssertionLabel, Disabled) {
    Style style;
    style.BreakAfterAssertionLabel = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        a1: assert property (p);
        endmodule
    )"), style), dedent(R"(
        module foo;
          a1 : assert property (p);
        endmodule
    )"));
}

TEST(BreakAfterAssertionLabel, ImmediateAssertionWithElse) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        initial begin
        a1: assert (x) else $error("e");
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            a1 :
            assert (x) else $error("e");
          end
        endmodule
    )"));
}

TEST(BreakAfterBegin, Disabled) {
    Style style;
    style.BreakAfterBegin = false;
    style.BreakBeforeEnd = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin x = 1; end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin x = 1; end
        endmodule
    )"));
}

TEST(BreakAfterBegin, Inserts) {
    Style style;
    style.BreakAfterBegin = true;
    style.BreakBeforeEnd = false;

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
}

TEST(BreakAfterBegin, Nested) {
    Style style;
    style.BreakAfterBegin = true;

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
}

TEST(BreakAfterBegin, PreservesNewlines) {
    Style style;
    style.BreakAfterBegin = true;

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

TEST(BreakBeforeEnd, Inserts) {
    Style style;
    style.BreakAfterBegin = false;
    style.BreakBeforeEnd = true;

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
}

TEST(CompactControlStatements, BlockBodies) {
    Style style;
    style.CompactControlStatements = true;

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
}

TEST(CompactControlStatements, Disabled) {
    Style style;
    style.CompactControlStatements = false;

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
}

TEST(CompactControlStatements, ExceedsColumnLimit) {
    Style style;
    style.BreakAfterAlways = BlockBreakStyle::Always;
    style.ColumnLimit = 30; // NOLINT
    style.CompactControlStatements = true;

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
}

TEST(CompactControlStatements, If) {
    Style style;
    style.CompactControlStatements = true;

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

TEST(CompactControlStatements, IfElse) {
    Style style;
    style.CompactControlStatements = true;

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
}

TEST(CompactControlStatements, IfElseIfElse) {
    Style style;
    style.CompactControlStatements = true;

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
}

TEST(CompactControlStatements, NoColumnLimit) {
    Style style;
    style.ColumnLimit = 0;
    style.CompactControlStatements = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb if (some_very_long_condition) some_very_long_target = some_very_long_value;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb if (some_very_long_condition) some_very_long_target = some_very_long_value;
        endmodule
    )"));
}

TEST(CompactControlStatements, PartiallyCompacts) {
    Style style;
    style.ColumnLimit = 30; // NOLINT
    style.CompactControlStatements = true;

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
}

TEST(CompactControlStatements, PreservesNewlines) {
    Style style;
    style.CompactControlStatements = true;

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
}

TEST(ConcurrentAssertionStatement, Restrict) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          restrict property (@(posedge clk) !$isunknown(addr_i));
        endmodule
    )"), style), dedent(R"(
        module foo;
          restrict property (@(posedge clk) !$isunknown(addr_i));
        endmodule
    )"));
}

TEST(ConditionalStatement, BareBodies) {
    const Style style;

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
}

TEST(ForLoopStatement, BareBody) {
    const Style style;

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
}

TEST(IndentCaseItem, Disabled) {
    Style style;
    style.IndentCaseItem = false;

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
}

TEST(IndentCaseItem, Enabled) {
    Style style;
    style.IndentCaseItem = true;

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
}

TEST(IndentCaseItem, SameLineBlock) {
    Style style;
    style.IndentCaseItem = false;

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
}

TEST(NamedBlockClause, NormalizesSpacing) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          function int add(int a, int b);
            return a + b;
          endfunction:add
        endmodule
    )"), style), dedent(R"(
        module foo;
          function int add(int a, int b);
            return a + b;
          endfunction : add
        endmodule
    )"));
}

TEST(OneStatementPerLine, Disabled) {
    Style style;
    style.OneStatementPerLine = false;

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
}

TEST(OneStatementPerLine, Enabled) {
    Style style;
    style.OneStatementPerLine = true;

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
}

TEST(OneStatementPerLine, Members) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo; wire a; wire b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          wire a;
          wire b;
        endmodule
    )"));
}

TEST(RandSequenceStatement, CaseItems) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        initial begin
        randsequence (main)
        main : case (counter % 2) 0: alpha; default: beta; endcase;
        endsequence
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            randsequence (main)
              main : case (counter % 2)
                0 : alpha;
                default : beta;
              endcase;
            endsequence
          end
        endmodule
    )"));
}

TEST(RandSequenceStatement, Productions) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        initial begin
        randsequence (main)
             main : first second third;
          first    :  {$display("first");};
          second : rand join (0.5) alpha beta;
          alpha  : {$display("alpha");};
          beta   : repeat (2) alpha;
        endsequence
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            randsequence (main)
              main : first second third;
              first : {$display("first");};
              second : rand join (0.5) alpha beta;
              alpha : {$display("alpha");};
              beta : repeat (2) alpha;
            endsequence
          end
        endmodule
    )"));
}

TEST(SpaceAfterCaseColon, Collapses) {
    Style style;
    style.SpaceAfterCaseColon = true;

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
}

TEST(SpaceAfterCaseColon, FormatOff) {
    Style style;
    style.SpaceAfterCaseColon = true;

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
}

TEST(SpaceAfterCaseColon, Inserts) {
    Style style;
    style.SpaceAfterCaseColon = true;

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
}

TEST(SpaceAfterCaseColon, PreservesNewlines) {
    Style style;
    style.SpaceAfterCaseColon = true;

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
}

TEST(SpaceAfterCaseColon, Removes) {
    Style style;
    style.SpaceAfterCaseColon = false;

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
}

TEST(SpaceAfterParensControlStatements, Collapses) {
    Style style;
    style.SpaceAfterParens.ControlStatements = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            if (a)   begin
            end

            for (int i = 0; i < 4; i++)   begin
            end

            foreach (arr[i])   begin
            end

            while (a)   begin
            end

            case (x)   matches
              default:;
            endcase
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            if (a) begin
            end

            for (int i = 0; i < 4; i++) begin
            end

            foreach (arr[i]) begin
            end

            while (a) begin
            end

            case (x) matches
              default:;
            endcase
          end
        endmodule
    )"));
}

TEST(SpaceAfterParensControlStatements, FormatOff) {
    const Style style;

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
}

TEST(SpaceAfterParensControlStatements, Inserts) {
    Style style;
    style.SpaceAfterParens.ControlStatements = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            if (a)begin
            end

            for (int i = 0; i < 4; i++)begin
            end

            foreach (arr[i])begin
            end

            while (a)begin
            end

            case (x)matches
              default:;
            endcase
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            if (a) begin
            end

            for (int i = 0; i < 4; i++) begin
            end

            foreach (arr[i]) begin
            end

            while (a) begin
            end

            case (x) matches
              default:;
            endcase
          end
        endmodule
    )"));
}

TEST(SpaceAfterParensControlStatements, PreservesNewlines) {
    const Style style;

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
}

TEST(SpaceAfterParensControlStatements, Removes) {
    Style style;
    style.SpaceAfterParens.ControlStatements = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            if (a) begin
            end

            for (int i = 0; i < 4; i++) begin
            end

            foreach (arr[i]) begin
            end

            while (a) begin
            end

            case (x) matches
              default:;
            endcase
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            if (a)begin
            end

            for (int i = 0; i < 4; i++)begin
            end

            foreach (arr[i])begin
            end

            while (a)begin
            end

            case (x)matches
              default:;
            endcase
          end
        endmodule
    )"));
}

TEST(SpaceBeforeCaseColon, Collapses) {
    Style style;
    style.SpaceBeforeCaseColon = true;

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
}

TEST(SpaceBeforeCaseColon, FormatOff) {
    Style style;
    style.SpaceBeforeCaseColon = true;

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
}

TEST(SpaceBeforeCaseColon, Inserts) {
    Style style;
    style.SpaceBeforeCaseColon = true;

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
}

TEST(SpaceBeforeCaseColon, PreservesNewlines) {
    Style style;
    style.SpaceBeforeCaseColon = true;

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
}

TEST(SpaceBeforeCaseColon, Removes) {
    Style style;
    style.SpaceBeforeCaseColon = false;

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
}

TEST(SpaceBeforeParensControlStatements, Collapses) {
    Style style;
    style.SpaceBeforeParens.ControlStatements = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            if   (a) begin
            end

            for   (int i = 0; i < 4; i++) begin
            end

            foreach   (arr[i]) begin
            end

            while   (a) begin
            end

            do begin
            end while   (a);

            case   (x)
              default:;
            endcase
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            if (a) begin
            end

            for (int i = 0; i < 4; i++) begin
            end

            foreach (arr[i]) begin
            end

            while (a) begin
            end

            do begin
            end while (a);

            case (x)
              default:;
            endcase
          end
        endmodule
    )"));
}

TEST(SpaceBeforeParensControlStatements, FormatOff) {
    const Style style;

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
}

TEST(SpaceBeforeParensControlStatements, Inserts) {
    Style style;
    style.SpaceBeforeParens.ControlStatements = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            if(a) begin
            end

            for(int i = 0; i < 4; i++) begin
            end

            foreach(arr[i]) begin
            end

            while(a) begin
            end

            do begin
            end while(a);

            case(x)
              default:;
            endcase
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            if (a) begin
            end

            for (int i = 0; i < 4; i++) begin
            end

            foreach (arr[i]) begin
            end

            while (a) begin
            end

            do begin
            end while (a);

            case (x)
              default:;
            endcase
          end
        endmodule
    )"));
}

TEST(SpaceBeforeParensControlStatements, PreservesNewlines) {
    const Style style;

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
}

TEST(SpaceBeforeParensControlStatements, Removes) {
    Style style;
    style.SpaceBeforeParens.ControlStatements = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            if (a) begin
            end

            for (int i = 0; i < 4; i++) begin
            end

            foreach (arr[i]) begin
            end

            while (a) begin
            end

            do begin
            end while (a);

            case (x)
              default:;
            endcase
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            if(a) begin
            end

            for(int i = 0; i < 4; i++) begin
            end

            foreach(arr[i]) begin
            end

            while(a) begin
            end

            do begin
            end while(a);

            case(x)
              default:;
            endcase
          end
        endmodule
    )"));
}

TEST(SpaceBeforeParensControlStatements, UniqueIf) {
    const Style style;

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
}
// clang-format on
