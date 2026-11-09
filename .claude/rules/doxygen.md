---
paths:
  - '**/*.{cpp,h}'
  - 'docs/*.{dox,md}'
---

# Doxygen Rules

Source code and documents under `docs/` are processed by Doxygen and are written
for human readers first. Do not shorten, remove, or relocate documentation
because it would be cheaper or more convenient for Claude to load. Control what
Claude reads with path-scoped rules under `.claude/rules/`, not by trimming the
documents.

## Doxygen Conventions

- Use Javadoc-style commands (e.g. `@ref`, not `\ref`).
- Every `docs/*.md` file (except `index.md` and `todo.md`) has a top-level label
  `{#labelid}`. The header line is padded with spaces so it is exactly 80
  characters wide. The label is the stem of the file.
- Pages with subsections include `@tableofcontents` on line 3 (after a blank
  line following the heading). The label matches the stem of the file name.
- References use `@ref` inside Markdown links, e.g. `[Link Text](@ref labelid)`.

## Documentation Style

- Oxford commas are not optional.
- Do not add punctuation after e.g. and i.e. (i.e. Oxford style).
- Keep documentation high-level; include implementation details only when
  needed.
- Configuration options, including enum names and values, are sorted in
  alphabetical order.
- List items begin with a capital letter, even when they are fragments.
- Use title case when identifying headings and sections.
- Do not abbreviate standard input, standard output, and standard error as
  stdin, stdout, and stderr, respectively.
