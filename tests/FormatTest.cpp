// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "Style.h"
#include "TestHelper.h"

#include <gtest/gtest.h>

using namespace slang::format;

TEST(LineEnding, CRLF) {
    Style style;
    style.LineEnding = LineEndingStyle::CRLF;

    EXPECT_EQ(reformat("module foo;\nassign x = 1;\nendmodule\n", style),
              "module foo;\r\n  assign x = 1;\r\nendmodule\r\n");
}

TEST(LineEnding, DeriveCRLF) {
    Style style;
    style.LineEnding = LineEndingStyle::DeriveCRLF;

    EXPECT_EQ(reformat("module foo;\r\nassign x = 1;\r\nendmodule\r\n", style),
              "module foo;\r\n  assign x = 1;\r\nendmodule\r\n");
}

TEST(LineEnding, DeriveCRLFPrefersLF) {
    Style style;
    style.LineEnding = LineEndingStyle::DeriveCRLF;

    EXPECT_EQ(reformat("module foo;\r\nassign x = 1;\nendmodule\n", style),
              "module foo;\n  assign x = 1;\nendmodule\n");
}

TEST(LineEnding, DeriveLF) {
    Style style;
    style.LineEnding = LineEndingStyle::DeriveLF;

    EXPECT_EQ(reformat("module foo;\nassign x = 1;\nendmodule\n", style),
              "module foo;\n  assign x = 1;\nendmodule\n");
}

TEST(LineEnding, DeriveLFPrefersCRLF) {
    Style style;
    style.LineEnding = LineEndingStyle::DeriveLF;

    EXPECT_EQ(reformat("module foo;\nassign x = 1;\r\nendmodule\r\n", style),
              "module foo;\r\n  assign x = 1;\r\nendmodule\r\n");
}

TEST(LineEnding, LF) {
    Style style;
    style.LineEnding = LineEndingStyle::LF;

    EXPECT_EQ(reformat("module foo;\r\nassign x = 1;\r\nendmodule\r\n", style),
              "module foo;\n  assign x = 1;\nendmodule\n");
}

// clang-format off
TEST(OneLineFormatOffRegex, SkipsMatchingLine) {
    Style style;
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
