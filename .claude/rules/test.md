---
paths:
  - 'test/**/*.{cpp,h}'
---

# Testing Rules

- Use the [GoogleTest][1] testing framework.
- Avoid mocks; this leads to unnecessary churn.
- Tests are intentionally simple and do not require comments.
- Use test fixtures instead of local wrappers for excessive repetition.
- Do not modify existing tests unless explicitly authorized by the user.
- Keep tests organized alphabetically by test suite, then by test name.
- Multi-line strings use the `dedent()` function from `test/TestHelper.h`
  together with raw string literals. Do not use escaped `\n` sequences in string
  literals. Statements are wrapped in `// clang-format off` / `// clang-format on`
  comment pragmas. Unindented newlines should be avoided for readability.

  ```cpp
  // CORRECT
  // clang-format off
  EXPECT_EQ(format(dedent(R"(
      module foo;
      assign x = 1;
      endmodule
  )"), style), dedent(R"(
      module foo;
        assign x = 1;
      endmodule
  )"));
  // clang-format on

  // WRONG
  EXPECT_EQ(format("module foo;\nassign x = 1;\nendmodule\n"),
            "module foo;\n  assign x = 1;\nendmodule\n");
  ```

[1]: https://google.github.io/googletest/
