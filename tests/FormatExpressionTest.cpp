// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "TestHelper.h"
#include "slang/format/Style.h"
#include "slang/format/styles/Default.h"

#include <gtest/gtest.h>

using namespace slang::format;
using namespace slang::format::style;

// clang-format off
TEST(AlignAfterOpenParen, AlignBinPacked) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = BracketAlignmentStyle::Align;
    style.ColumnLimit = 40; // NOLINT

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie, delta, echo_val);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie,
                     delta, echo_val);
          end
        endmodule
    )"));
}

TEST(AlignAfterOpenParen, AlignEmptyArguments) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = BracketAlignmentStyle::Align;
    style.ColumnLimit = 40; // NOLINT

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            bar();
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            bar();
          end
        endmodule
    )"));
}

TEST(AlignAfterOpenParen, AlignExistingBreaks) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = BracketAlignmentStyle::Align;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            obj.method(alpha,
              bravo, charlie);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            obj.method(alpha,
                       bravo, charlie);
          end
        endmodule
    )"));
}

TEST(AlignAfterOpenParen, AlignFits) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = BracketAlignmentStyle::Align;
    style.ColumnLimit = 40; // NOLINT

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            bar(a, b, c);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            bar(a, b, c);
          end
        endmodule
    )"));
}

TEST(AlignAfterOpenParen, AlignLastArgumentOverflows) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = BracketAlignmentStyle::Align;
    style.ColumnLimit = 40; // NOLINT

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display(alpha_beta, gamma_delta_val);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display(alpha_beta,
                     gamma_delta_val);
          end
        endmodule
    )"));
}

TEST(AlignAfterOpenParen, AlignMethodCall) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = BracketAlignmentStyle::Align;
    style.ColumnLimit = 40; // NOLINT

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            obj.method(alpha, bravo, charlie, delta);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            obj.method(alpha, bravo, charlie,
                       delta);
          end
        endmodule
    )"));
}

TEST(AlignAfterOpenParen, AlignNestedCalls) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = BracketAlignmentStyle::Align;
    style.ColumnLimit = 50; // NOLINT

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display("result: %d", calculate(alpha, bravo, charlie));
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display("result: %d",
                     calculate(alpha, bravo, charlie));
          end
        endmodule
    )"));
}

TEST(AlignAfterOpenParen, AlignSingleArgument) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = BracketAlignmentStyle::Align;
    style.ColumnLimit = 40; // NOLINT

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            bar(x);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            bar(x);
          end
        endmodule
    )"));
}

TEST(AlignAfterOpenParen, AlignSystemTask) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = BracketAlignmentStyle::Align;
    style.ColumnLimit = 40; // NOLINT

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display("format: %d %d", alpha, bravo, charlie);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display("format: %d %d", alpha,
                     bravo, charlie);
          end
        endmodule
    )"));
}

TEST(AlignAfterOpenParen, AlwaysBreakBinPacked) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = BracketAlignmentStyle::AlwaysBreak;
    style.ColumnLimit = 40; // NOLINT

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie, delta, echo_val);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display(
                "alpha", bravo, charlie, delta,
                echo_val);
          end
        endmodule
    )"));
}

TEST(AlignAfterOpenParen, AlwaysBreakFits) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = BracketAlignmentStyle::AlwaysBreak;
    style.ColumnLimit = 40; // NOLINT

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            bar(a, b, c);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            bar(a, b, c);
          end
        endmodule
    )"));
}

TEST(AlignAfterOpenParen, BlockIndentBinPacked) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = BracketAlignmentStyle::BlockIndent;
    style.ColumnLimit = 40; // NOLINT

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie, delta, echo_val);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display(
                "alpha", bravo, charlie, delta,
                echo_val
            );
          end
        endmodule
    )"));
}

TEST(AlignAfterOpenParen, BlockIndentFits) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = BracketAlignmentStyle::BlockIndent;
    style.ColumnLimit = 40; // NOLINT

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            bar(a, b, c);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            bar(a, b, c);
          end
        endmodule
    )"));
}

TEST(AlignAfterOpenParen, DontAlign) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = BracketAlignmentStyle::DontAlign;
    style.ColumnLimit = 40; // NOLINT

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie, delta, echo_val);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie, delta, echo_val);
          end
        endmodule
    )"));
}

TEST(AlignAfterOpenParen, ElabSystemTask) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = BracketAlignmentStyle::Align;
    style.ColumnLimit = 40; // NOLINT

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          $fatal(1, "some very long error message here");
        endmodule
    )"), style), dedent(R"(
        module foo;
          $fatal(1,
                 "some very long error message here");
        endmodule
    )"));
}

