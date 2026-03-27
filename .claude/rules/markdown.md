---
paths:
  - '**/*.md'
---

# Markdown Rules

- Use [GitHub Flavored Markdown][1].
- Use the Google [Markdown style guide][2] with the following amendments:
  - Table rows use leading and trailing pipes.
  - Table columns are aligned with spaces.
  - Table cells are bracketed with spaces.
  - Reference links are preferred.
  - Reference links appear at the end of the file.
  - Reference links are numbered sequentially in order of appearance.
  - When modifying reference links, re-number existing references.
- Use 7-bit ASCII when writing text; never use emojis.
- Extended ASCII may be used when creating diagrams.
- Use HTML character entities for special characters.
- Use special characters only when an ASCII equivalent does not exist
  (e.g. use `-` instead of `&ndash;` or `&mdash;`). If unsure, ask the user.

[1]: https://github.github.com/gfm/
[2]: https://google.github.io/styleguide/docguide/style.html
