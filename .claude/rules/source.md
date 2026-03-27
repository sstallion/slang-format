---
paths:
  - 'source/**/*.{cpp,h}'
  - 'test/**/*.{cpp,h}'
---

# Source Code Rules

## Organization

- Naming conventions for files, interfaces, and user-defined types follows the
  same conventions as the upstream [slang][1] project.
- Public interfaces are declared inside the `slang::format` namespace.
- Each translation unit has a corresponding header that declares
  its public interface (e.g. `Unit.h` for `Unit.cpp`).
- Use unnamed namespaces for file-scoped declarations.
- Unnamed namespaces must not be nested inside other namespaces.
- `using namespace` directives are restricted to `std::literals` and slang
  namespaces (e.g. `slang::format`, `slang::syntax`).
- Tests are considered source code.
- Tests for each translation unit are located in dedicated files under `test/`
  (e.g. `test/UnitTest.cpp` for `source/Unit.cpp`).

## Language Features

- Use idiomatic C++20; anachronistic programming patterns should be avoided
  (e.g. C++03 and earlier).
- Use `auto` when possible.
- Use `const` when possible.
- Use `constexpr` when possible.
- Use `std::literals` symbols when possible.
- Use designated initializers when possible.
- Use uniform initialization when possible.
- Use post-increment and post-decrement operators when possible.
- Containers infer the size of contents from initialization when possible.

## Source Code Style

- Code style follows the same conventions as the upstream [slang][1] project.
- Lists are sorted in ASCIIbetical order (e.g. digits first, then uppercase
  letters, followed by lowercase letters). Use this ordering by default.
- Use newlines to group related statements logically.
- A blank line must follow the end of every control statement and every
  scope-creating construct unless it is at the end of a block.
- Statements surrounded by pragma comments (e.g. `// clang-format off` followed
  by `// clang-format on`) are wrapped by blank lines unless it is already at a
  natural boundary such as the start or end of a block.

### Includes

- Follow include-what-you-use (IWYU); each file must include headers for all
  types and symbols used.
- Relying on transitive includes is not permitted.
- Unused includes must be removed.
- Do not use forward declarations to avoid including headers.
- Local headers use `#pragma once` instead of traditional include guards.
- Local headers are included using quotes (e.g. `#include "Unit.h"`).
- System and third-party dependences are included using angle brackets (e.g.
  `#include <algorithm>`, `#include <slang/util/CommandLine.h>`).

### Comments

- Reserve comments for non-trivial functionality and documenting interfaces and
  user-defined types.
- Comments should be written on a single line, even if it exceeds the column
  limit. These will be reflowed by the `/clang-format` skill later.
- Use trailing comments for trivial documentation.
- Do not add comments to identify positional arguments.
- Use Doxygen-style comments (i.e. `///`) to document interfaces and
  user-defined types.
- Doxygen-style comments should be written using third-person singular present
  tense with proper sentence structure.
- Use Doxygen-style member comments to document class and struct members (i.e.
  `///<`).
- A blank line must precede standalone comment lines unless it is already at a
  natural boundary such as the start of a block. This rule does not apply to
  Doxygen-style comments.
- Do not add comments to identify sections of the source code; comments that
  identify classes or structs should be formatted using Doxygen-style comments.

[1]: https://sv-lang.com/commoncomp.html
