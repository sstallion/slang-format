---
name: clang-format
description: >
  Formats source code using clang-format. Use when source code in the project
  needs to be formatted.
allowed-tools: Bash(clang-format*)
---

# clang-format

- If `clang-format` is not on `$PATH`, use the newest version-suffixed
  executable found there (e.g. `clang-format-<version>` or
  `clang-format-mp-<version>`).
- Run the command exactly as written, adding only `$ARGUMENTS` and the files to
  format. Ignore output unless the exit status is non-zero.

## Format Source Code

```
clang-format -i $ARGUMENTS <file> ...
```
