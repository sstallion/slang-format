// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "slang/format/Format.h"
#include "slang/format/Style.h"
#include "slang/format/styles/Default.h"
#include "slang/util/Dedent.h"

#include <gtest/gtest.h>

using namespace slang::format;
using slang::dedent;

TEST(AllowShortConstraintBlocksOnASingleLine, Disabled) {
    auto style = getDefaultStyle();
    style.AllowShortConstraintBlocksOnASingleLine = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        class foo;
        constraint c {kind inside {[0:7]};}
        endclass
    )"), style), dedent(R"(
        class foo;
          constraint c {
            kind inside {[0:7]};
          }
        endclass
    )"));
    // clang-format on
}

TEST(AllowShortConstraintBlocksOnASingleLine, ExpandsMultipleItems) {
    auto style = getDefaultStyle();
    style.AllowShortConstraintBlocksOnASingleLine = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        class foo;
        constraint c {len inside {[1:16]}; data.size() == len;}
        endclass
    )"), style), dedent(R"(
        class foo;
          constraint c {
            len inside {[1:16]};
            data.size() == len;
          }
        endclass
    )"));
    // clang-format on
}

TEST(AllowShortConstraintBlocksOnASingleLine, JoinsSingleItem) {
    auto style = getDefaultStyle();
    style.AllowShortConstraintBlocksOnASingleLine = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        class foo;
        constraint c {
        kind inside {[0:7]};
        }
        endclass
    )"), style), dedent(R"(
        class foo;
          constraint c {kind inside {[0:7]};}
        endclass
    )"));
    // clang-format on
}

TEST(AllowShortConstraintBlocksOnASingleLine, RandomizeWithMultipleItems) {
    auto style = getDefaultStyle();
    style.AllowShortConstraintBlocksOnASingleLine = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        initial begin
        if (!u_bar.randomize() with {
        len == 4;
        op inside {OP_A0, OP_A1};
        }) begin
        $error("failed");
        end
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            if (!u_bar.randomize() with {
                  len == 4;
                  op inside {OP_A0, OP_A1};
                }) begin
              $error("failed");
            end
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AllowShortConstraintBlocksOnASingleLine, RandomizeWithSingleItem) {
    auto style = getDefaultStyle();
    style.AllowShortConstraintBlocksOnASingleLine = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        initial begin
        void'(std::randomize(counter) with {
        counter inside {[1:10]};
        });
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            void'(std::randomize(counter) with {counter inside {[1:10]};});
          end
        endmodule
    )"));
    // clang-format on
}

TEST(AllowShortCoverpointsOnASingleLine, Disabled) {
    auto style = getDefaultStyle();
    style.AllowShortCoverpointsOnASingleLine = false;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        class foo;
        covergroup cg;
        cp_addr: coverpoint addr {option.auto_bin_max = 8;}
        endgroup
        endclass
    )"), style), dedent(R"(
        class foo;
          covergroup cg;
            cp_addr: coverpoint addr {
              option.auto_bin_max = 8;
            }
          endgroup
        endclass
    )"));
    // clang-format on
}

TEST(AllowShortCoverpointsOnASingleLine, ExpandsWhenExceedingColumnLimit) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        class foo;
        covergroup cg;
        cp_op: coverpoint op {
        bins a_ops[] = {OP_A0, OP_A1, OP_A2, OP_A3};
        bins b_ops = {OP_B2, OP_B3};
        ignore_bins ig = {OP_A0};
        }
        endgroup
        endclass
    )"), style), dedent(R"(
        class foo;
          covergroup cg;
            cp_op: coverpoint op {
              bins a_ops[] = {OP_A0, OP_A1, OP_A2, OP_A3};
              bins b_ops = {OP_B2, OP_B3};
              ignore_bins ig = {OP_A0};
            }
          endgroup
        endclass
    )"));
    // clang-format on
}

TEST(AllowShortCoverpointsOnASingleLine, JoinsCoverCross) {
    auto style = getDefaultStyle();
    style.AllowShortCoverpointsOnASingleLine = true;

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        class foo;
          covergroup cg;
            x_op_len : cross cp_op, cp_len {
                 ignore_bins skip = binsof(cp_len.lg) && binsof(cp_op.b_ops);
            }
          endgroup
        endclass
    )"), style), dedent(R"(
        class foo;
          covergroup cg;
            x_op_len : cross cp_op, cp_len {ignore_bins skip = binsof (cp_len.lg) && binsof (cp_op.b_ops);}
          endgroup
        endclass
    )"));
    // clang-format on
}

TEST(AllowShortCoverpointsOnASingleLine, JoinsWhenFits) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        class foo;
        covergroup cg;
        cp_len: coverpoint len {
        bins sm = {[1:4]};
        bins md = {[5:12]};
        bins lg = {[13:16]};
        }
        endgroup
        endclass
    )"), style), dedent(R"(
        class foo;
          covergroup cg;
            cp_len: coverpoint len {bins sm = {[1:4]}; bins md = {[5:12]}; bins lg = {[13:16]};}
          endgroup
        endclass
    )"));
    // clang-format on
}

TEST(ClassDeclaration, Members) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        class foo;
        int x;
        endclass
    )"), style), dedent(R"(
        class foo;
          int x;
        endclass
    )"));
    // clang-format on
}

TEST(CovergroupDeclaration, Members) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        class foo;
        covergroup cg;
        option.per_instance = 1;
        endgroup
        endclass
    )"), style), dedent(R"(
        class foo;
          covergroup cg;
            option.per_instance = 1;
          endgroup
        endclass
    )"));
    // clang-format on
}

TEST(DistConstraintList, ExpandsItems) {
    const auto style = getDefaultStyle();

    // clang-format off
    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        initial begin
        assert(u_bar.randomize() with {
        op dist {OP_A0 := 40, OP_A1 := 30, [OP_A2 : OP_B3] :/ 30};
        });
        end
        endmodule
    )"), style), dedent(R"(
        module foo;
          initial begin
            assert (u_bar.randomize() with {
              op dist {
                OP_A0 := 40,
                OP_A1 := 30,
                [OP_A2 : OP_B3] :/ 30
              };
            });
          end
        endmodule
    )"));
    // clang-format on
}
