// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "slang/format/Format.h"
#include "slang/format/Style.h"
#include "slang/format/styles/Default.h"
#include "slang/util/Dedent.h"

#include <gtest/gtest.h>

using namespace slang::format;
using slang::dedent;

// clang-format off
TEST(Comment, BeforeEnd) {
    const auto style = getDefaultStyle();

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
}

TEST(Comment, BeforeEndfunction) {
    const auto style = getDefaultStyle();

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
}

TEST(Comment, BeforeEndmodule) {
    const auto style = getDefaultStyle();

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        // comment
        endmodule
    )"), style), dedent(R"(
        module foo;
          // comment
        endmodule
    )"));
}

TEST(Comment, BlockCommentBeforeEndmodule) {
    const auto style = getDefaultStyle();

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        /* comment */
        endmodule
    )"), style), dedent(R"(
        module foo;
          /* comment */
        endmodule
    )"));
}

TEST(Comment, InBlock) {
    const auto style = getDefaultStyle();

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
}

TEST(ContinuationIndentWidth, Custom) {
    auto style = getDefaultStyle();
    style.ContinuationIndentWidth = 2;

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
}

TEST(ContinuationIndentWidth, Default) {
    auto style = getDefaultStyle();
    style.ContinuationIndentWidth = 4;

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
}

TEST(Directive, AfterModule) {
    const auto style = getDefaultStyle();

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
}

TEST(Directive, DefineGuard) {
    const auto style = getDefaultStyle();

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
}

TEST(Directive, Ifdef) {
    const auto style = getDefaultStyle();

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
}

TEST(Directive, Include) {
    const auto style = getDefaultStyle();

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
}

TEST(Directive, MacroUsage) {
    const auto style = getDefaultStyle();

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
}

TEST(Directive, Multiple) {
    const auto style = getDefaultStyle();

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
}

TEST(FormatOff, BlockComment) {
    const auto style = getDefaultStyle();

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
}

TEST(FormatOff, LineComment) {
    const auto style = getDefaultStyle();

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
}

TEST(FormatOff, VeribleBlockComment) {
    const auto style = getDefaultStyle();

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
}

TEST(FormatOff, VeribleLineComment) {
    const auto style = getDefaultStyle();

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
}

TEST(IndentWidth, Custom) {
    auto style = getDefaultStyle();
    style.IndentWidth = 4;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
            assign x = 1;
        endmodule
    )"));
}

TEST(IndentWidth, Default) {
    auto style = getDefaultStyle();
    style.IndentWidth = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;
        endmodule
    )"));
}

TEST(MaxEmptyLinesToKeep, One) {
    auto style = getDefaultStyle();
    style.MaxEmptyLinesToKeep = 1;
    style.RemoveEmptyLines = true;

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
}

TEST(MaxEmptyLinesToKeep, Two) {
    auto style = getDefaultStyle();
    style.MaxEmptyLinesToKeep = 2;
    style.RemoveEmptyLines = true;

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
}

TEST(MaxEmptyLinesToKeep, Zero) {
    auto style = getDefaultStyle();
    style.MaxEmptyLinesToKeep = 0;
    style.RemoveEmptyLines = true;

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
}

TEST(ParameterPortListIndentWidth, Custom) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.ParameterPortListIndentWidth = 2;

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
}

TEST(ParameterPortListIndentWidth, Default) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.ParameterPortListIndentWidth = 4;

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
}

TEST(ParameterPortListIndentWidth, Instantiation) {
    auto style = getDefaultStyle();
    style.ParameterPortListIndentWidth = 3;

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
}

TEST(ParameterPortListIndentWidth, WithComments) {
    auto style = getDefaultStyle();
    style.ParameterPortListIndentWidth = 4;

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
}

TEST(RemoveEmptyLines, BeforeEndToken) {
    auto style = getDefaultStyle();
    style.RemoveEmptyLines = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1;


        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;

        endmodule
    )"));
}

TEST(RemoveEmptyLines, Disabled) {
    auto style = getDefaultStyle();
    style.RemoveEmptyLines = false;

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
}

TEST(RemoveEmptyLines, FormatOff) {
    auto style = getDefaultStyle();
    style.RemoveEmptyLines = true;

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
}

TEST(RemoveEmptyLines, FormatOffVerible) {
    auto style = getDefaultStyle();
    style.RemoveEmptyLines = true;

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
}

TEST(SpaceAfterBrackets, Collapses) {
    auto style = getDefaultStyle();
    style.SpaceAfterBrackets.PackedDimensions = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0]   data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] data;
        endmodule
    )"));
}

TEST(SpaceAfterBrackets, FormatOff) {
    const auto style = getDefaultStyle();

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
}

TEST(SpaceAfterBrackets, Inserts) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

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
}

TEST(SpaceAfterBrackets, PreservesNewlines) {
    const auto style = getDefaultStyle();

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
}

