// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "slang/format/Format.h"
#include "slang/format/Style.h"
#include "slang/format/styles/Default.h"
#include "slang/util/Dedent.h"

#include <string_view>

#include <gtest/gtest.h>

using namespace slang::format;
using slang::dedent;

TEST(EventSeparator, CommaIffClause) {
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    const auto style = getDefaultStyle();

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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
    auto style = getDefaultStyle();
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
