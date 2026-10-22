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
- **Dependencies** - A bulleted list of third-party libraries with a one-line
  description of what it provides.
- **Directory Structure** - An ASCII tree listing every source file under
  `include/`, `source/`, and `tests/` with a one-line description of each file's
  role. Add or remove entries whenever source code and tests are changed.
- **Formatting Flow** - A Mermaid flowchart tracing the path from input through
  output. Update the diagram whenever a pass is added, removed, or reordered.
- **Architectural Decisions** - Prose sections, one per decision, explaining the
  approach chosen and the rationale behind it. Add a new section whenever a
  non-obvious design decision is made.

### Usage

`docs/usage.md` documents command-line usage, configuration, and ignore-file
behavior. It is modeled after [ClangFormat][1]. It must be kept in sync with the
source code as the implementation evolves. Read this document only when making
changes to the command-line interface or file-handling behavior.

The file contains the following sections, in order:

- An unnamed introductory section describing slang-format as a standalone tool,
  and Documents command-line options (as a verbatim help-text code block),
  response file syntax, stdin support, and exit codes.
- **Configuration** - Documents the `.slang-format` YAML file and
  directory-hierarchy lookup behavior with example configuration.
- **Ignoring Files** - Documents `.slang-format-ignore` pattern format and
  supported matching rules.

### Style Options

`docs/style-options.md` documents user-facing configuration. It is modeled after
[ClangFormatStyleOptions][2]. It must be kept in sync with the source code as
the implementation evolves. Read this document only when making user-facing
configuration changes.

Predefined style headers under `include/slang/format/styles/` list every option
explicitly and are part of the user-facing configuration. Whenever a user-facing
option is added, removed, renamed, or has its default value changed, update
every style header (adding the field to each style with the value appropriate
for that style) in the same change as `Style.h`, `ConfigFile.cpp`, the tests,
and this document. `Style.h` carries documentation and types only; default
values live exclusively in the style headers.

The file contains the following sections, in order:

- An unnamed introductory section covering the `.slang-format` YAML file and
  directory-hierarchy lookup behavior with example configuration.
- **Disabling Formatting** - Documents formatting pragmas.
- **Configurable Options** - Documents per-option type, default value, allowed
  values (for enums), and before and after code examples written in
  SystemVerilog using `sv` fenced code blocks.
- Shared struct fields that are unused by a particular configuration option
  must not be documented for that option.
- Struct fields are to be documented in ASCIIbetical order unless the field is
  named `Enabled` or `Disabled`, which should always be listed first.

### TODO

`docs/todo.md` contains a list of future work to be done. This file is
maintained by the user and should not be modified. Do not read this document
unless explicitly requested by the user.

### Verible Compatibility

`docs/verible.md` tracks the compatibility state between Verible's default
formatting behavior and slang-format's output. Update this document when
deviations are added, resolved, or reclassified.

The file contains the following sections, in order:

- An unnamed introductory section describing the golden reference file and
  torture test fixture.
- **Status** - A summary table of remaining deviations by root cause category,
  with hunk counts and affected line numbers.
- **Deviation** sections - One per root cause category, each containing affected
  lines, root cause analysis, why it is not yet fixed, and possible resolution
  or required refactoring.
- **Implementation Details** - Prose sections documenting non-obvious alignment
  mechanisms that are relevant to the deviations.

## Documentation Style

- Keep documentation sufficiently high-level; avoid implementation details
  unless specifically needed.
- Do not fully qualify enum names and values in documentation.
- Configuration options (including enum names and values) are sorted in
  alphabetical order.
- List items should begin with a capital letter, even if it is a fragment.

[1]: https://clang.llvm.org/docs/ClangFormat.html
[2]: https://clang.llvm.org/docs/ClangFormatStyleOptions.html
