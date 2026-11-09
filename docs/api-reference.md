# API Reference                                                 {#api-reference}

Documentation for the public slang-format C++ API.

The `slang::format` namespace contains the public interface for the slang-format
library. slang-format is designed as a reusable formatting engine that can be
embedded in other tools and workflows, including linting pipelines, editor
integrations, and custom formatting tools. The library parses SystemVerilog
source code using [slang][1]'s syntax tree facilities, applies configurable
style options, and produces reformatted output. Verilog-2005 (IEEE 1364-2005) is
fully supported as a subset of the SystemVerilog language.

See the individual listing pages for a complete reference, but some useful
starting points for exploration are:

- The `slang::format::reformat()` functions accept SystemVerilog source text or
  a stream and return the formatted result according to a given style. These are
  the main entry points for formatting operations.
- The `slang::format::Style` struct defines all configurable formatting options,
  from indentation and alignment to whitespace and code rewriting. Each field
  corresponds to a key in the `.slang-format` configuration file.
- The `slang::format::getDefaultStyle()` function returns a `Style` populated
  with the default values for all options. This is useful as a starting point
  for programmatic style customization.
- The `slang::format::VersionInfo` class provides compile-time version
  information about the library, including major, minor, patch, prerelease, and
  build metadata components.
- Supporting types such as `slang::format::AlignConsecutiveStyle`,
  `slang::format::InsertBeginEndStyle`, and the various enum types define the
  structure and possible values for composite style options.

See the [Developer Guide](@ref developer-guide) for information on building and
integrating the library.

[1]: https://sv-lang.com
