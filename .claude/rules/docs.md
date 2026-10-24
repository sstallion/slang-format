---
paths:
  - 'docs/**/*.md'
---

# Documentation Rules

Documents under `docs/` are written for human readers first. Do not shorten,
remove, or relocate documentation because it would be cheaper or more convenient
for Claude to load. Control what Claude reads with path-scoped rules under
`.claude/rules/`, not by trimming the documents.

## Documents

| File                    | Describes                                 | Update When                                           |
| ----------------------- | ----------------------------------------- | ----------------------------------------------------- |
| `docs/architecture.md`  | Internal design                           | Source files, passes, or design decisions change      |
| `docs/usage.md`         | Command line, configuration, ignore files | The command-line interface or file handling changes   |
| `docs/style-options.md` | User-facing style options                 | An option is added, removed, renamed, or re-defaulted |
| `docs/verible.md`       | Deviations from Verible's default output  | A deviation is added, resolved, or reclassified       |
| `docs/todo.md`          | Future work, maintained by the user       | Never; do not read or modify unless asked             |

`docs/usage.md` and `docs/style-options.md` are modeled after [ClangFormat][1]
and [ClangFormatStyleOptions][2].

### Architecture

Sections, in order:

- An unnamed introduction describing the project and its goals.
- **Dependencies** - Third-party libraries with a one-line description each.
- **Directory Structure** - An ASCII tree of the directories under `include/`,
  `source/`, `tests/`, and `tools/` with a one-line description of each
  directory's role.
- **Formatting Flow** - A Mermaid flowchart tracing the path from input to
  output.
- **Architectural Decisions** - Grouped by layer (Library and Tool,
  Configuration, Rewriting, Formatting, Post-Processing, Testing), with one
  subsection per decision explaining the approach and its rationale.

### Usage

Sections, in order:

- An unnamed introduction describing slang-format as a standalone tool, with
  command-line options as a verbatim help-text code block, response file
  syntax, stdin support, and exit codes.
- **Configuration** - The `.slang-format` YAML file and directory-hierarchy
  lookup, with an example.
- **Ignoring Files** - The `.slang-format-ignore` pattern format and matching
  rules.

### Style Options

Sections, in order:

- An unnamed introduction covering the `.slang-format` YAML file and
  directory-hierarchy lookup, with an example.
- **Disabling Formatting** - Formatting pragmas.
- **Configurable Options** - Per option: type, default value, allowed values for
  enums, and before and after examples in `sv` fenced code blocks.
  - Struct fields that an option does not use are not documented for that option.
  - Struct fields are documented in ASCIIbetical order, except `Enabled` or
    `Disabled`, which are listed first.

### Verible Compatibility

Sections, in order:

- An unnamed introduction describing the golden reference file and torture
  test fixture.
- **Status** - A table of remaining deviations by root cause, with hunk counts
  and affected line numbers.
- One **Deviation** section per root cause: affected lines, analysis, why it is
  not yet fixed, and possible resolution.
- **Implementation Details** - Alignment mechanisms relevant to the
  deviations.

## Documentation Style

- Oxford commas are not optional in this house.
- Keep documentation high-level; include implementation details only when
  needed.
- Do not fully qualify enum names and values.
- Configuration options, including enum names and values, are sorted in
  alphabetical order.
- List items begin with a capital letter, even when they are fragments.

[1]: https://clang.llvm.org/docs/ClangFormat.html
[2]: https://clang.llvm.org/docs/ClangFormatStyleOptions.html
