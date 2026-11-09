// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "slang/format/Format.h"
#include "slang/format/Style.h"
#include "slang/format/styles/Default.h"
#include "slang/util/Dedent.h"

#include <gtest/gtest.h>

using namespace slang::format;
using slang::dedent;

TEST(BindDirective, Instantiation) {
    auto style = getDefaultStyle();
    style.AlignConsecutivePortConnections = {};
    style.BinPackPortConnections = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        bind foo bar u_bar (
        .clk(clk),
        .rst_n(rst_n),
        .req(req_int),
        .gnt(gnt_int)
        );
    )"), style), dedent(R"(
        bind foo bar u_bar (
            .clk(clk),
            .rst_n(rst_n),
            .req(req_int),
            .gnt(gnt_int)
        );
    )"));
    // clang-format on
}

TEST(BindDirective, TargetInstances) {
    auto style = getDefaultStyle();
    style.AlignConsecutivePortConnections = {};
    style.BinPackPortConnections = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        bind foo : u_foo bar u_bar (
        .clk(clk)
        );
    )"), style), dedent(R"(
        bind foo:u_foo bar u_bar (
            .clk(clk)
        );
    )"));
    // clang-format on
}

TEST(ContinuousAssign, Delay) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign #5 x = y;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign #5 x = y;
        endmodule
    )"));
    // clang-format on
}

TEST(ContinuousAssign, DriveStrength) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign (strong1, weak0) x = y;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign (strong1, weak0) x = y;
        endmodule
    )"));
    // clang-format on
}

TEST(DPIExport, Function) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        package foo;
        export "DPI-C" c_fn = function fn;
        endpackage
    )"), style), dedent(R"(
        package foo;
          export "DPI-C"c_fn=function fn;
        endpackage
    )"));
    // clang-format on
}

TEST(DPIImport, ExpandsPorts) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        package foo;
        import "DPI-C" context function int unsigned c_checksum(input int unsigned seed, input int unsigned len);
        endpackage
    )"), style), dedent(R"(
        package foo;
          import "DPI-C" context function int unsigned c_checksum(
            input int unsigned seed,
            input int unsigned len
          );
        endpackage
    )"));
    // clang-format on
}

TEST(DPIImport, SinglePortFits) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        package foo;
        import "DPI-C" function void single_port(input int x);
        endpackage
    )"), style), dedent(R"(
        package foo;
          import "DPI-C" function void single_port(input int x);
        endpackage
    )"));
    // clang-format on
}

TEST(DefParam, Assignments) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        defparam bar.x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          defparam bar.x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(DefaultClockingReference, Declaration) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        default clocking cb;
        endmodule
    )"), style), dedent(R"(
        module foo;
          default clocking cb;
        endmodule
    )"));
    // clang-format on
}

TEST(DefaultDisableDeclaration, Declaration) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        default disable iff rst;
        endmodule
    )"), style), dedent(R"(
        module foo;
          default disable iff rst;
        endmodule
    )"));
    // clang-format on
}

TEST(EnumType, ExpandsMembers) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        typedef enum bit [3:0] {OP_A[4] = 4'd0, OP_B[2:3]} op_e;
        endmodule
    )"), style), dedent(R"(
        module foo;
          typedef enum bit [3:0] {
            OP_A[4] = 4'd0,
            OP_B[2:3]
          } op_e;
        endmodule
    )"));
    // clang-format on
}

TEST(EnumType, NormalizesSpacing) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        package foo;
        typedef enum bit [1:0]{
        ST_IDLE,
        ST_REQ
        }state_e;
        endpackage
    )"), style), dedent(R"(
        package foo;
          typedef enum bit [1:0] {
            ST_IDLE,
            ST_REQ
          } state_e;
        endpackage
    )"));
    // clang-format on
}

TEST(EnumType, PreservesNewlines) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveAssignments = {};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        typedef enum logic [2:0] {
        ST_IDLE = 3'd0,
        ST_REQ = 3'd1
        } state_e;
        endmodule
    )"), style), dedent(R"(
        module foo;
          typedef enum logic [2:0] {
            ST_IDLE = 3'd0,
            ST_REQ = 3'd1
          } state_e;
        endmodule
    )"));
    // clang-format on
}

TEST(ForwardTypedefDeclaration, Class) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        package foo;
        typedef class bar;
        endpackage
    )"), style), dedent(R"(
        package foo;
          typedef class bar;
        endpackage
    )"));
    // clang-format on
}

TEST(GenvarDeclaration, Identifiers) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        genvar i, j;
        endmodule
    )"), style), dedent(R"(
        module foo;
          genvar i, j;
        endmodule
    )"));
    // clang-format on
}

TEST(NetDeclaration, Delay) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        wire #5 x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          wire #5 x;
        endmodule
    )"));
    // clang-format on
}

TEST(NetDeclaration, DriveStrength) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        wire (strong0, pull1) x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          wire (strong0, pull1) x;
        endmodule
    )"));
    // clang-format on
}

TEST(NetTypeDeclaration, WithFunction) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        nettype real mynet with resolve;
        endmodule
    )"), style), dedent(R"(
        module foo;
          nettype real mynet with resolve;
        endmodule
    )"));
    // clang-format on
}

TEST(PackageExportAllDeclaration, Declaration) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        package foo;
        export *::*;
        endpackage
    )"), style), dedent(R"(
        package foo;
          export*::*;
        endpackage
    )"));
    // clang-format on
}

TEST(PackageExportDeclaration, Items) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        package foo;
        export bar::baz;
        endpackage
    )"), style), dedent(R"(
        package foo;
          export bar::baz;
        endpackage
    )"));
    // clang-format on
}

TEST(StructUnionType, Members) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        package foo;
        typedef struct packed{
        logic valid;
        logic [3:0] strb;
        }beat_t;
        endpackage
    )"), style), dedent(R"(
        package foo;
          typedef struct packed {
            logic valid;
            logic [3:0] strb;
          } beat_t;
        endpackage
    )"));
    // clang-format on
}

TEST(TimeUnitsDeclaration, Divider) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          timeunit 1ns / 1ps;
        endmodule
    )"), style), dedent(R"(
        module foo;
          timeunit 1ns/1ps;
        endmodule
    )"));
    // clang-format on
}

TEST(UserDefinedNetDeclaration, Delay) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        mynet #1 x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          mynet#1 x;
        endmodule
    )"));
    // clang-format on
}
