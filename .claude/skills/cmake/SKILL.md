---
name: cmake
description: >
  Builds and tests source code using CMake. Use when source code in the project
  needs to be built or tested.
---

# CMake

## Context

- A top-level `build` directory is used to maintain build files.
- Build files are generated when the `build` directory is missing or empty.
- Build files are generated before source code is built.
- Source code is built before running tests.
- The build is cleaned by deleting the `build` directory.

## Commands

- Commands are called from the project root directory.
- Do not add or omit flags or arguments to the command line.
- Ignore output unless exit status is non-zero.
- If an error is encountered, do not attempt to fix the issue.

### Generate Build Files

- On Windows, use the `Visual Studio 17 2022` generator.
- On Linux and macOS, use the `Ninja` generator if `ninja` is found in `$PATH`.
  Otherwise, use the `Unix Makefiles` generator.
- Save a memory for which generator is used.
- Log output to a temporary file named `cmake-generate.log`.
- To generate build files, issue:

  ```shell
  cmake -B <path-to-build> -G <generator-name> $ARGUMENTS
  ```

### Build Source Code

- Log output to a temporary file named `cmake-build.log`.
- To build source code, issue:

  ```shell
  cmake --build <path-to-build> $ARGUMENTS
  ```

### Test Source Code

- To test source code, issue:

  ```shell
  ctest --output-on-failure --test-dir <path-to-build> $ARGUMENTS
  ```
