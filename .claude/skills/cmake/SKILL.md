---
name: cmake
description: >
  Builds and tests source code using CMake. Use when source code in the project
  needs to be built or tested.
allowed-tools: Bash(cmake *) Bash(ctest *)
---

# CMake

- Run commands from the project root, exactly as written, adding only
  `$ARGUMENTS`.
- Build files live in `build/`. Generate them when `build/` is missing or
  empty, build before testing, and clean by deleting `build/`.
- Generate and build output goes to a log so compiler output stays out of the
  conversation. Read the log only when the exit status is non-zero.
- Use `$TMPDIR` for temporary files.

## Generate Build Files

On Windows, `<generator>` is `Visual Studio 17 2022`. Otherwise it is `Ninja`
when `ninja` is on `$PATH` and `Unix Makefiles` when it is not.

```shell
cmake -B build -G <generator> $ARGUMENTS > "$TMPDIR/cmake-generate.log" 2>&1
```

## Build Source Code

```shell
cmake --build build $ARGUMENTS > "$TMPDIR/cmake-build.log" 2>&1
```

## Test Source Code

```shell
ctest --output-on-failure --test-dir build $ARGUMENTS
```