TEST(SpaceAfterBrackets, Removes) {
    auto style = getDefaultStyle();
    style.SpaceAfterBrackets.PackedDimensions = false;

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
}

TEST(SpaceAfterBrackets, UnpackedDimensions) {
    auto style = getDefaultStyle();
    style.SpaceAfterBrackets.PackedDimensions = false;
    style.SpaceAfterBrackets.UnpackedDimensions = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0]data[0:255][0:127];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0]data[0:255] [0:127];
        endmodule
    )"));
}

TEST(SpaceAfterComma, Collapses) {
    auto style = getDefaultStyle();
    style.SpaceAfterComma = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign {a  ,  b  ,  c} = d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign {a, b, c} = d;
        endmodule
    )"));
}

TEST(SpaceAfterComma, Disabled) {
    auto style = getDefaultStyle();
    style.SpaceAfterComma = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign {a,b,c} = d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign {a,b,c} = d;
        endmodule
    )"));
}

TEST(SpaceAfterComma, FormatOff) {
    auto style = getDefaultStyle();
    style.SpaceAfterComma = true;

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
}

TEST(SpaceAfterComma, Inserts) {
    auto style = getDefaultStyle();
    style.SpaceAfterComma = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign {a,b,c} = d;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign {a, b, c} = d;
        endmodule
    )"));
}

TEST(SpaceAfterComma, PreservesNewlines) {
    auto style = getDefaultStyle();
    style.ParameterPortListIndentWidth = 2;
    style.SpaceAfterComma = true;

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
}

TEST(SpaceAfterComma, Removes) {
    auto style = getDefaultStyle();
    style.SpaceAfterComma = false;

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
}

TEST(SpaceAfterSemicolon, Collapses) {
    auto style = getDefaultStyle();
    style.SpaceAfterSemicolon = true;

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
}

TEST(SpaceAfterSemicolon, Disabled) {
    auto style = getDefaultStyle();
    style.SpaceAfterSemicolon = false;

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
}

TEST(SpaceAfterSemicolon, FormatOff) {
    auto style = getDefaultStyle();
    style.SpaceAfterSemicolon = true;

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
}

TEST(SpaceAfterSemicolon, Inserts) {
    auto style = getDefaultStyle();
    style.SpaceAfterSemicolon = true;

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
}

TEST(SpaceAfterSemicolon, PreservesNewlines) {
    auto style = getDefaultStyle();
    style.SpaceAfterSemicolon = true;

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
}

TEST(SpaceAfterSemicolon, Removes) {
    auto style = getDefaultStyle();
    style.SpaceAfterSemicolon = false;

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
}

TEST(SpaceAroundOperators, Collapses) {
    auto style = getDefaultStyle();
    style.SpaceAroundOperators = true;

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
}

TEST(SpaceAroundOperators, Disabled) {
    auto style = getDefaultStyle();
    style.SpaceAroundOperators = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x=a+b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x=a+b;
        endmodule
    )"));
}

TEST(SpaceAroundOperators, FormatOff) {
    auto style = getDefaultStyle();
    style.SpaceAroundOperators = true;

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
}

TEST(SpaceAroundOperators, Inserts) {
    auto style = getDefaultStyle();
    style.SpaceAroundOperators = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x=a+b;
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = a + b;
        endmodule
    )"));
}

TEST(SpaceAroundOperators, InsertsDeclarator) {
    auto style = getDefaultStyle();
    style.SpaceAroundOperators = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          localparam CLK_COUNT_MAX=CLKS_PER_BIT - 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          localparam CLK_COUNT_MAX = CLKS_PER_BIT - 1;
        endmodule
    )"));
}

TEST(SpaceAroundOperators, PreservesNewlines) {
    auto style = getDefaultStyle();
    style.SpaceAroundOperators = true;

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
}

TEST(SpaceAroundOperators, Removes) {
    auto style = getDefaultStyle();
    style.SpaceAroundOperators = false;

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
}

TEST(SpaceAroundOperators, RemovesDeclarator) {
    auto style = getDefaultStyle();
    style.SpaceAroundOperators = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          localparam CLK_COUNT_MAX = CLKS_PER_BIT - 1;
        endmodule
    )"), style), dedent(R"(
        module foo;
          localparam CLK_COUNT_MAX=CLKS_PER_BIT-1;
        endmodule
    )"));
}

TEST(SpaceBeforeBrackets, Collapses) {
    auto style = getDefaultStyle();
    style.SpaceBeforeBrackets.PackedDimensions = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic   [7:0] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] data;
        endmodule
    )"));
}

TEST(SpaceBeforeBrackets, FormatOff) {
    const auto style = getDefaultStyle();

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
}

TEST(SpaceBeforeBrackets, IgnoresExpressions) {
    const auto style = getDefaultStyle();

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
}

TEST(SpaceBeforeBrackets, Inserts) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

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
}

TEST(SpaceBeforeBrackets, PreservesNewlines) {
    const auto style = getDefaultStyle();

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
}

