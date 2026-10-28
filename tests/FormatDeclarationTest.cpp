// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "Style.h"
#include "TestHelper.h"

#include <gtest/gtest.h>

using namespace slang::format;

// clang-format off
TEST(BindDirective, Instantiation) {
    Style style;
    style.AlignConsecutivePortConnections = {};
    style.BinPackPortConnections = true;

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
}

TEST(DPIImport, ExpandsPorts) {
    const Style style;

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
}

TEST(DPIImport, SinglePortFits) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        package foo;
        import "DPI-C" function void single_port(input int x);
        endpackage
    )"), style), dedent(R"(
        package foo;
          import "DPI-C" function void single_port(input int x);
        endpackage
    )"));
}

TEST(EnumType, ExpandsMembers) {
    const Style style;

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
}

TEST(EnumType, NormalizesSpacing) {
    const Style style;

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
}

TEST(EnumType, PreservesNewlines) {
    Style style;
    style.AlignConsecutiveAssignments = {};

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
}

TEST(StructUnionType, Members) {
    Style style;
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

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
}

TEST(TimeUnitsDeclaration, Divider) {
    const Style style;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          timeunit 1ns / 1ps;
        endmodule
    )"), style), dedent(R"(
        module foo;
          timeunit 1ns/1ps;
        endmodule
    )"));
}
// clang-format on
