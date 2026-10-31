// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "slang/format/Format.h"
#include "slang/format/Style.h"
#include "slang/format/styles/Default.h"
#include "slang/util/Dedent.h"

#include <gtest/gtest.h>

using namespace slang::format;
using slang::dedent;

TEST(Comment, BeforeEnd) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin
        // comment
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            // comment
          end
        endmodule
    )"));
    // clang-format on
}

TEST(Comment, BeforeEndfunction) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        function void bar;
        // comment
        endfunction
        endmodule
    )"), style), dedent(R"(
        module foo;
          function void bar;
            // comment
          endfunction
        endmodule
    )"));
    // clang-format on
}

TEST(Comment, BeforeEndmodule) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        // comment
        endmodule
    )"), style), dedent(R"(
        module foo;
          // comment
        endmodule
    )"));
    // clang-format on
}

TEST(Comment, BlockCommentBeforeEndmodule) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        /* comment */
        endmodule
    )"), style), dedent(R"(
        module foo;
          /* comment */
        endmodule
    )"));
    // clang-format on
}

TEST(Comment, InBlock) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        always_comb begin
        // comment
        x = 1;
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          always_comb begin
            // comment
            x = 1;
          end
        endmodule
    )"));
    // clang-format on
}

TEST(ContinuationIndentWidth, Custom) {
    auto style = getDefaultStyle();
    style.ContinuationIndentWidth = 2;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = a +
          b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = a +
            b;
        endmodule
    )"));
    // clang-format on
}

TEST(ContinuationIndentWidth, Default) {
    auto style = getDefaultStyle();
    style.ContinuationIndentWidth = 4;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = a +
          b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = a +
              b;
        endmodule
    )"));
    // clang-format on
}

TEST(Directive, AfterModule) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign a = 1;
        endmodule
        `default_nettype wire
    )"), style), dedent(R"(
        module foo;
          assign a = 1;
        endmodule
        `default_nettype wire
    )"));
    // clang-format on
}

TEST(Directive, DefineGuard) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign a = 1;
        endmodule
        `ifndef GUARD
        `define GUARD
        `endif
    )"), style), dedent(R"(
        module foo;
          assign a = 1;
        endmodule
        `ifndef GUARD
        `define GUARD
        `endif
    )"));
    // clang-format on
}

TEST(Directive, Ifdef) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        `ifdef FOO
        module foo;
          assign a = 1;
        endmodule
        `endif
    )"), style), dedent(R"(
        `ifdef FOO
        module foo;
          assign a = 1;
        endmodule
        `endif
    )"));
    // clang-format on
}

