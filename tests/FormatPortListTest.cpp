// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "Style.h"
#include "TestHelper.h"

#include <gtest/gtest.h>

using namespace slang::format;

// clang-format off
TEST(BinPackParameters, Disabled) {
    Style style;
    style.BinPackParameters = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        function void foo(input int a, input int b, input int c);
        endfunction
        endmodule
    )"), style), dedent(R"(
        module foo;
          function void foo(
              input int a,
              input int b,
              input int c
          );
          endfunction
        endmodule
    )"));
}

TEST(BinPackParameters, Fits) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        task automatic bump(ref int counter, input int unsigned n = 1);
        endtask
        endmodule
    )"), style), dedent(R"(
        module foo;
          task automatic bump(ref int counter, input int unsigned n = 1);
          endtask
        endmodule
    )"));
}

TEST(BinPackParameters, WrapsAtColumnLimit) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        function automatic logic [DW-1:0] mask_bytes(input logic [DW-1:0] value, input logic [STRB_W-1:0] strb, input bit invert = 1'b0);
        return '0;
        endfunction
        endmodule
    )"), style), dedent(R"(
        module foo;
          function automatic logic [DW-1:0] mask_bytes(
              input logic [DW-1:0] value, input logic [STRB_W-1:0] strb, input bit invert = 1'b0);
            return '0;
          endfunction
        endmodule
    )"));
}

TEST(BinPackPortConnections, DisabledExpandsParameters) {
    Style style;
    style.BinPackPortConnections = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar #(.W(8), .RST_VAL(8'hFF)) u_bar (.clk(clk), .rst_n(rst_n), .en(vld_i), .d(data_i[7:0]), .q(dff_q));
        endmodule
    )"), style), dedent(R"(
        module foo;
          bar #(
              .W(8),
              .RST_VAL(8'hFF)
          ) u_bar (
              .clk(clk),
              .rst_n(rst_n),
              .en(vld_i),
              .d(data_i[7:0]),
              .q(dff_q)
          );
        endmodule
    )"));
}

TEST(BinPackPortConnections, DisabledMultilineWildcard) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar #(
        .W(DW)
        ) u_bar (.*);
        endmodule
    )"), style), dedent(R"(
        module foo;
          bar #(
              .W(DW)
          ) u_bar (.*);
        endmodule
    )"));
}

TEST(BinPackPortConnections, DisabledPositionalParameters) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar #(8, 8'h00) u_bar (clk, rst_n, vld_i, data_i[15:8], dff_q2);
        endmodule
    )"), style), dedent(R"(
        module foo;
          bar #(8, 8'h00) u_bar (
              clk,
              rst_n,
              vld_i,
              data_i[15:8],
              dff_q2
          );
        endmodule
    )"));
}

TEST(BinPackPortConnections, DisabledSingleNamedParameter) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar #(.W(4)) u_bar[0:3] (.clk(clk), .rst_n(rst_n), .en(1'b1), .d(nib_d), .q(nib_q));
        endmodule
    )"), style), dedent(R"(
        module foo;
          bar #(
              .W(4)
          ) u_bar[0:3] (
              .clk(clk),
              .rst_n(rst_n),
              .en(1'b1),
              .d(nib_d),
              .q(nib_q)
          );
        endmodule
    )"));
}

TEST(BinPackPortConnections, DisabledSinglePositionalParameter) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar #(DW) u_bar;
        endmodule
    )"), style), dedent(R"(
        module foo;
          bar #(DW) u_bar;
        endmodule
    )"));
}

TEST(BinPackPortConnections, DisabledWildcard) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar #(.W(DW)) u_bar (.*);
        endmodule
    )"), style), dedent(R"(
        module foo;
          bar #(.W(DW)) u_bar (.*);
        endmodule
    )"));
}

TEST(BinPackPortConnections, EnabledFits) {
    Style style;
    style.BinPackPortConnections = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar u_bar (.clk(clk), .rst_n(rst_n));
        endmodule
    )"), style), dedent(R"(
        module foo;
          bar u_bar (.clk(clk), .rst_n(rst_n));
        endmodule
    )"));
}

TEST(BinPackPortConnections, EnabledPositional) {
    Style style;
    style.BinPackPortConnections = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar #(8, 8'h00) u_bar (
        clk,
        rst_n,
        en,
        d,
        q
        );
        endmodule
    )"), style), dedent(R"(
        module foo;
          bar #(8, 8'h00) u_bar (
              clk,
              rst_n,
              en,
              d,
              q
          );
        endmodule
    )"));
}

TEST(BinPackPortConnections, EnabledPreservesNewlines) {
    Style style;
    style.BinPackPortConnections = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar #(
        .W(8),
        .RST_VAL(8'hFF)
        ) u_bar (
        .clk(clk),
        .rst_n(rst_n),
        .d(data_i),
        .q(dff_q)
        );
        endmodule
    )"), style), dedent(R"(
        module foo;
          bar #(
              .W(8),
              .RST_VAL(8'hFF)
          ) u_bar (
              .clk(clk),
              .rst_n(rst_n),
              .d(data_i),
              .q(dff_q)
          );
        endmodule
    )"));
}

