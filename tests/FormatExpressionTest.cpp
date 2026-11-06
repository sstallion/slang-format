// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "slang/format/Format.h"
#include "slang/format/Style.h"
#include "slang/format/styles/Default.h"
#include "slang/util/Dedent.h"

#include <gtest/gtest.h>

using namespace slang::format;
using slang::dedent;

TEST(AlignAfterOpenParen, AlignBinPacked) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = ParenAlignmentStyle::Align;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
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
    // clang-format on
}

TEST(AlignAfterOpenParen, AlignEmptyArguments) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = ParenAlignmentStyle::Align;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
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
    // clang-format on
}

TEST(AlignAfterOpenParen, AlignExistingBreaks) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = ParenAlignmentStyle::Align;

    // clang-format off
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
    // clang-format on
}

TEST(AlignAfterOpenParen, AlignFits) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = ParenAlignmentStyle::Align;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
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
    // clang-format on
}

TEST(AlignAfterOpenParen, AlignLastArgumentOverflows) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = ParenAlignmentStyle::Align;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
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
    // clang-format on
}

TEST(AlignAfterOpenParen, AlignMethodCall) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = ParenAlignmentStyle::Align;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
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
    // clang-format on
}

TEST(AlignAfterOpenParen, AlignNestedCalls) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = ParenAlignmentStyle::Align;
    style.ColumnLimit = 50; // NOLINT

    // clang-format off
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
    // clang-format on
}

TEST(AlignAfterOpenParen, AlignSingleArgument) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = ParenAlignmentStyle::Align;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
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
    // clang-format on
}

TEST(AlignAfterOpenParen, AlignSystemTask) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = ParenAlignmentStyle::Align;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
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
    // clang-format on
}

TEST(AlignAfterOpenParen, AlwaysBreakBinPacked) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = ParenAlignmentStyle::AlwaysBreak;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
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
    // clang-format on
}

TEST(AlignAfterOpenParen, AlwaysBreakFits) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = ParenAlignmentStyle::AlwaysBreak;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
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
    // clang-format on
}

TEST(AlignAfterOpenParen, BlockIndentBinPacked) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = ParenAlignmentStyle::BlockIndent;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
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
    // clang-format on
}

TEST(AlignAfterOpenParen, BlockIndentFits) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = ParenAlignmentStyle::BlockIndent;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
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
    // clang-format on
}

TEST(AlignAfterOpenParen, DontAlign) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = ParenAlignmentStyle::DontAlign;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
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
    // clang-format on
}

TEST(AlignAfterOpenParen, ElabSystemTask) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = ParenAlignmentStyle::Align;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
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
    // clang-format on
}

TEST(AlignAfterOpenParen, NewClassExpression) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = ParenAlignmentStyle::Align;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
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
    // clang-format on
}

TEST(AlignAfterOpenParen, NoColumnLimit) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = ParenAlignmentStyle::Align;
    style.ColumnLimit = 0;

    // clang-format off
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
    // clang-format on
}

TEST(AttributeInstance, NormalizesSpacing) {
    auto style = getDefaultStyle();
    style.AlignConsecutiveDeclarations = {};
    style.AlignConsecutivePackedDimensions = {};

    // clang-format off
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
    // clang-format on
}

TEST(BinPackArguments, Disabled) {
    auto style = getDefaultStyle();
    style.BinPackArguments = false;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
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
    // clang-format on
}

TEST(BinPackArguments, DisabledAlwaysBreak) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = ParenAlignmentStyle::AlwaysBreak;
    style.BinPackArguments = false;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
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
    // clang-format on
}

TEST(BinPackArguments, DisabledBlockIndent) {
    auto style = getDefaultStyle();
    style.AlignAfterOpenParen = ParenAlignmentStyle::BlockIndent;
    style.BinPackArguments = false;
    style.ColumnLimit = 40; // NOLINT

    // clang-format off
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
    // clang-format on
}

TEST(DelayedSequenceElement, BeforeParenthesizedExpression) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(DelayedSequenceElement, BracketRange) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}

TEST(RangeSpacing, AlwaysBitSelect) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::Always;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign y = x[7:0];
        endmodule
    )"), style), dedent(R"(
        module foo;
          assign y = x[7 : 0];
        endmodule
    )"));
    // clang-format on
}

TEST(RangeSpacing, AlwaysInserts) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::Always;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [3:0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [3 : 0] x;
        endmodule
    )"));
    // clang-format on
}

TEST(RangeSpacing, AlwaysKeepsSpaces) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::Always;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [3 : 0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [3 : 0] x;
        endmodule
    )"));
    // clang-format on
}

TEST(RangeSpacing, MatchSourceKeepsCompact) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::MatchSource;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [3:0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [3:0] x;
        endmodule
    )"));
    // clang-format on
}

TEST(RangeSpacing, MatchSourceKeepsSpaces) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::MatchSource;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [3 : 0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [3 : 0] x;
        endmodule
    )"));
    // clang-format on
}

TEST(RangeSpacing, MatchSourceValueRange) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::MatchSource;

    // clang-format off
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
    // clang-format on
}

TEST(RangeSpacing, NeverKeepsCompact) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::Never;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [3:0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [3:0] x;
        endmodule
    )"));
    // clang-format on
}

TEST(RangeSpacing, NeverRemoves) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::Never;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [3 : 0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [3:0] x;
        endmodule
    )"));
    // clang-format on
}

TEST(RangeSpacing, PreserveCollapsesMultiSpace) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::Preserve;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [3   :   0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [3 : 0] x;
        endmodule
    )"));
    // clang-format on
}

TEST(RangeSpacing, PreserveKeepsCompact) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::Preserve;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [3:0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [3:0] x;
        endmodule
    )"));
    // clang-format on
}

TEST(RangeSpacing, PreserveKeepsSpaces) {
    auto style = getDefaultStyle();
    style.RangeSpacing = RangeSpacingStyle::Preserve;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        logic [3 : 0] x;
        endmodule
    )"), style), dedent(R"(
        module foo;
          logic [3 : 0] x;
        endmodule
    )"));
    // clang-format on
}

TEST(SequenceRepetition, Consecutive) {
    const auto style = getDefaultStyle();

    // clang-format off
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
    // clang-format on
}
