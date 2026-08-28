// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "Style.h"
#include "TestHelper.h"

#include <string_view>

#include <gtest/gtest.h>

using namespace slang::format;

TEST(ApplyInsertBeginEnd, AlreadyWrapped) {
    Style style;
    style.InsertBeginEnd.Enabled = true;
    style.InsertBeginEnd.AlwaysStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        if (a)
        begin
        x = 1;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            if (a)
            begin
              x = 1;
            end
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyInsertBeginEnd, AlwaysAlreadyWrapped) {
    Style style;
    style.InsertBeginEnd.Enabled = true;

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

TEST(ApplyInsertBeginEnd, AlwaysComb) {
    Style style;
    style.InsertBeginEnd.Enabled = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        x = 1;
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

TEST(ApplyInsertBeginEnd, AlwaysFF) {
    Style style;
    style.InsertBeginEnd.Enabled = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_ff @(posedge clk)
        x <= 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_ff @(posedge clk) begin
            x <= 1;
          end
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyInsertBeginEnd, AlwaysStatementsDisabled) {
    Style style;
    style.InsertBeginEnd.Enabled = true;
    style.InsertBeginEnd.AlwaysStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyInsertBeginEnd, AlwaysWithControlStatement) {
    Style style;
    style.InsertBeginEnd.Enabled = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always @(*) if (a) x = 1; else y = 0;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always @(*) begin
            if (a) begin
              x = 1;
            end else begin
              y = 0;
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyInsertBeginEnd, AlwaysWithNestedControl) {
    Style style;
    style.InsertBeginEnd.Enabled = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        if (a)
        x = 1;
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

TEST(ApplyInsertBeginEnd, CaseItem) {
    Style style;
    style.InsertBeginEnd.Enabled = true;
    style.InsertBeginEnd.AlwaysStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        case (a)
        1: x = 1;
        default: x = 0;
        endcase
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            case (a)
              1: begin
                x = 1;
              end
              default: begin
                x = 0;
              end
            endcase
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyInsertBeginEnd, CaseItemAlreadyWrapped) {
    Style const style;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        case (a)
        1: begin
        x = 1;
        end
        endcase
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            case (a)
              1: begin
                x = 1;
              end
            endcase
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyInsertBeginEnd, ControlStatementsDisabled) {
    Style style;
    style.InsertBeginEnd.ControlStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        if (a)
        x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            if (a)
              x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyInsertBeginEnd, DeepNesting) {
    Style style;
    style.InsertBeginEnd.Enabled = true;
    style.InsertBeginEnd.AlwaysStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        if (a)
        if (b)
        if (c)
        x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            if (a) begin
              if (b) begin
                if (c) begin
                  x = 1;
                end
              end
            end
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyInsertBeginEnd, DoWhile) {
    Style style;
    style.InsertBeginEnd.Enabled = true;
    style.InsertBeginEnd.InitialStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        initial
        do
        x = 1;
        while (a);
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial
            do begin
              x = 1;
            end
              while (a);
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyInsertBeginEnd, ElseIf) {
    Style style;
    style.InsertBeginEnd.Enabled = true;
    style.InsertBeginEnd.AlwaysStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        if (a)
        x = 1;
        else if (b)
        y = 2;
        else
        z = 3;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            if (a) begin
              x = 1;
            end
            else if (b) begin
              y = 2;
            end
            else begin
              z = 3;
            end
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyInsertBeginEnd, EnabledFalse) {
    Style style;
    style.InsertBeginEnd.Enabled = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        if (a)
        x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            if (a)
              x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyInsertBeginEnd, FinalBlock) {
    Style style;
    style.InsertBeginEnd.Enabled = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        final
        x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          final begin
            x = 1;
          end
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyInsertBeginEnd, ForLoop) {
    Style style;
    style.InsertBeginEnd.Enabled = true;
    style.InsertBeginEnd.AlwaysStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        for (int i = 0; i < 4; i++)
        x = i;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            for (int i = 0; i < 4; i++) begin
              x = i;
            end
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyInsertBeginEnd, IfElse) {
    Style style;
    style.InsertBeginEnd.Enabled = true;
    style.InsertBeginEnd.AlwaysStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        if (a)
        x = 1;
        else
        y = 0;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            if (a) begin
              x = 1;
            end
            else begin
              y = 0;
            end
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyInsertBeginEnd, IfSingleStatement) {
    Style style;
    style.InsertBeginEnd.Enabled = true;
    style.InsertBeginEnd.AlwaysStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        if (a)
        x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            if (a) begin
              x = 1;
            end
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyInsertBeginEnd, InitialAlreadyWrapped) {
    Style style;
    style.InsertBeginEnd.Enabled = true;

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

TEST(ApplyInsertBeginEnd, InitialBlock) {
    Style style;
    style.InsertBeginEnd.Enabled = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        initial
        x = 1;
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

TEST(ApplyInsertBeginEnd, InitialStatementsDisabled) {
    Style style;
    style.InsertBeginEnd.Enabled = true;
    style.InsertBeginEnd.InitialStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        initial
        x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial
            x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyInsertBeginEnd, NestedIf) {
    Style style;
    style.InsertBeginEnd.Enabled = true;
    style.InsertBeginEnd.AlwaysStatements = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb
        if (a)
        if (b)
        x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb
            if (a) begin
              if (b) begin
                x = 1;
              end
            end
        endmodule
    )"));
    // clang-format on
}