TEST(ModportItem, NoSpaceBeforeParen) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        interface foo;
          modport mst (input clk, output data);
          modport slv (input data, output ack);
        endinterface
    )"), style), dedent(R"(
        interface foo;
          modport mst(input clk, output data);
          modport slv(input data, output ack);
        endinterface
    )"));
}

TEST(ModportItem, PortLists) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        interface foo;
          modport mst(
               clocking cb,
            output req, addr, wdata, we,
            input gnt, rdata
          );
          modport slv(input req, output gnt);
        endinterface
    )"), style), dedent(R"(
        interface foo;
          modport mst(
              clocking cb,
              output req, addr, wdata, we,
              input gnt, rdata
          );
          modport slv(input req, output gnt);
        endinterface
    )"));
}

TEST(SpaceBeforeParensParameterList, Collapses) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.SpaceBeforeParens.ParameterList = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo   #(
          parameter N = 4
        )(
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
}

TEST(SpaceBeforeParensParameterList, FormatOff) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        // slang-format off
        module foo#(
          parameter N = 4
        )(
          input a
        );
        endmodule
        // slang-format on
    )"), style), dedent(R"(
        // slang-format off
        module foo#(
          parameter N = 4
        )(
          input a
        );
        endmodule
        // slang-format on
    )"));
}

TEST(SpaceBeforeParensParameterList, Inserts) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo#(
          parameter N = 4
        )(
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
}

TEST(SpaceBeforeParensParameterList, InstantiationInserts) {
    Style style;
    style.SpaceBeforeParens.ParameterList = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar#(
        .W(8)
        ) u_bar (
        .clk(clk)
        );
        endmodule
    )"), style), dedent(R"(
        module foo;
          bar #(
              .W(8)
          ) u_bar (
              .clk(clk)
          );
        endmodule
    )"));
}

TEST(SpaceBeforeParensParameterList, InstantiationRemoves) {
    Style style;
    style.SpaceBeforeParens.ParameterList = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar #(
        .W(8)
        ) u_bar (
        .clk(clk)
        );
        endmodule
    )"), style), dedent(R"(
        module foo;
          bar#(
              .W(8)
          ) u_bar (
              .clk(clk)
          );
        endmodule
    )"));
}

TEST(SpaceBeforeParensParameterList, PreservesNewlines) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo
        #(
          parameter N = 4
        )(
          input a
        );
        endmodule
    )"), style), dedent(R"(
        module foo
            #(
            parameter N = 4
        ) (
            input a
        );
        endmodule
    )"));
}

TEST(SpaceBeforeParensParameterList, Removes) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.SpaceBeforeParens.ParameterList = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo #(
          parameter N = 4
        )(
          input a
        );
        endmodule
    )"), style), dedent(R"(
        module foo#(
            parameter N = 4
        ) (
            input a
        );
        endmodule
    )"));
}

TEST(SpaceBeforeParensPortList, Collapses) {
    Style style;
    style.SpaceBeforeParens.PortList = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo   (
          input a
        );
        endmodule
    )"), style), dedent(R"(
        module foo (
            input a
        );
        endmodule
    )"));
}

TEST(SpaceBeforeParensPortList, FormatOff) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        // slang-format off
        module foo(
          input a
        );
        endmodule
        // slang-format on
    )"), style), dedent(R"(
        // slang-format off
        module foo(
          input a
        );
        endmodule
        // slang-format on
    )"));
}

TEST(SpaceBeforeParensPortList, Inserts) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo(
          input a
        );
        endmodule
    )"), style), dedent(R"(
        module foo (
            input a
        );
        endmodule
    )"));
}

TEST(SpaceBeforeParensPortList, InstantiationInserts) {
    Style style;
    style.SpaceBeforeParens.PortList = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar u_bar(
        .clk(clk)
        );
        endmodule
    )"), style), dedent(R"(
        module foo;
          bar u_bar (
              .clk(clk)
          );
        endmodule
    )"));
}

TEST(SpaceBeforeParensPortList, InstantiationRemoves) {
    Style style;
    style.SpaceBeforeParens.PortList = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar u_bar (
        .clk(clk)
        );
        endmodule
    )"), style), dedent(R"(
        module foo;
          bar u_bar(
              .clk(clk)
          );
        endmodule
    )"));
}

TEST(SpaceBeforeParensPortList, PreservesNewlines) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo
        (
          input a
        );
        endmodule
    )"), style), dedent(R"(
        module foo
            (
            input a
        );
        endmodule
    )"));
}

TEST(SpaceBeforeParensPortList, Removes) {
    Style style;
    style.SpaceBeforeParens.PortList = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo (
          input a
        );
        endmodule
    )"), style), dedent(R"(
        module foo(
            input a
        );
        endmodule
    )"));
}
// clang-format on
