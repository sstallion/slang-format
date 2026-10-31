---
paths:
  - '**/*.{cpp,h}'
  - 'docs/*.{dox,md}'
---

# Documentation Rules

Documents under `docs/` are processed by Doxygen and are written for human
readers first. Do not shorten, remove, or relocate documentation because it
would be cheaper or more convenient for Claude to load. Control what Claude
reads with path-scoped rules under `.claude/rules/`, not by trimming the
documents.

## Doxygen Conventions

- Use Javadoc-style commands (e.g. `@ref`, not `\ref`).
- Every `docs/*.md` file (except `index.md` and `todo.md`) has a top-level label
  `{#labelid}`. The header line is padded with spaces so it is exactly 80
  characters wide. The label is the stem of the file.
- Pages with subsections include `@tableofcontents` on line 3 (after a blank
  line following the heading). The label matches the stem of the file name.
- References use `@ref` inside Markdown links, e.g. `[Link Text](@ref labelid)`.

## Documents

| File                       | Describes                                  | Update When                                            |
| -------------------------- | ------------------------------------------ | ------------------------------------------------------ |
| `docs/api-reference.md`    | API documentation landing page             | Public API pages are added or reorganized              |
| `docs/architecture.md`     | Internal design                            | Directory structure, or design decisions change        |
| `docs/building.md`         | Building from source                       | Build prerequisites, steps, or CMake options change    |
| `docs/compatibility.md`    | Compatibility with other tools             | Compatibility options are added, modified, or removed  |
| `docs/developer-guide.md`  | Developer documentation landing page       | Developer pages are added or reorganized               |
| `docs/index.md`            | Main page                                  | The project description or introduction changes        |
| `docs/style-options.md`    | User-facing style options                  | Style options are added, modified, removed, or renamed |
| `docs/todo.md`             | Future work, maintained by the user        | Never; do not read or modify unless asked              |
| `docs/usage.md`            | CLI usage, configuration, and ignore files | The command-line interface or file handling changes    |
| `docs/user-manual.md`      | User documentation landing page            | User-facing pages are added or reorganized             |

`docs/usage.md` and `docs/style-options.md` are modeled after [ClangFormat][1]
and [ClangFormatStyleOptions][2].

### Main Page

TODO.

### User Manual

TODO.

#### Usage

Sections, in order:

- An unnamed introduction describing slang-format as a standalone tool, with
  command-line options as a verbatim help-text code block, response file
  syntax, stdin support, and exit codes.
- **Configuration** - The `.slang-format` YAML file and directory-hierarchy
  lookup, with an example.
- **Ignoring Files** - The `.slang-format-ignore` pattern format and matching
  rules.

#### Style Options

Sections, in order:

- An unnamed introduction covering the `.slang-format` YAML file and
  directory-hierarchy lookup, with an example.
- **Disabling Formatting** - Formatting pragmas.
- **Configurable Options** - Per option: heading (name only, no type suffix),
  `Type:` line with `@ref` link, description, allowed values for enums,
  and before and after examples in `sv` fenced code blocks. Default values are
  not documented. Options are not separated by horizontal rules.
  - Struct fields that an option does not use are not documented for that option.
  - Struct fields are documented in ASCIIbetical order, except `Enabled` or
    `Disabled`, which are listed first.
  - Sub-options referenced from other parent options carry an explicit
    `{#parent-suboption}` anchor ID on the heading and are referenced using
    `@ref` links.

#### Tool Compatibility

TODO.

### Developer Guide

TODO.

#### Building from Source

TODO.

#### Architecture

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

### API Reference

TODO.

## Documentation Style

- Oxford commas are not optional in this house.
- Do not add punctuation after e.g. and i.e. (Oxford style).
- Keep documentation high-level; include implementation details only when
  needed.
- Do not fully qualify enum names and values.
- Configuration options, including enum names and values, are sorted in
  alphabetical order.
- List items begin with a capital letter, even when they are fragments.

[1]: https://clang.llvm.org/docs/ClangFormat.html
[2]: https://clang.llvm.org/docs/ClangFormatStyleOptions.html
