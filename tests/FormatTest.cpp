// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "TestHelper.h"
#include "slang/format/Style.h"
#include "slang/format/styles/Default.h"

#include <gtest/gtest.h>

using namespace slang::format;
using namespace slang::format::style;

TEST(LineEnding, CRLF) {
    auto style = getDefaultStyle();
    style.LineEnding = LineEndingStyle::CRLF;

    EXPECT_EQ(reformat("module foo;\nassign x = 1;\nendmodule\n", style),
              "module foo;\r\n  assign x = 1;\r\nendmodule\r\n");
}

TEST(LineEnding, DeriveCRLF) {
    auto style = getDefaultStyle();
    style.LineEnding = LineEndingStyle::DeriveCRLF;

    EXPECT_EQ(reformat("module foo;\r\nassign x = 1;\r\nendmodule\r\n", style),
              "module foo;\r\n  assign x = 1;\r\nendmodule\r\n");
}

TEST(LineEnding, DeriveCRLFPrefersLF) {
    auto style = getDefaultStyle();
    style.LineEnding = LineEndingStyle::DeriveCRLF;

    EXPECT_EQ(reformat("module foo;\r\nassign x = 1;\nendmodule\n", style),
              "module foo;\n  assign x = 1;\nendmodule\n");
}

TEST(LineEnding, DeriveLF) {
    auto style = getDefaultStyle();
    style.LineEnding = LineEndingStyle::DeriveLF;

    EXPECT_EQ(reformat("module foo;\nassign x = 1;\nendmodule\n", style),
              "module foo;\n  assign x = 1;\nendmodule\n");
}

TEST(LineEnding, DeriveLFPrefersCRLF) {
    auto style = getDefaultStyle();
    style.LineEnding = LineEndingStyle::DeriveLF;

    EXPECT_EQ(reformat("module foo;\nassign x = 1;\r\nendmodule\r\n", style),
              "module foo;\r\n  assign x = 1;\r\nendmodule\r\n");
}

TEST(LineEnding, LF) {
    auto style = getDefaultStyle();
    style.LineEnding = LineEndingStyle::LF;

    EXPECT_EQ(reformat("module foo;\r\nassign x = 1;\r\nendmodule\r\n", style),
              "module foo;\n  assign x = 1;\nendmodule\n");
}

// clang-format off
TEST(OneLineFormatOffRegex, SkipsMatchingLine) {
    auto style = getDefaultStyle();
    style.OneLineFormatOffRegex = ".*STATE_.*";

    EXPECT_EQ(reformat(dedent(R"(
        module foo;
        assign x = STATE_RESET;
        assign y = 2;
        endmodule
    )"), style), dedent(R"(
        module foo;
        assign x = STATE_RESET;
          assign y = 2;
        endmodule
    )"));
}
// clang-format on
