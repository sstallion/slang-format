# User Manual                                                     {#user-manual}

Instructions on how to use slang-format as an end user.

slang-format is a standalone tool that formats SystemVerilog source code
according to configurable style options. It can be used to format files in
place, write formatted output to standard output, or read from standard input as
part of a pipeline. Verilog-2005 (IEEE 1364-2005) is fully supported as a subset
of the SystemVerilog language. All major operating systems are supported,
including Linux, macOS, and Windows.

This section covers day-to-day use of slang-format, from command-line invocation
and configuration to the full set of formatting options and interoperability with
other tools.

- [Usage](@ref usage) -- Command-line options, configuration files, response
  files, and ignore patterns
- [Style Options](@ref style-options) -- Configurable formatting options and
  their behavior, including indentation, alignment, whitespace, line breaking,
  and code rewriting
- [Tool Compatibility](@ref compatibility) -- Interoperability with other
  SystemVerilog tools and formatters
- [Changelog][1] -- Release history and notable changes

[1]: https://github.com/sstallion/slang-format/blob/main/CHANGELOG.md
