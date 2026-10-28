// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "Style.h"
#include "TestHelper.h"

#include <gtest/gtest.h>

using namespace slang::format;

// clang-format off
TEST(InferFromSource, Disabled) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true};

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

TEST(InferFromSource, FlushLeftSource) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true, .InferFromSource = true, .MaxPadding = 2};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1;
          assign longname = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;
          assign longname = 2;
        endmodule
    )"));
}

TEST(InferFromSource, LargeExcessAligns) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true, .InferFromSource = true, .MaxPadding = 2};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x          = 1;
          assign longname = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x        = 1;
          assign longname = 2;
        endmodule
    )"));
}

TEST(InferFromSource, SmallDisplacementAligns) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true, .InferFromSource = true, .MaxPadding = 4};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign a = 1;
          assign bb = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign a = 1;
          assign bb = 2;
        endmodule
    )"));
}

TEST(MaxPadding, Exceeded) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true, .MaxPadding = 2};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = 1;
          assign longname = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;
          assign longname = 2;
        endmodule
    )"));
}

TEST(MaxPadding, MixedDepths) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true, .MaxPadding = 2};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          always_comb begin
            clk_count <= 0;
            data_count <= 0;
            if (enable) begin
              x = 1;
              longname = 2;
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            clk_count  <= 0;
            data_count <= 0;
            if (enable) begin
              x = 1;
              longname = 2;
            end
          end
        endmodule
    )"));
}

TEST(MaxPadding, WithinLimit) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true, .MaxPadding = 2};
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo #(
            parameter CLKS_PER_BIT = 4,
            parameter DATA_WIDTH = 8
        ) (
            input clk_i
        );
        endmodule
    )"), style), dedent(R"(
        module foo #(
            parameter CLKS_PER_BIT = 4,
            parameter DATA_WIDTH   = 8
        ) (
            input clk_i
        );
        endmodule
    )"));
}

TEST(MinPadding, BypassesInference) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true,
                                         .InferFromSource = true,
                                         .MaxPadding = 2,
                                         .MinPadding = 2};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign state_d = state_q;
          assign rdy_o = 1'b0;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign state_d = state_q;
          assign rdy_o   = 1'b0;
        endmodule
    )"));
}

TEST(MinPadding, ExceedsThreshold) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true,
                                         .InferFromSource = true,
                                         .MaxPadding = 4,
                                         .MinPadding = 1};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign a = 1;
          assign bbb = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign a = 1;
          assign bbb = 2;
        endmodule
    )"));
}

TEST(MinPadding, InterveningStatementsBreakSubRun) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true,
                                         .InferFromSource = true,
                                         .MaxPadding = 2,
                                         .MinPadding = 2};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          task automatic drive(input logic [7:0] a);
            @(cb);
            cb.req <= 1'b1;
            cb.addr <= a;
            wait (gnt == 1'b1);
            @(cb);
            cb.req <= 1'b0;
          endtask
        endmodule
    )"), style), dedent(R"(
        module foo;
          task automatic drive(input logic [7:0] a);
            @(cb);
            cb.req  <= 1'b1;
            cb.addr <= a;
            wait (gnt == 1'b1);
            @(cb);
            cb.req <= 1'b0;
          endtask
        endmodule
    )"));
}

TEST(MinPadding, PortConnections) {
    Style style;
    style.AlignConsecutivePortConnections = {.Enabled = true,
                                              .InferFromSource = true,
                                              .MaxPadding = 2,
                                              .MinPadding = 2};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          bar u_bar (
              .a(x),
              .bbb(y)
          );
        endmodule
    )"), style), dedent(R"(
        module foo;
          bar u_bar (
              .a  (x),
              .bbb(y)
          );
        endmodule
    )"));
}

TEST(MinPadding, Zero) {
    Style style;
    style.AlignConsecutiveAssignments = {.Enabled = true,
                                         .InferFromSource = true,
                                         .MaxPadding = 2};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign state_d = state_q;
          assign rdy_o = 1'b0;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign state_d = state_q;
          assign rdy_o = 1'b0;
        endmodule
    )"));
}
// clang-format on