TEST(SpaceBeforeBrackets, Removes) {
    auto style = getDefaultStyle();
    style.SpaceBeforeBrackets.PackedDimensions = false;

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
}

TEST(SpaceBeforeBrackets, UnpackedDimensions) {
    auto style = getDefaultStyle();
    style.SpaceBeforeBrackets.PackedDimensions = false;
    style.SpaceBeforeBrackets.UnpackedDimensions = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] data[0:255];
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic[7:0] data [0:255];
        endmodule
    )"));
}

TEST(SpacesBeforeTrailingComments, BlockComment) {
    const auto style = getDefaultStyle();

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1; /* comment */
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;  /* comment */
        endmodule
    )"));
}

TEST(SpacesBeforeTrailingComments, Custom) {
    auto style = getDefaultStyle();
    style.SpacesBeforeTrailingComments = 4;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1; // comment
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;    // comment
        endmodule
    )"));
}

TEST(SpacesBeforeTrailingComments, Default) {
    auto style = getDefaultStyle();
    style.SpacesBeforeTrailingComments = 2;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1; // comment
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1;  // comment
        endmodule
    )"));
}

TEST(SpacesBeforeTrailingComments, FormatOff) {
    const auto style = getDefaultStyle();

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
}

TEST(SpacesBeforeTrailingComments, IgnoresStandaloneComment) {
    const auto style = getDefaultStyle();

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
}

TEST(SpacesBeforeTrailingComments, One) {
    auto style = getDefaultStyle();
    style.SpacesBeforeTrailingComments = 1;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = 1; // comment
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = 1; // comment
        endmodule
    )"));
}

TEST(SpacesInBraces, Collapses) {
    auto style = getDefaultStyle();
    style.SpacesInBraces = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = {  a  ,  b  ,  c  };
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = { a, b, c };
        endmodule
    )"));
}

TEST(SpacesInBraces, Disabled) {
    auto style = getDefaultStyle();
    style.SpacesInBraces = false;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          assign x = {a, b, c};
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign x = {a, b, c};
        endmodule
    )"));
}

TEST(SpacesInBraces, FormatOff) {
    auto style = getDefaultStyle();
    style.SpacesInBraces = true;

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
}

TEST(SpacesInBraces, Inserts) {
    auto style = getDefaultStyle();
    style.SpacesInBraces = true;

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
}

TEST(SpacesInBraces, PreservesNewlines) {
    auto style = getDefaultStyle();
    style.SpacesInBraces = true;

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
}

TEST(SpacesInBraces, Removes) {
    const auto style = getDefaultStyle();

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
}

TEST(SpacesInBrackets, Collapses) {
    auto style = getDefaultStyle();
    style.SpacesInBrackets = true;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [  7:0  ] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [ 7:0 ] data;
        endmodule
    )"));
}

TEST(SpacesInBrackets, Disabled) {
    const auto style = getDefaultStyle();

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          logic [7:0] data;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [7:0] data;
        endmodule
    )"));
}

TEST(SpacesInBrackets, FormatOff) {
    auto style = getDefaultStyle();
    style.SpacesInBrackets = true;

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
}

TEST(SpacesInBrackets, IgnoresExpressions) {
    auto style = getDefaultStyle();
    style.SpacesInBrackets = true;

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
}

TEST(SpacesInBrackets, Inserts) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};
    style.SpacesInBrackets = true;

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
}

TEST(SpacesInBrackets, PreservesNewlines) {
    auto style = getDefaultStyle();
    style.SpacesInBrackets = true;

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
}

TEST(SpacesInBrackets, Removes) {
    const auto style = getDefaultStyle();

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
}

TEST(SpacesInParens, Collapses) {
    auto style = getDefaultStyle();
    style.SpacesInParens = true;

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
}

TEST(SpacesInParens, Disabled) {
    const auto style = getDefaultStyle();

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
}

TEST(SpacesInParens, FormatOff) {
    auto style = getDefaultStyle();
    style.SpacesInParens = true;

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
}

TEST(SpacesInParens, Inserts) {
    auto style = getDefaultStyle();
    style.SpacesInParens = true;

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
}

TEST(SpacesInParens, PreservesNewlines) {
    auto style = getDefaultStyle();
    style.ParameterPortListIndentWidth = 2;
    style.SpacesInParens = true;

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
}

TEST(SpacesInParens, Removes) {
    const auto style = getDefaultStyle();

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
}

TEST(WhitespaceNormalization, IntegerVectorLiteral) {
    const auto style = getDefaultStyle();

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
}

TEST(WhitespaceNormalization, SystemFunctionAfterCloseParen) {
    const auto style = getDefaultStyle();

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assume property (@(posedge clk)$stable(rst_n) or $rose(rst_n));
        endmodule
    )"), style), dedent(R"(
        module foo;
          assume property (@(posedge clk) $stable(rst_n) or $rose(rst_n));
        endmodule
    )"));
}
// clang-format on
