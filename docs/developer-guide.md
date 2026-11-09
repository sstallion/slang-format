# Developer Guide                                             {#developer-guide}

Information on the internals of slang-format and its use as a library.

slang-format is designed first and foremost as a collection of software
libraries that can be used, in whole or in part, in other projects. Those might
include linting pipelines, editor integrations, code generation as part of a
build, automated refactoring tools, and custom formatting workflows. The library
is built on top of [slang][1], which provides the SystemVerilog parsing and
syntax tree infrastructure that slang-format uses to analyze and rewrite source
code. Configuration is handled through [yaml-cpp][2], and the test suite uses
[GoogleTest][3].

This section gives an overview of the slang-format library and codebase, and
serves as a starting point for learning to build on it in other projects.

- [Building from Source](@ref building) -- Instructions for obtaining and
  building the library from source, including prerequisites and CMake
  configuration options
- [Architecture](@ref architecture) -- Overview of the project's internal
  design, directory structure, formatting pipeline, and key architectural
  decisions
- [Contributing][4] -- Guidelines for contributing to the project
- [License][5] -- The MIT license

[1]: https://sv-lang.com
[2]: https://github.com/jbeder/yaml-cpp
[3]: https://github.com/google/googletest
[4]: https://github.com/sstallion/slang-format/blob/main/CONTRIBUTING.md
[5]: https://github.com/sstallion/slang-format/blob/main/LICENSE
