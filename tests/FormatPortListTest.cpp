// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "slang/format/Format.h"
#include "slang/format/Style.h"
#include "slang/format/styles/Default.h"
#include "slang/util/Dedent.h"

#include <gtest/gtest.h>

using namespace slang::format;
using slang::dedent;

TEST(BinPackParameters, Disabled) {
    auto style = getDefaultStyle();
    style.BinPackParameters = false;

    // clang-format off
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
    // clang-format on
}

TEST(BinPackParameters, Fits) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(BinPackParameters, WrapsAtColumnLimit) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(BinPackPortConnections, DisabledExpandsParameters) {
    auto style = getDefaultStyle();
    style.BinPackPortConnections = false;

    // clang-format off
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
    // clang-format on
}

TEST(BinPackPortConnections, DisabledMultilineWildcard) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(BinPackPortConnections, DisabledPositionalParameters) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(BinPackPortConnections, DisabledSingleNamedParameter) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(BinPackPortConnections, DisabledSinglePositionalParameter) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar #(DW) u_bar;
        endmodule
    )"), style), dedent(R"(
        module foo;
          bar #(DW) u_bar;
        endmodule
    )"));
    // clang-format on
}

TEST(BinPackPortConnections, DisabledWildcard) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar #(.W(DW)) u_bar (.*);
        endmodule
    )"), style), dedent(R"(
        module foo;
          bar #(.W(DW)) u_bar (.*);
        endmodule
    )"));
    // clang-format on
}

TEST(BinPackPortConnections, EnabledFits) {
    auto style = getDefaultStyle();
    style.BinPackPortConnections = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        bar u_bar (.clk(clk), .rst_n(rst_n));
        endmodule
    )"), style), dedent(R"(
        module foo;
          bar u_bar (.clk(clk), .rst_n(rst_n));
        endmodule
    )"));
    // clang-format on
}

TEST(BinPackPortConnections, EnabledPositional) {
    auto style = getDefaultStyle();
    style.BinPackPortConnections = true;

    // clang-format off
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
    // clang-format on
}

TEST(BinPackPortConnections, EnabledPreservesNewlines) {
    auto style = getDefaultStyle();
    style.BinPackPortConnections = true;

    // clang-format off
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
    // clang-format on
}

TEST(ModportItem, NoSpaceBeforeParen) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(ModportItem, PortLists) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(SpaceBeforeParensParameterList, Collapses) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.SpaceBeforeParens.ParameterList = true;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceBeforeParensParameterList, FormatOff) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(SpaceBeforeParensParameterList, Inserts) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
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
    // clang-format on
}

TEST(SpaceBeforeParensParameterList, InstantiationInserts) {
    auto style = getDefaultStyle();
    style.SpaceBeforeParens.ParameterList = true;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceBeforeParensParameterList, InstantiationRemoves) {
    auto style = getDefaultStyle();
    style.SpaceBeforeParens.ParameterList = false;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceBeforeParensParameterList, PreservesNewlines) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
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
    // clang-format on
}

TEST(SpaceBeforeParensParameterList, Removes) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.SpaceBeforeParens.ParameterList = false;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceBeforeParensPortList, Collapses) {
    auto style = getDefaultStyle();
    style.SpaceBeforeParens.PortList = true;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceBeforeParensPortList, FormatOff) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(SpaceBeforeParensPortList, Inserts) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(SpaceBeforeParensPortList, InstantiationInserts) {
    auto style = getDefaultStyle();
    style.SpaceBeforeParens.PortList = true;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceBeforeParensPortList, InstantiationRemoves) {
    auto style = getDefaultStyle();
    style.SpaceBeforeParens.PortList = false;

    // clang-format off
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
    // clang-format on
}

TEST(SpaceBeforeParensPortList, PreservesNewlines) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(SpaceBeforeParensPortList, Removes) {
    auto style = getDefaultStyle();
    style.SpaceBeforeParens.PortList = false;

    // clang-format off
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
    // clang-format on
}
