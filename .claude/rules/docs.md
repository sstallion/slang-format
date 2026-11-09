---
paths:
  - 'docs/*.md'
---

# Documentation Rules

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

The Doxygen main page (`USE_MDFILE_AS_MAINPAGE`). Content TBD.

### User Manual

An unnamed introduction with a one-line tagline describing slang-format for end
users, followed by a paragraph framing the section's scope. A bulleted list of
sub-pages with one-line descriptions follows. Changelog links to GitHub. Does
not duplicate content from sub-pages; acts as a summary hub.

#### Usage

Sections, in order:

- An unnamed introduction describing slang-format as a standalone tool, with
  command-line options as a verbatim help-text code block, response file syntax,
  stdin support, and exit codes.
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
  `Type:` line with `@ref` link for custom types or backticked name for builtins
  (`bool`, `unsigned`, `string`), description, and code examples in `sv` fenced
  code blocks. Configuration examples use `yaml` fenced code blocks. Default
  values are not documented. Options are not separated by horizontal rules.
  Cross-references to other options use `[OptionName](#optionname)` (lowercase
  anchor, no type suffix).
  - **Enum Options** After the description, `Possible values:` introduces a
    bullet list. Each bullet starts with the backticked value name followed by a
    description on the same line. Code examples are indented under their bullet.
    Enum options sharing possible values with a primary definition use `Possible
    values:` followed by `See [Primary](#anchor) for shared possible values.`
  - **Boolean Options** Describe what `true` does, starting with "If
    \`true\`,". When the option has an example, include an `Example output:`
    section with an `sv` block showing the `true` behavior. `Possible values:`
    is not used for booleans.
  - **Struct Options** Use `Example configuration:` with a YAML block, optional
    `Example output:` with an `sv` block, and `Nested style options:`
    introducing a bullet list. Each sub-option bullet is formatted as `` `type
    Name` `` (e.g. `` `bool Enabled` ``). Options sharing a struct type with a
    primary definition use `See [Primary](#anchor) for shared nested style
    options.` and document only their unique sub-options.
  - **Scalar Options** (`unsigned`, `string`) have a description. Use `Example
    configuration:` with a YAML block and `Example output:` with an `sv` block
    when behavior is non-obvious. No `Possible values:` heading.
  - Code examples are taken from the SystemVerilog LRM (IEEE Std 1800-2023), not
    invented ad hoc.
  - Struct fields that an option does not use are not documented for that
    option.
  - Struct fields are documented in ASCIIbetical order, except `Enabled` or
    `Disabled`, which are listed first.

#### Tool Compatibility

TODO.

### Developer Guide

An unnamed introduction with a one-line tagline, followed by a paragraph
positioning slang-format as a library-first project with enumerated use cases, a
second paragraph framing the section, and a bulleted list of sub-pages with
one-line descriptions. Contributing and License link to GitHub. Does not
duplicate content from sub-pages; acts as a summary hub.

#### Building from Source

TODO.

#### Architecture

Sections, in order:

- An unnamed introduction describing the project and its goals.
- **Dependencies** - Third-party libraries with a one-line description each.
- **Directory Structure** - An ASCII tree of the project's directories
  directories with a one-line description of each directory's role.
- **Formatting Flow** - A Mermaid flowchart tracing the path from input to
  output.
- **Architectural Decisions** - Grouped by layer (Organization, Configuration,
  Rewriting, Formatting, Post-Processing, Testing), with one subsection per
  decision explaining the approach and its rationale.

### API Reference

An unnamed introduction with a one-line tagline, followed by a paragraph
describing the `slang::format` namespace and the library's purpose. A bulleted
list of key entry points follows, with fully qualified C++ names and one-sentence
descriptions. A closing sentence points to the Developer Guide for integration
guidance. Does not duplicate content from sub-pages; acts as a summary hub.

[1]: https://clang.llvm.org/docs/ClangFormat.html
[2]: https://clang.llvm.org/docs/ClangFormatStyleOptions.html
