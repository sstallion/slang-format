---
name: git-commit
description: >
  Commits changes using Git. Use when changes are ready to be submitted to
  source control.
disable-model-invocation: true
allowed-tools: Bash(git add *) Bash(git commit *) Bash(git diff *) Bash(git status *)
---

# Git Commit

- Run commands from the project root, exactly as written, adding only
  `$ARGUMENTS`.
- Stage unstaged changes, then commit.
- The message is a subject line, a blank line, and a body:
  - The subject is an imperative clause of at most 50 characters.
  - The body explains the reason in third-person singular present tense with
    proper sentence structure, wrapped at 72 characters and at most 10 lines.
  - When more than one change is made, the body is a Markdown unordered list.
  - The body ends with a `Co-Authored-By` trailer, as required by
    `CONTRIBUTING.md`.

## Add Changes

```
git add $ARGUMENTS <file> ...
```

## Commit Changes

```
git commit $ARGUMENTS -m <message>
```
