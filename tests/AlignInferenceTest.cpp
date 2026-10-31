// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "slang/format/Format.h"
#include "slang/format/Style.h"
#include "slang/format/styles/Default.h"
#include "slang/util/Dedent.h"

#include <gtest/gtest.h>

using namespace slang::format;
using slang::dedent;

TEST(InferFromSource, Disabled) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;

    // clang-format off
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
    // clang-format on
}

TEST(InferFromSource, FlushLeftSource) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveAssignments.InferFromSource = true;
    style.AlignConsecutiveAssignments.MaxPadding = 2;

    // clang-format off
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
    // clang-format on
}

TEST(InferFromSource, LargeExcessAligns) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveAssignments.InferFromSource = true;
    style.AlignConsecutiveAssignments.MaxPadding = 2;

    // clang-format off
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
    // clang-format on
}

TEST(InferFromSource, SmallDisplacementAligns) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveAssignments.InferFromSource = true;
    style.AlignConsecutiveAssignments.MaxPadding = 4;

    // clang-format off
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
    // clang-format on
}

TEST(MaxPadding, Exceeded) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveAssignments.MaxPadding = 2;

    // clang-format off
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
    // clang-format on
}

TEST(MaxPadding, MixedDepths) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveAssignments.MaxPadding = 2;

    // clang-format off
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
    // clang-format on
}

TEST(MaxPadding, WithinLimit) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveAssignments.MaxPadding = 2;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
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
    // clang-format on
}

TEST(MinPadding, BypassesInference) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveAssignments.InferFromSource = true;
    style.AlignConsecutiveAssignments.MaxPadding = 2;
    style.AlignConsecutiveAssignments.MinPadding = 2;

    // clang-format off
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
    // clang-format on
}

TEST(MinPadding, ExceedsThreshold) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveAssignments.InferFromSource = true;
    style.AlignConsecutiveAssignments.MaxPadding = 4;
    style.AlignConsecutiveAssignments.MinPadding = 1;

    // clang-format off
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
    // clang-format on
}

TEST(MinPadding, InterveningStatementsBreakSubRun) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveAssignments.InferFromSource = true;
    style.AlignConsecutiveAssignments.MaxPadding = 2;
    style.AlignConsecutiveAssignments.MinPadding = 2;

    // clang-format off
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
    // clang-format on
}

TEST(MinPadding, PortConnections) {
    auto style = getDefaultStyle();
    style.AlignConsecutivePortConnections = {};
    style.AlignConsecutivePortConnections.Enabled = true;
    style.AlignConsecutivePortConnections.InferFromSource = true;
    style.AlignConsecutivePortConnections.MaxPadding = 2;
    style.AlignConsecutivePortConnections.MinPadding = 2;

    // clang-format off
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
    // clang-format on
}

TEST(MinPadding, Zero) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};
    style.AlignConsecutiveAssignments.Enabled = true;
    style.AlignConsecutiveAssignments.InferFromSource = true;
    style.AlignConsecutiveAssignments.MaxPadding = 2;

    // clang-format off
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
    // clang-format on
}
