// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "Style.h"
#include "TestHelper.h"

#include <string_view>

#include <gtest/gtest.h>

using namespace slang::format;

TEST(EventSeparator, CommaIffClause) {
    Style style;
    style.EventSeparator = EventSeparatorStyle::Comma;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_ff @(posedge clk iff en or negedge rst)
        x <= 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_ff @(posedge clk iff en, negedge rst)
            x <= 1;
        endmodule
    )"));
    // clang-format on
}

TEST(EventSeparator, CommaMixedSeparators) {
    Style style;
    style.EventSeparator = EventSeparatorStyle::Comma;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_ff @(posedge clk or negedge rst, a)
        x <= 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_ff @(posedge clk, negedge rst, a)
            x <= 1;
        endmodule
    )"));
    // clang-format on
}

TEST(EventSeparator, CommaMultipleSignals) {
    Style style;
    style.EventSeparator = EventSeparatorStyle::Comma;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always @(a or b or c)
        x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always @(a, b, c)
            x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(EventSeparator, CommaPreservesComma) {
    Style style;
    style.EventSeparator = EventSeparatorStyle::Comma;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_ff @(posedge clk, negedge rst)
        x <= 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_ff @(posedge clk, negedge rst)
            x <= 1;
        endmodule
    )"));
    // clang-format on
}

TEST(EventSeparator, CommaRewritesOr) {
    Style style;
    style.EventSeparator = EventSeparatorStyle::Comma;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_ff @(posedge clk or negedge rst)
        x <= 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_ff @(posedge clk, negedge rst)
            x <= 1;
        endmodule
    )"));
    // clang-format on
}

TEST(EventSeparator, OrPreservesOr) {
    Style style;
    style.EventSeparator = EventSeparatorStyle::Or;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_ff @(posedge clk or negedge rst)
        x <= 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_ff @(posedge clk or negedge rst)
            x <= 1;
        endmodule
    )"));
    // clang-format on
}

TEST(EventSeparator, OrRewritesComma) {
    Style style;
    style.EventSeparator = EventSeparatorStyle::Or;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_ff @(posedge clk, negedge rst)
        x <= 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_ff @(posedge clk or negedge rst)
            x <= 1;
        endmodule
    )"));
    // clang-format on
}

TEST(EventSeparator, Preserve) {
    Style style;
    style.EventSeparator = EventSeparatorStyle::Preserve;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_ff @(posedge clk or negedge rst)
        x <= 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_ff @(posedge clk or negedge rst)
            x <= 1;
        endmodule
    )"));
    // clang-format on
}