TEST(AlignAfterOpenParen, NewClassExpression) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = BracketAlignmentStyle::Align;
    style.ColumnLimit = 40; // NOLINT

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            obj = new(alpha, bravo, charlie, delta);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            obj = new(alpha, bravo, charlie,
                      delta);
          end
        endmodule
    )"));
}

TEST(AlignAfterOpenParen, NoColumnLimit) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = BracketAlignmentStyle::Align;
    style.ColumnLimit = 0;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie, delta, echo_val, foxtrot, golf, hotel, india);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie, delta, echo_val, foxtrot, golf, hotel, india);
          end
        endmodule
    )"));
}

TEST(AttributeInstance, NormalizesSpacing) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          (*keep = "true", dont_touch*)
          logic [7:0] attr_sig;
        endmodule
    )"), style), dedent(R"(
        module foo;
          (* keep = "true", dont_touch *)
          logic [7:0] attr_sig;
        endmodule
    )"));
}

TEST(BinPackArguments, Disabled) {
    auto style = getDefaultStyle();
    style.BinPackArguments = false;
    style.ColumnLimit = 40; // NOLINT

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie, delta);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display("alpha",
                     bravo,
                     charlie,
                     delta);
          end
        endmodule
    )"));
}

TEST(BinPackArguments, DisabledAlwaysBreak) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = BracketAlignmentStyle::AlwaysBreak;
    style.BinPackArguments = false;
    style.ColumnLimit = 40; // NOLINT

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie, delta);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display(
                "alpha",
                bravo,
                charlie,
                delta);
          end
        endmodule
    )"));
}

TEST(BinPackArguments, DisabledBlockIndent) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = BracketAlignmentStyle::BlockIndent;
    style.BinPackArguments = false;
    style.ColumnLimit = 40; // NOLINT

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            $display("alpha", bravo, charlie, delta);
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            $display(
                "alpha",
                bravo,
                charlie,
                delta
            );
          end
        endmodule
    )"));
}

TEST(DelayedSequenceElement, BeforeParenthesizedExpression) {
    const auto style = getDefaultStyle();

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        property p;
        req ##1(!req);
        endproperty
        endmodule
    )"), style), dedent(R"(
        module foo;
          property p;
            req ##1 (!req);
          endproperty
        endmodule
    )"));
}

TEST(DelayedSequenceElement, BracketRange) {
    const auto style = getDefaultStyle();

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        property p;
        @(posedge clk) disable iff (!rst_n) req |-> ##[1:5] gnt;
        endproperty
        endmodule
    )"), style), dedent(R"(
        module foo;
          property p;
            @(posedge clk) disable iff (!rst_n) req |-> ##[1:5] gnt;
          endproperty
        endmodule
    )"));
}

TEST(RangeSpacing, AlwaysBitSelect) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::Always;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign y = x[7:0];
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign y = x[7 : 0];
        endmodule
    )"));
}

TEST(RangeSpacing, AlwaysInserts) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::Always;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [3:0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [3 : 0] x;
        endmodule
    )"));
}

TEST(RangeSpacing, AlwaysKeepsSpaces) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::Always;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [3 : 0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [3 : 0] x;
        endmodule
    )"));
}

TEST(RangeSpacing, MatchSourceKeepsCompact) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::MatchSource;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [3:0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [3:0] x;
        endmodule
    )"));
}

TEST(RangeSpacing, MatchSourceKeepsSpaces) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::MatchSource;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [3 : 0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [3 : 0] x;
        endmodule
    )"));
}

TEST(RangeSpacing, MatchSourceValueRange) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::MatchSource;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
          initial begin
            x = y inside {[1:16]};
          end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            x = y inside {[1:16]};
          end
        endmodule
    )"));
}

TEST(RangeSpacing, NeverKeepsCompact) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::Never;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [3:0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [3:0] x;
        endmodule
    )"));
}

TEST(RangeSpacing, NeverRemoves) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::Never;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [3 : 0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [3:0] x;
        endmodule
    )"));
}

TEST(RangeSpacing, PreserveCollapsesMultiSpace) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::Preserve;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [3   :   0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [3 : 0] x;
        endmodule
    )"));
}

TEST(RangeSpacing, PreserveKeepsCompact) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::Preserve;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [3:0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [3:0] x;
        endmodule
    )"));
}

TEST(RangeSpacing, PreserveKeepsSpaces) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::Preserve;

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [3 : 0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [3 : 0] x;
        endmodule
    )"));
}

TEST(SequenceRepetition, Consecutive) {
    const auto style = getDefaultStyle();

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        sequence s;
        req_int ##1(!req_int)[*0:$] ##1 gnt_int[->n];
        endsequence
        endmodule
    )"), style), dedent(R"(
        module foo;
          sequence s;
            req_int ##1 (!req_int) [* 0: $] ##1 gnt_int [-> n];
          endsequence
        endmodule
    )"));
}
// clang-format on
