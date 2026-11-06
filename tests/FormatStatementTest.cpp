// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "slang/format/Format.h"
#include "slang/format/Style.h"
#include "slang/format/styles/Default.h"
#include "slang/util/Dedent.h"

#include <gtest/gtest.h>

using namespace slang::format;
using slang::dedent;

TEST(BreakAfterAssertionLabel, ConcurrentAssertion) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(BreakAfterAssertionLabel, Disabled) {
    auto style = getDefaultStyle();
    style.BreakAfterAssertionLabel = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        a1: assert property (p);
        endmodule
    )"), style), dedent(R"(
        module foo;
          a1 : assert property (p);
        endmodule
    )"));
    // clang-format on
}

TEST(BreakAfterAssertionLabel, ImmediateAssertionWithElse) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(BreakAfterBegin, Disabled) {
    auto style = getDefaultStyle();
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

TEST(BreakAfterBegin, Inserts) {
    auto style = getDefaultStyle();
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

TEST(BreakAfterBegin, Nested) {
    auto style = getDefaultStyle();
    style.BreakAfterBegin = true;

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

TEST(BreakAfterBegin, PreservesNewlines) {
    auto style = getDefaultStyle();
    style.BreakAfterBegin = true;

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

TEST(BreakBeforeEnd, Inserts) {
    auto style = getDefaultStyle();
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

TEST(CompactControlStatements, BlockBodies) {
    auto style = getDefaultStyle();
    style.CompactControlStatements = true;

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

TEST(CompactControlStatements, Disabled) {
    auto style = getDefaultStyle();
    style.CompactControlStatements = false;

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

TEST(CompactControlStatements, ExceedsColumnLimit) {
    auto style = getDefaultStyle();
    style.BreakAfterAlways = BlockBreakStyle::Always;
    style.ColumnLimit = 30; // NOLINT
    style.CompactControlStatements = true;

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

TEST(CompactControlStatements, If) {
    auto style = getDefaultStyle();
    style.CompactControlStatements = true;

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

TEST(CompactControlStatements, IfElse) {
    auto style = getDefaultStyle();
    style.CompactControlStatements = true;

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

TEST(CompactControlStatements, IfElseIfElse) {
    auto style = getDefaultStyle();
    style.CompactControlStatements = true;

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

TEST(CompactControlStatements, NoColumnLimit) {
    auto style = getDefaultStyle();
    style.ColumnLimit = 0;
    style.CompactControlStatements = true;

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

TEST(CompactControlStatements, PartiallyCompacts) {
    auto style = getDefaultStyle();
    style.ColumnLimit = 30; // NOLINT
    style.CompactControlStatements = true;

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

TEST(CompactControlStatements, PreservesNewlines) {
    auto style = getDefaultStyle();
    style.CompactControlStatements = true;

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

TEST(ConcurrentAssertionStatement, Restrict) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          restrict property (@(posedge clk) !$isunknown(addr_i));
        endmodule
    )"), style), dedent(R"(
        module foo;
          restrict property (@(posedge clk) !$isunknown(addr_i));
        endmodule
    )"));
    // clang-format on
}

TEST(ConditionalStatement, BareBodies) {
    const auto style = getDefaultStyle();

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

TEST(ForLoopStatement, BareBody) {
    const auto style = getDefaultStyle();

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

TEST(IndentCaseItem, Disabled) {
    auto style = getDefaultStyle();
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

TEST(IndentCaseItem, Enabled) {
    auto style = getDefaultStyle();
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

TEST(IndentCaseItem, SameLineBlock) {
    auto style = getDefaultStyle();
    style.IndentCaseItem = false;

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

TEST(NamedBlockClause, NormalizesSpacing) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(OneStatementPerLine, Disabled) {
    auto style = getDefaultStyle();
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

TEST(OneStatementPerLine, Enabled) {
    auto style = getDefaultStyle();
    style.OneStatementPerLine = true;

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

TEST(OneStatementPerLine, Members) {
    const auto style = getDefaultStyle();

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

TEST(RandSequenceStatement, CaseItems) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(RandSequenceStatement, Productions) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(SpaceAfterCaseColon, Collapses) {
    auto style = getDefaultStyle();
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

TEST(SpaceAfterCaseColon, FormatOff) {
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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

TEST(SpaceAfterCaseColon, PreservesNewlines) {
    auto style = getDefaultStyle();
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

TEST(SpaceAfterCaseColon, Removes) {
    auto style = getDefaultStyle();
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

TEST(SpaceAfterParensControlStatements, Collapses) {
    auto style = getDefaultStyle();
    style.SpaceAfterParens.ControlStatements = true;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceAfterParensControlStatements, FormatOff) {
    const auto style = getDefaultStyle();

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
    auto style = getDefaultStyle();
    style.SpaceAfterParens.ControlStatements = true;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceAfterParensControlStatements, PreservesNewlines) {
    const auto style = getDefaultStyle();

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

TEST(SpaceAfterParensControlStatements, Removes) {
    auto style = getDefaultStyle();
    style.SpaceAfterParens.ControlStatements = false;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceBeforeCaseColon, Collapses) {
    auto style = getDefaultStyle();
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

TEST(SpaceBeforeCaseColon, FormatOff) {
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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

TEST(SpaceBeforeCaseColon, PreservesNewlines) {
    auto style = getDefaultStyle();
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

TEST(SpaceBeforeCaseColon, Removes) {
    auto style = getDefaultStyle();
    style.SpaceBeforeCaseColon = false;

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

TEST(SpaceBeforeParensControlStatements, Collapses) {
    auto style = getDefaultStyle();
    style.SpaceBeforeParens.ControlStatements = true;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceBeforeParensControlStatements, FormatOff) {
    const auto style = getDefaultStyle();

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
    auto style = getDefaultStyle();
    style.SpaceBeforeParens.ControlStatements = true;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceBeforeParensControlStatements, PreservesNewlines) {
    const auto style = getDefaultStyle();

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

TEST(SpaceBeforeParensControlStatements, Removes) {
    auto style = getDefaultStyle();
    style.SpaceBeforeParens.ControlStatements = false;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceBeforeParensControlStatements, UniqueIf) {
    const auto style = getDefaultStyle();

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
