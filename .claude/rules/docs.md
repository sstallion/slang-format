---
paths:
  - 'docs/**/*.md'
---

# Documentation Rules

## Documents

### Architecture

`docs/architecture.md` documents the internal design. It must be kept in sync
with the source code as the implementation evolves. Read this document at the
beginning of every session.

The file contains the following sections, in order:

- An unnamed introductory section describing the project and its goals.
- **Directory Structure** - An ASCII tree listing every source file under
  `source/` and `test/` with a one-line description of each file's role. Add or
  remove entries whenever source code and tests are added or removed.
- **Dependencies** - A bulleted list of third-party libraries with a one-line
  description of what it provides.
- **Formatting Flow** - An ASCII block diagram tracing the path from input
  through output Update the diagram whenever a pass is added, removed, or
  reordered.
- **Architectural Decisions** - Prose sections, one per decision, explaining the
  approach chosen and the rationale behind it. Add a new section whenever a
  non-obvious design decision is made.

### Style Options

`docs/style-options.md` documents user-facing configuration. It is modeled after
[clang-format Style Options][1]. It must be kept in sync with the source code as
the implementation evolves. Read this document only when making user-facing
configuration changes.

The file contains the following sections, in order:

- An unnamed introductory section covering the `.slang-format` YAML file and
  directory-hierarchy lookup behavior with example configuration.
- **Disabling Formatting** - Documents formatting pragmas.
- **Configurable Options** - Documents per-option type, default value, allowed
  values (for enums), and before and after code examples written in
  SystemVerilog using `sv` fenced code blocks.

### TODO

`docs/todo.md` contains a list of future work to be done. This file is
maintained by the user and should not be modified. Do not read this document
unless explicitly requested by the user.

## Documentation Style

- Keep documentation sufficiently high-level; avoid implementation details
  unless specifically needed.
- Do not fully qualify enum names and values in documentation.
- Configuration options (including enum names and values) are sorted in
  alphabetical order.
- List items should begin with a capital letter, even if it is a fragment.

[1]: https://clang.llvm.org/docs/ClangFormatStyleOptions.html
