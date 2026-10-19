// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "Style.h"
#include "TestHelper.h"

#include <gtest/gtest.h>

using namespace slang::format;

// clang-format off
TEST(AlignAfterOpenParen, AlignBinPacked) {
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    const Style style;

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
    const Style style;

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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    Style style;
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
    const Style style;

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
