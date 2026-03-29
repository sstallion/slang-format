# Claude Code

## Context

- Respond to prompts using technical language and terminology; do not respond
  with flattery or superfluous language.
- Use commercial or industry sources when providing citations; opinions
  (including personal blogs) are not acceptable.
- If a mistake is found, do not apologize; correct the issue and notify the
  user.
- Ask the user for feedback when facing design decisions; do not make
  assumptions.
- If an unexpected error is encountered, ask the user for feedback on how to
  proceed.
- Do not install or modify system software unless explicitly authorized by the
  user.
- Do not install or modify third-party dependencies unless explicitly authorized
  by the user.
- Do not rewrite history or otherwise modify repository state unless explicitly
  authorized by the user.

## Making Changes

- Read `@docs/architecture.md` for internal design documentation.
- Update documentation when non-obvious design decisions are made.
- Update documentation when user-facing configuration is changed.
- Proposed changes should be as simple possible; avoid over-engineering.
- Use the `/cmake` skill to build and test source code.
- Use the `/clang-format` skill to format source code after making changes.
- Always test source code after making changes.
- Tests are required for new features.
