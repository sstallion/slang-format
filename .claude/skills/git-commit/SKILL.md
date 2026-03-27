---
name: git-commit
description: >
  Commits changes using Git. Use when changes are ready to be submitted to
  source control.
---

# Git Commit

## Context

- Add unstaged changes to the next commit.
- Commit messages are composed of a subject followed by a body.
- The subject summarizes changes using an imperative clause.
- The subject line is limited to 50 characters.
- The body summarizes the reason using third-person singular present tense with
  proper sentence structure.
- The body is separated from the subject by a newline, is limited to 72
  characters per line, and should be no longer than 10 lines.
- If more than one change is made, use a Markdown-style unordered list.
- Commits are attributed using `Co-authored-by`.

## Commands

- Commands are called from the project root directory.
- Do not add or omit flags or arguments to the command line.
- If an error is encountered, do not attempt to fix the issue.

### Add Changes

To add unstaged changes to the next commit, issue:

```shell
git add $ARGUMENTS <file> ...
```

### Commit Changes

To commit changes, issue:

```shell
git commit $ARGUMENTS -m <message>
```
