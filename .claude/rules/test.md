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

[1]: https://google.github.io/googletest/