TEST(InsertBeginEnd, AlwaysAlreadyWrapped) {
    Style style;
    style.InsertBeginEnd.AlwaysStatements = true;
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

TEST(InsertBeginEnd, AlwaysComb) {
    Style style;
    style.InsertBeginEnd.AlwaysStatements = true;
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

TEST(InsertBeginEnd, AlwaysFF) {
    Style style;
    style.InsertBeginEnd.AlwaysStatements = true;
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

TEST(InsertBeginEnd, AlwaysStatementsDisabled) {
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
          always_comb
            x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(InsertBeginEnd, AlwaysWithControlStatement) {
    Style style;
    style.InsertBeginEnd.AlwaysStatements = true;
    style.InsertBeginEnd.ControlStatements = true;
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

TEST(InsertBeginEnd, AlwaysWithNestedControl) {
    Style style;
    style.InsertBeginEnd.AlwaysStatements = true;
    style.InsertBeginEnd.ControlStatements = true;
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

TEST(InsertBeginEnd, CaseItem) {
    Style style;
    style.InsertBeginEnd.ControlStatements = true;
    style.InsertBeginEnd.Enabled = true;

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

TEST(InsertBeginEnd, CaseItemAlreadyWrapped) {
    Style style;
    style.InsertBeginEnd.ControlStatements = true;
    style.InsertBeginEnd.Enabled = true;

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

TEST(InsertBeginEnd, ControlStatementAlreadyWrapped) {
    Style style;
    style.InsertBeginEnd.ControlStatements = true;
    style.InsertBeginEnd.Enabled = true;

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

TEST(InsertBeginEnd, ControlStatementsDisabled) {
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
          always_comb
            if (a)
              x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(InsertBeginEnd, DeepNesting) {
    Style style;
    style.InsertBeginEnd.ControlStatements = true;
    style.InsertBeginEnd.Enabled = true;

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

TEST(InsertBeginEnd, Disabled) {
    const Style style;

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

TEST(InsertBeginEnd, DoWhile) {
    Style style;
    style.ContinuationIndentWidth = 2;
    style.InsertBeginEnd.ControlStatements = true;
    style.InsertBeginEnd.Enabled = true;

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

TEST(InsertBeginEnd, ElseIf) {
    Style style;
    style.InsertBeginEnd.ControlStatements = true;
    style.InsertBeginEnd.Enabled = true;

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

TEST(InsertBeginEnd, Final) {
    Style style;
    style.InsertBeginEnd.Enabled = true;
    style.InsertBeginEnd.InitialStatements = true;

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

TEST(InsertBeginEnd, ForLoop) {
    Style style;
    style.InsertBeginEnd.ControlStatements = true;
    style.InsertBeginEnd.Enabled = true;

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

TEST(InsertBeginEnd, If) {
    Style style;
    style.InsertBeginEnd.ControlStatements = true;
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
          always_comb
            if (a) begin
              x = 1;
            end
        endmodule
    )"));
    // clang-format on
}

TEST(InsertBeginEnd, IfElse) {
    Style style;
    style.InsertBeginEnd.ControlStatements = true;
    style.InsertBeginEnd.Enabled = true;

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

TEST(InsertBeginEnd, Initial) {
    Style style;
    style.InsertBeginEnd.Enabled = true;
    style.InsertBeginEnd.InitialStatements = true;

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

TEST(InsertBeginEnd, InitialAlreadyWrapped) {
    Style style;
    style.InsertBeginEnd.Enabled = true;
    style.InsertBeginEnd.InitialStatements = true;

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

TEST(InsertBeginEnd, InitialStatementsDisabled) {
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
          initial
            x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(InsertParens, Delays) {
    Style style;
    style.InsertParens.Delays = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always #5 clk = ~clk;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always #(5) clk = ~clk;
        endmodule
    )"));
    // clang-format on
}

TEST(InsertParens, DelaysPreservesParens) {
    Style style;
    style.InsertParens.Delays = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always #(5) clk = ~clk;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always #(5) clk = ~clk;
        endmodule
    )"));
    // clang-format on
}

TEST(InsertParens, Disabled) {
    Style style;
    style.InsertParens = {};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always @* x = 1;
        always @signal x = 1;
        always #5 clk = ~clk;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always @* x = 1;
          always @signal x = 1;
          always #5 clk = ~clk;
        endmodule
    )"));
    // clang-format on
}

TEST(InsertParens, ExpressionEvents) {
    Style style;
    style.InsertParens.ExpressionEvents = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        class foo;
        covergroup cg @clk;
        endgroup
        endclass
    )"), style), dedent(R"(
        class foo;
          covergroup cg @(clk);
          endgroup
        endclass
    )"));
    // clang-format on
}

TEST(InsertParens, ExpressionEventsPreservesParens) {
    Style style;
    style.InsertParens.ExpressionEvents = true;

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

TEST(InsertParens, ImplicitEvents) {
    Style style;
    style.InsertParens.ImplicitEvents = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always @* x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always @(*) x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(InsertParens, ImplicitEventsPreservesParens) {
    Style style;
    style.InsertParens.ImplicitEvents = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always @(*) x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always @(*) x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(InsertParens, NamedEvents) {
    Style style;
    style.InsertParens.NamedEvents = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always @signal x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always @(signal) x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(InsertParens, NamedEventsPreservesParens) {
    Style style;
    style.InsertParens.NamedEvents = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always @(signal) x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          always @(signal) x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(PackedDimensionBounds, EqualBoundsPreserved) {
    Style style;
    style.PackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [0:0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [0:0] x;
        endmodule
    )"));
    // clang-format on
}

TEST(PackedDimensionBounds, IgnoresUnpackedDimensions) {
    Style style;
    style.PackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic x [0:7];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic x[0:7];
        endmodule
    )"));
    // clang-format on
}

TEST(PackedDimensionBounds, ImplicitType) {
    Style style;
    style.PackedDimensionBounds = DimensionBoundsStyle::MSBFirst;
    style.ParameterPortListIndentWidth = 2;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo(
        input [0:7] x
        );
        endmodule
    )"), style), dedent(R"(
        module foo (
          input [7:0] x
        );
        endmodule
    )"));
    // clang-format on
}

TEST(PackedDimensionBounds, LSBFirstPreservesOrder) {
    Style style;
    style.PackedDimensionBounds = DimensionBoundsStyle::LSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [0:7] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [0:7] x;
        endmodule
    )"));
    // clang-format on
}

TEST(PackedDimensionBounds, LSBFirstSwaps) {
    Style style;
    style.PackedDimensionBounds = DimensionBoundsStyle::LSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [7:0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [0:7] x;
        endmodule
    )"));
    // clang-format on
}

TEST(PackedDimensionBounds, MSBFirstPreservesOrder) {
    Style style;
    style.PackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [7:0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] x;
        endmodule
    )"));
    // clang-format on
}

TEST(PackedDimensionBounds, MSBFirstSwaps) {
    Style style;
    style.PackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [0:7] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] x;
        endmodule
    )"));
    // clang-format on
}

TEST(PackedDimensionBounds, MultipleDimensions) {
    Style style;
    style.PackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [0:7][0:3] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] [3:0] x;
        endmodule
    )"));
    // clang-format on
}

TEST(PackedDimensionBounds, NonLiteralBounds) {
    Style style;
    style.PackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [N-1:0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [N-1:0] x;
        endmodule
    )"));
    // clang-format on
}

TEST(PackedDimensionBounds, Preserve) {
    Style style;
    style.PackedDimensionBounds = DimensionBoundsStyle::Preserve;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [0:7] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [0:7] x;
        endmodule
    )"));
    // clang-format on
}

TEST(UnpackedDimensionBounds, EqualBoundsPreserved) {
    Style style;
    style.UnpackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic x [0:0];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic x[0:0];
        endmodule
    )"));
    // clang-format on
}

TEST(UnpackedDimensionBounds, IgnoresPackedDimensions) {
    Style style;
    style.UnpackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [0:7] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [0:7] x;
        endmodule
    )"));
    // clang-format on
}

TEST(UnpackedDimensionBounds, LSBFirstPreservesOrder) {
    Style style;
    style.UnpackedDimensionBounds = DimensionBoundsStyle::LSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic x [0:7];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic x[0:7];
        endmodule
    )"));
    // clang-format on
}

TEST(UnpackedDimensionBounds, LSBFirstSwaps) {
    Style style;
    style.UnpackedDimensionBounds = DimensionBoundsStyle::LSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic x [7:0];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic x[0:7];
        endmodule
    )"));
    // clang-format on
}

TEST(UnpackedDimensionBounds, MSBFirstPreservesOrder) {
    Style style;
    style.UnpackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic x [7:0];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic x[7:0];
        endmodule
    )"));
    // clang-format on
}

TEST(UnpackedDimensionBounds, MSBFirstSwaps) {
    Style style;
    style.UnpackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic x [0:7];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic x[7:0];
        endmodule
    )"));
    // clang-format on
}

TEST(UnpackedDimensionBounds, MultipleDimensions) {
    Style style;
    style.UnpackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic x [0:7][0:3];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic x[7:0][3:0];
        endmodule
    )"));
    // clang-format on
}

TEST(UnpackedDimensionBounds, NonLiteralBounds) {
    Style style;
    style.UnpackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic x [N-1:0];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic x[N-1:0];
        endmodule
    )"));
    // clang-format on
}

TEST(UnpackedDimensionBounds, Preserve) {
    Style style;
    style.UnpackedDimensionBounds = DimensionBoundsStyle::Preserve;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic x [0:7];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic x[0:7];
        endmodule
    )"));
    // clang-format on
}
