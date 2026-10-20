// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "TestHelper.h"
#include "slang/format/Format.h"
#include "slang/format/Style.h"
#include "slang/format/styles/Default.h"

#include <gtest/gtest.h>

using namespace slang::format;
using namespace slang::format::style;

// clang-format off
TEST(BreakAfterAssertionLabel, ConcurrentAssertion) {
    const auto style = getDefaultStyle();

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
    auto style = getDefaultStyle();
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
    const auto style = getDefaultStyle();

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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    const auto style = getDefaultStyle();

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
    const auto style = getDefaultStyle();

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
    const auto style = getDefaultStyle();

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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    const auto style = getDefaultStyle();

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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    const auto style = getDefaultStyle();

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
    const auto style = getDefaultStyle();

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
    const auto style = getDefaultStyle();

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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    const auto style = getDefaultStyle();

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
    auto style = getDefaultStyle();
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
    const auto style = getDefaultStyle();

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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    const auto style = getDefaultStyle();

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
    auto style = getDefaultStyle();
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
    const auto style = getDefaultStyle();

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
    auto style = getDefaultStyle();
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
    const auto style = getDefaultStyle();

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
