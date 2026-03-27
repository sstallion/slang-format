---
name: clang-format
description: >
  Formats source code using clang-format. Use when source code in the project
  needs to be formatted.
---

# clang-format

## Commands

- If the `clang-format` executable is not found, search `$PATH` for a version
  suffix (e.g. `clang-format-<version>`).
- If a suffixed executable is found, use the latest version.
- Save a memory for which executable is used.
- Do not add or omit flags or arguments to the command line.
- Ignore output unless exit status is non-zero.
- If an error is encountered, do not attempt to fix the issue.

### Format Source Code

To format source code, issue:

```shell
clang-format -i $ARGUMENTS <file> ...
```