TEST(Directive, Include) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        `include "file.svh"
        module foo;
          assign a = 1;
        endmodule
    )"), style), dedent(R"(
        `include "file.svh"
        module foo;
          assign a = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(Directive, MacroUsage) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        `define STRINGIFY(x) `"x`"
        module foo;
          initial $display(`STRINGIFY(bar));
        endmodule
    )"), style), dedent(R"(
        `define STRINGIFY(x) `"x`"
        module foo;
          initial $display(`STRINGIFY(bar));
        endmodule
    )"));
    // clang-format on
}

TEST(Directive, Multiple) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        `timescale 1ns / 1ps
        `default_nettype none
        module foo;
          assign a = 1;
        endmodule
    )"), style), dedent(R"(
        `timescale 1ns / 1ps
        `default_nettype none
        module foo;
          assign a = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(FormatOff, BlockComment) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        /* slang-format off */
        assign x = 1;
        /* slang-format on */
        assign y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          /* slang-format off */
        assign x = 1;
        /* slang-format on */
          assign y = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(FormatOff, LineComment) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        // slang-format off
        assign x = 1;
        // slang-format on
        assign y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off
        assign x = 1;
        // slang-format on
          assign y = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(FormatOff, VeribleBlockComment) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        /* verilog_format: off */
        assign x = 1;
        /* verilog_format: on */
        assign y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          /* verilog_format: off */
        assign x = 1;
        /* verilog_format: on */
          assign y = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(FormatOff, VeribleLineComment) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        // verilog_format: off
        assign x = 1;
        // verilog_format: on
        assign y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          // verilog_format: off
        assign x = 1;
        // verilog_format: on
          assign y = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(IndentWidth, Custom) {
    auto style = getDefaultStyle();
    style.IndentWidth = 4;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
            assign x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(IndentWidth, Default) {
    auto style = getDefaultStyle();
    style.IndentWidth = 2;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(MaxEmptyLinesToKeep, One) {
    auto style = getDefaultStyle();
    style.MaxEmptyLinesToKeep = 1;
    style.RemoveEmptyLines = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;

        assign y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;

          assign y = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(MaxEmptyLinesToKeep, Two) {
    auto style = getDefaultStyle();
    style.MaxEmptyLinesToKeep = 2;
    style.RemoveEmptyLines = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;

        assign y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;

          assign y = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(MaxEmptyLinesToKeep, Zero) {
    auto style = getDefaultStyle();
    style.MaxEmptyLinesToKeep = 0;
    style.RemoveEmptyLines = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;

        assign y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;
          assign y = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(ParameterPortListIndentWidth, Custom) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.ParameterPortListIndentWidth = 2;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo #(
          parameter N = 4
        ) (
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

TEST(ParameterPortListIndentWidth, Default) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.ParameterPortListIndentWidth = 4;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo #(
          parameter N = 4
        ) (
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

TEST(ParameterPortListIndentWidth, Instantiation) {
    auto style = getDefaultStyle();
    style.ParameterPortListIndentWidth = 3;

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
          bar #(
             .W(8)
          ) u_bar (
             .clk(clk)
          );
        endmodule
    )"));
    // clang-format on
}

TEST(ParameterPortListIndentWidth, WithComments) {
    auto style = getDefaultStyle();
    style.ParameterPortListIndentWidth = 4;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo #(
        // comment
        parameter N = 4
        ) (
        // comment
        input a
        );
        endmodule
    )"), style), dedent(R"(
        module foo #(
            // comment
            parameter N = 4
        ) (
            // comment
            input a
        );
        endmodule
    )"));
    // clang-format on
}

TEST(RemoveEmptyLines, BeforeEndToken) {
    auto style = getDefaultStyle();
    style.RemoveEmptyLines = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;

        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;

        endmodule
    )"));
    // clang-format on
}

TEST(RemoveEmptyLines, Disabled) {
    auto style = getDefaultStyle();
    style.RemoveEmptyLines = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;

        assign y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;

          assign y = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(RemoveEmptyLines, FormatOff) {
    auto style = getDefaultStyle();
    style.RemoveEmptyLines = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign a = 1;
        // slang-format off

        assign b = 2;
        // slang-format on

        assign c = 3;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign a = 1;
          // slang-format off

        assign b = 2;
        // slang-format on

          assign c = 3;
        endmodule
    )"));
    // clang-format on
}

TEST(RemoveEmptyLines, FormatOffVerible) {
    auto style = getDefaultStyle();
    style.RemoveEmptyLines = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign a = 1;
        // verilog_format: off

        assign b = 2;
        // verilog_format: on

        assign c = 3;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign a = 1;
          // verilog_format: off

        assign b = 2;
        // verilog_format: on

          assign c = 3;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterBrackets, Collapses) {
    auto style = getDefaultStyle();
    style.SpaceAfterBrackets.PackedDimensions = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0]   data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterBrackets, FormatOff) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          // slang-format off
          logic [7:0]data;
          // slang-format on
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off
          logic [7:0]data;
          // slang-format on
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterBrackets, Inserts) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0]data;
          logic data[0:255];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] data;
          logic data[0:255];
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterBrackets, PreservesNewlines) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0]
          data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0]
              data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterBrackets, Removes) {
    auto style = getDefaultStyle();
    style.SpaceAfterBrackets.PackedDimensions = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0]  data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0]data;
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic data [0:255] ;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic data[0:255];
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterBrackets, UnpackedDimensions) {
    auto style = getDefaultStyle();
    style.SpaceAfterBrackets.PackedDimensions = false;
    style.SpaceAfterBrackets.UnpackedDimensions = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0]data[0:255][0:127];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0]data[0:255] [0:127];
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterComma, Collapses) {
    auto style = getDefaultStyle();
    style.SpaceAfterComma = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign {a  ,  b  ,  c} = d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign {a, b, c} = d;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterComma, Disabled) {
    auto style = getDefaultStyle();
    style.SpaceAfterComma = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign {a,b,c} = d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign {a,b,c} = d;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterComma, FormatOff) {
    auto style = getDefaultStyle();
    style.SpaceAfterComma = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          // slang-format off
          assign {a,b,c} = d;
          // slang-format on
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off
          assign {a,b,c} = d;
          // slang-format on
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterComma, Inserts) {
    auto style = getDefaultStyle();
    style.SpaceAfterComma = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign {a,b,c} = d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign {a, b, c} = d;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterComma, PreservesNewlines) {
    auto style = getDefaultStyle();
    style.ParameterPortListIndentWidth = 2;
    style.SpaceAfterComma = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo(
          input a,
          input b
        );
        endmodule
    )"), style), dedent(R"(
        module foo (
          input a,
          input b
        );
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterComma, Removes) {
    auto style = getDefaultStyle();
    style.SpaceAfterComma = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign {a , b , c} = d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign {a,b,c} = d;
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign {a,  b,  c} = d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign {a,b,c} = d;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterSemicolon, Collapses) {
    auto style = getDefaultStyle();
    style.SpaceAfterSemicolon = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            for (int i = 0  ;  i < 4  ;  i++) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            for (int i = 0; i < 4; i++) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterSemicolon, Disabled) {
    auto style = getDefaultStyle();
    style.SpaceAfterSemicolon = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            for (int i = 0;i < 4;i++) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            for (int i = 0;i < 4;i++) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterSemicolon, FormatOff) {
    auto style = getDefaultStyle();
    style.SpaceAfterSemicolon = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            // slang-format off
            for (int i = 0;i < 4;i++) begin
            end
            // slang-format on
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            // slang-format off
            for (int i = 0;i < 4;i++) begin
            end
            // slang-format on
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterSemicolon, Inserts) {
    auto style = getDefaultStyle();
    style.SpaceAfterSemicolon = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            for (int i = 0;i < 4;i++) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            for (int i = 0; i < 4; i++) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterSemicolon, PreservesNewlines) {
    auto style = getDefaultStyle();
    style.SpaceAfterSemicolon = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign a = 1;
          assign b = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign a = 1;
          assign b = 2;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAfterSemicolon, Removes) {
    auto style = getDefaultStyle();
    style.SpaceAfterSemicolon = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            for (int i = 0 ; i < 4 ; i++) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            for (int i = 0;i < 4;i++) begin
            end
          end
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            for (int i = 0;  i < 4;  i++) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            for (int i = 0;i < 4;i++) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAroundOperators, Collapses) {
    auto style = getDefaultStyle();
    style.SpaceAroundOperators = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x  =  a  +  b  ==  c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = a + b == c;
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            x  <=  a  &&  b;
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            x <= a && b;
          end
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign y  =  a  ?  b  :  c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign y = a ? b : c;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAroundOperators, Disabled) {
    auto style = getDefaultStyle();
    style.SpaceAroundOperators = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x=a+b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x=a+b;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAroundOperators, FormatOff) {
    auto style = getDefaultStyle();
    style.SpaceAroundOperators = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          // slang-format off
          assign x=a+b;
          // slang-format on
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off
          assign x=a+b;
          // slang-format on
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAroundOperators, Inserts) {
    auto style = getDefaultStyle();
    style.SpaceAroundOperators = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x=a+b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = a + b;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAroundOperators, InsertsDeclarator) {
    auto style = getDefaultStyle();
    style.SpaceAroundOperators = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          localparam CLK_COUNT_MAX=CLKS_PER_BIT - 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          localparam CLK_COUNT_MAX = CLKS_PER_BIT - 1;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAroundOperators, PreservesNewlines) {
    auto style = getDefaultStyle();
    style.SpaceAroundOperators = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x =
            a + b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x =
              a + b;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAroundOperators, Removes) {
    auto style = getDefaultStyle();
    style.SpaceAroundOperators = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = a + b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x=a+b;
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign y = a ? b : c;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign y=a?b:c;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceAroundOperators, RemovesDeclarator) {
    auto style = getDefaultStyle();
    style.SpaceAroundOperators = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          localparam CLK_COUNT_MAX = CLKS_PER_BIT - 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          localparam CLK_COUNT_MAX=CLKS_PER_BIT-1;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeBrackets, Collapses) {
    auto style = getDefaultStyle();
    style.SpaceBeforeBrackets.PackedDimensions = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic   [7:0] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeBrackets, FormatOff) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          // slang-format off
          logic[7:0] data;
          // slang-format on
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off
          logic[7:0] data;
          // slang-format on
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeBrackets, IgnoresExpressions) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            x = data[i];
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            x = data[i];
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeBrackets, Inserts) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic[7:0] data;
          logic data[0:255];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] data;
          logic data[0:255];
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeBrackets, PreservesNewlines) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic
          [7:0] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic
              [7:0] data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeBrackets, Removes) {
    auto style = getDefaultStyle();
    style.SpaceBeforeBrackets.PackedDimensions = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic[7:0] data;
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic data [0:255];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic data[0:255];
        endmodule
    )"));
    // clang-format on
}

TEST(SpaceBeforeBrackets, UnpackedDimensions) {
    auto style = getDefaultStyle();
    style.SpaceBeforeBrackets.PackedDimensions = false;
    style.SpaceBeforeBrackets.UnpackedDimensions = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] data[0:255];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic[7:0] data [0:255];
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesBeforeTrailingComments, BlockComment) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1; /* comment */
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;  /* comment */
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesBeforeTrailingComments, Custom) {
    auto style = getDefaultStyle();
    style.SpacesBeforeTrailingComments = 4;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1; // comment
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;    // comment
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesBeforeTrailingComments, Default) {
    auto style = getDefaultStyle();
    style.SpacesBeforeTrailingComments = 2;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1; // comment
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;  // comment
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesBeforeTrailingComments, FormatOff) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        // slang-format off
        assign x = 1; // comment
        // slang-format on
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off
        assign x = 1; // comment
        // slang-format on
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesBeforeTrailingComments, IgnoresStandaloneComment) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        // standalone comment
        assign x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          // standalone comment
          assign x = 1;
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesBeforeTrailingComments, One) {
    auto style = getDefaultStyle();
    style.SpacesBeforeTrailingComments = 1;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1; // comment
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1; // comment
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBraces, Collapses) {
    auto style = getDefaultStyle();
    style.SpacesInBraces = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = {  a  ,  b  ,  c  };
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = { a, b, c };
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBraces, Disabled) {
    auto style = getDefaultStyle();
    style.SpacesInBraces = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = {a, b, c};
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = {a, b, c};
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBraces, FormatOff) {
    auto style = getDefaultStyle();
    style.SpacesInBraces = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          // slang-format off
          assign x = {a, b, c};
          // slang-format on
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off
          assign x = {a, b, c};
          // slang-format on
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBraces, Inserts) {
    auto style = getDefaultStyle();
    style.SpacesInBraces = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = {a, b, c};
          assign y = '{a, b};
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = { a, b, c };
          assign y = '{ a, b };
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBraces, PreservesNewlines) {
    auto style = getDefaultStyle();
    style.SpacesInBraces = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = {
          a,
          b
          };
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = {
              a,
              b
              };
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBraces, Removes) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = { a, b, c };
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = {a, b, c};
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = {  a,  b,  c  };
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = {a, b, c};
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBrackets, Collapses) {
    auto style = getDefaultStyle();
    style.SpacesInBrackets = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [  7:0  ] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [ 7:0 ] data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBrackets, Disabled) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBrackets, FormatOff) {
    auto style = getDefaultStyle();
    style.SpacesInBrackets = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          // slang-format off
          logic [7:0] data;
          // slang-format on
        endmodule
    )"), style), dedent(R"(
        module foo;
          // slang-format off
          logic [7:0] data;
          // slang-format on
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBrackets, IgnoresExpressions) {
    auto style = getDefaultStyle();
    style.SpacesInBrackets = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            x = data[i];
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            x = data[i];
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBrackets, Inserts) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.SpacesInBrackets = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] data;
          logic data [0:255];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [ 7:0 ] data;
          logic data[ 0:255 ];
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBrackets, PreservesNewlines) {
    auto style = getDefaultStyle();
    style.SpacesInBrackets = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [
          7:0
          ] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [
              7:0
              ] data;
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInBrackets, Removes) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [ 7:0 ] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] data;
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic data [ 0:255 ];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic data[0:255];
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInParens, Collapses) {
    auto style = getDefaultStyle();
    style.SpacesInParens = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            if (  a  ) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            if ( a ) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInParens, Disabled) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            if (a) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            if (a) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInParens, FormatOff) {
    auto style = getDefaultStyle();
    style.SpacesInParens = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            // slang-format off
            if (a) begin
            end
            // slang-format on
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            // slang-format off
            if (a) begin
            end
            // slang-format on
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInParens, Inserts) {
    auto style = getDefaultStyle();
    style.SpacesInParens = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            if (a) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            if ( a ) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInParens, PreservesNewlines) {
    auto style = getDefaultStyle();
    style.ParameterPortListIndentWidth = 2;
    style.SpacesInParens = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo(
          input a,
          input b
        );
        endmodule
    )"), style), dedent(R"(
        module foo (
          input a,
          input b
        );
        endmodule
    )"));
    // clang-format on
}

TEST(SpacesInParens, Removes) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            if ( a ) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            if (a) begin
            end
          end
        endmodule
    )"));

    EXPECT_EQ(reformat(dedent(R"(
        module foo;

          initial begin
            if (  a  ) begin
            end
          end
        endmodule
    )"), style), dedent(R"(
        module foo;

          initial begin
            if (a) begin
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(WhitespaceNormalization, IntegerVectorLiteral) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign a = 4'b0000;
        assign b = 8'hFF;
        assign c = 4'd15;
        assign d = 3'o7;
        assign e = 4'sb1010;
        assign f = 4'bxxxx;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign a = 4'b0000;
          assign b = 8'hFF;
          assign c = 4'd15;
          assign d = 3'o7;
          assign e = 4'sb1010;
          assign f = 4'bxxxx;
        endmodule
    )"));
    // clang-format on
}

TEST(WhitespaceNormalization, SystemFunctionAfterCloseParen) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assume property (@(posedge clk)$stable(rst_n) or $rose(rst_n));
        endmodule
    )"), style), dedent(R"(
        module foo;
          assume property (@(posedge clk) $stable(rst_n) or $rose(rst_n));
        endmodule
    )"));
    // clang-format on
}
