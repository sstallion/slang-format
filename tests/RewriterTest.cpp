// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "Style.h"
#include "TestHelper.h"

#include <string_view>

#include <gtest/gtest.h>

using namespace slang::format;

TEST(ApplyEventSeparator, CommaPreservesExistingComma) {
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

TEST(ApplyEventSeparator, CommaRewritesOrToComma) {
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

TEST(ApplyEventSeparator, EdgeQualifiedSignals) {
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

TEST(ApplyEventSeparator, IffClause) {
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

TEST(ApplyEventSeparator, MixedSeparators) {
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

TEST(ApplyEventSeparator, MultipleSignals) {
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

TEST(ApplyEventSeparator, OrPreservesExistingOr) {
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

TEST(ApplyEventSeparator, OrRewritesCommaToOr) {
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

TEST(ApplyEventSeparator, PreserveDoesNothing) {
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

TEST(ApplyPackedDimensionBounds, EqualBoundsPreserved) {
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

TEST(ApplyPackedDimensionBounds, ImplicitType) {
    Style style;
    style.PackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo(
        input [0:7] x
        );
        endmodule
    )"), style), dedent(R"(
        module foo(
          input [7:0] x
        );
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyPackedDimensionBounds, LSBFirstPreservesCorrectOrder) {
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

TEST(ApplyPackedDimensionBounds, LSBFirstSwapsWhenNeeded) {
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

TEST(ApplyPackedDimensionBounds, MSBFirstPreservesCorrectOrder) {
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

TEST(ApplyPackedDimensionBounds, MSBFirstSwapsWhenNeeded) {
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

TEST(ApplyPackedDimensionBounds, MultipleDimensions) {
    Style style;
    style.PackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [0:7][0:3] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0][3:0] x;
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyPackedDimensionBounds, NonLiteralBoundsPreserved) {
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

TEST(ApplyPackedDimensionBounds, PreserveDoesNothing) {
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

TEST(ApplyPackedDimensionBounds, UnpackedDimensionSkipped) {
    Style style;
    style.PackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic x [0:7];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic x [0:7];
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyUnpackedDimensionBounds, EqualBoundsPreserved) {
    Style style;
    style.UnpackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic x [0:0];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic x [0:0];
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyUnpackedDimensionBounds, LSBFirstPreservesCorrectOrder) {
    Style style;
    style.UnpackedDimensionBounds = DimensionBoundsStyle::LSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic x [0:7];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic x [0:7];
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyUnpackedDimensionBounds, LSBFirstSwapsWhenNeeded) {
    Style style;
    style.UnpackedDimensionBounds = DimensionBoundsStyle::LSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic x [7:0];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic x [0:7];
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyUnpackedDimensionBounds, MSBFirstPreservesCorrectOrder) {
    Style style;
    style.UnpackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic x [7:0];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic x [7:0];
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyUnpackedDimensionBounds, MSBFirstSwapsWhenNeeded) {
    Style style;
    style.UnpackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic x [0:7];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic x [7:0];
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyUnpackedDimensionBounds, MultipleDimensions) {
    Style style;
    style.UnpackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic x [0:7][0:3];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic x [7:0][3:0];
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyUnpackedDimensionBounds, NonLiteralBoundsPreserved) {
    Style style;
    style.UnpackedDimensionBounds = DimensionBoundsStyle::MSBFirst;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic x [N-1:0];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic x [N-1:0];
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyUnpackedDimensionBounds, PackedDimensionSkipped) {
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

TEST(ApplyUnpackedDimensionBounds, PreserveDoesNothing) {
    Style style;
    style.UnpackedDimensionBounds = DimensionBoundsStyle::Preserve;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic x [0:7];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic x [0:7];
        endmodule
    )"));
    // clang-format on
}

TEST(ApplyInsertBeginEnd, AlreadyWrapped) {
    Style style;
    style.InsertBeginEnd.Enabled = true;
    style.InsertBeginEnd.AlwaysStatements = false;
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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
    Style style;
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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
    style.BreakBeforeInitial = BreakAfterBlockStyle::Never;

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
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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
    style.BreakBeforeAlways = BreakAfterBlockStyle::Never;

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

TEST(ApplyInsertParens, AllDisabledDoesNothing) {
    Style const style;

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

TEST(ApplyInsertParens, Delays) {
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

TEST(ApplyInsertParens, DelaysPreservesExisting) {
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

TEST(ApplyInsertParens, ExpressionEvents) {
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

TEST(ApplyInsertParens, ExpressionEventsPreservesExisting) {
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

TEST(ApplyInsertParens, ImplicitEvents) {
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

TEST(ApplyInsertParens, ImplicitEventsPreservesExisting) {
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

TEST(ApplyInsertParens, NamedEvents) {
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

TEST(ApplyInsertParens, NamedEventsPreservesExisting) {
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
