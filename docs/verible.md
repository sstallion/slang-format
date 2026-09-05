# Verible Compatibility

This document tracks the gap analysis between [Verible][1]'s default formatting
behavior and slang-format's current capabilities. The goal is to determine what
features are missing such that a Verible style can be emulated perfectly using
slang-format.

The analysis is based on Verible's CLI defaults (from `format-style-init.cc`),
not the internal struct defaults used by its test suite. Several Verible struct
defaults are overridden by CLI flag defaults; the CLI values are what users
experience when running `verible-verilog-format` with no flags.

## Architectural Context

slang-format is a preserve-and-indent formatter: it replaces leading whitespace
(indentation) but preserves all inter-token spacing on a line. Verible is a
normalize-all-whitespace formatter: it rewrites spacing between every token pair
according to context-sensitive rules hardcoded in `token-annotator.cc`.

This distinction is the most significant architectural difference between the
two tools and drives the largest gap identified below.

## Already Matching

The following Verible defaults are already produced by slang-format's default
configuration with no changes needed:

| Verible Default                   | slang-format Equivalent                              |
| --------------------------------- | ---------------------------------------------------- |
| `indentation_spaces: 2`           | `IndentWidth: 2`                                     |
| Module body indented              | `ModuleDeclarationSyntax` handles all four kinds     |
| Case item indentation             | `IndentCaseItem: true`                               |
| Nested control flow               | `visitBody()` increments depth per scope level       |
| Content on new line after `begin` | `BreakAfterBegin: true`                              |
| Newline before `end`              | `BreakBeforeEnd: true`                               |
| Max 1 consecutive empty line      | `MaxEmptyLinesToKeep: 1`                             |
| No auto begin/end insertion       | `InsertBeginEnd.Enabled: false`                      |
| No parenthesis insertion          | All `InsertParens` fields false                      |
| Event separator preserved         | `EventSeparator: Preserve`                           |
| Dimension bounds preserved        | `PackedDimensionBounds: Preserve`                    |
| Alignment off                     | All `AlignConsecutive*` disabled                     |

In slang's syntax tree, `ModuleDeclarationSyntax` covers `module`, `interface`,
`package`, and `program` declarations via a unified `isKind()` predicate. All
four construct types are therefore handled by the existing `handle()` method.

## Configuration-Only Matches

The following Verible defaults can be matched by changing slang-format
configuration values without any code changes:

| Verible Behavior                             | Required Configuration              |
| -------------------------------------------- | ----------------------------------- |
| Port declarations at 4-space indent          | `ParameterPortListIndentWidth: 4`   |
| Continuation lines at 4-space indent         | `ContinuationIndentWidth: 4`        |
| No blank line before `always`/`initial`      | `BreakBeforeAlways: Never`          |
|                                              | `BreakBeforeInitial: Never`         |
| No break between header and body             | `BreakAfterAlways: Never`           |
|                                              | `BreakAfterInitial: Never`          |
| No blank line before `function`/`task`       | `BreakBeforeFunction: false`        |
|                                              | `BreakBeforeTask: false`            |
| No blank line before `specify`               | `BreakBeforeSpecifyBlock: false`    |

A `.slang-format` preset for the configuration-only portion would be:

```yaml
BreakAfterAlways: Never
BreakAfterInitial: Never
BreakBeforeAlways: Never
BreakBeforeFunction: false
BreakBeforeInitial: Never
BreakBeforeSpecifyBlock: false
BreakBeforeTask: false
ContinuationIndentWidth: 4
ParameterPortListIndentWidth: 4
```

Verible does not force blank lines before `function`, `task`, or `specify`
blocks; its general spacing heuristics and blank-line preservation handle
separation. slang-format's defaults insert these blank lines, so they must be
disabled for Verible emulation.

Similarly, Verible keeps `always_comb begin` on a single line rather than
forcing a break between the header and body. Setting `BreakAfterAlways` and
`BreakAfterInitial` to `Never` matches this behavior.

## Missing Features

### Token Spacing Normalization

Verible normalizes all inter-token whitespace via context-sensitive rules. The
complete rule set, as implemented in `token-annotator.cc`, includes:

- 1 space after `,` and `;`; 0 before
- 0 space inside `()`, `[]`, `{}`
- 1 space around binary and assignment operators (`+`, `-`, `*`, `=`, `<=`,
  `+=`, etc.)
- 0 space for unary prefix and postfix operators (`!`, `~`, `++`, `--`)
- 1 space around ternary `?` and `:`
- 0 space around cast `'` (e.g. `int'(x)`)
- 0 space around `.` and `::`
- 1 space between control-flow keywords (`if`, `for`, `while`, etc.) and `(`
- 0 space between function/method name and `(`
- 1 space between module instance name and `(`
- 0 space after `@`
- 0 space inside based numeric literals (e.g. `16'hFF`)
- Minimum 2 spaces before trailing end-of-line comments

slang-format preserves inter-token whitespace verbatim. In `Format.cpp`, the
`emitTrivia` method passes non-leading `Whitespace` trivia through to the output
unchanged. There is no spacing classification or normalization infrastructure.

This is the single largest gap. Without it, slang-format output will differ from
Verible on virtually any input that has non-canonical spacing. Implementing this
requires a token-context classification system that examines the preceding and
following tokens to determine appropriate spacing, and modification of the trivia
emission logic to replace inter-token whitespace rather than preserve it.

### `end else begin` Compaction

Verible places `end else begin` (and `end else` without `begin`) on a single
line. This is a mandatory formatting rule that applies regardless of the
original source layout.

slang-format preserves whatever line structure exists in the source for the
`else` clause. If the source has `end`, `else`, and `begin` on separate lines,
slang-format keeps them that way.

The `ConditionalStatementSyntax` handler would need to actively manage
whitespace between `end` and `else` (and between `else` and `begin`),
suppressing newlines and replacing them with a single space.

### `begin` Stays on Controlling Keyword Line

Verible ensures `begin` stays on the same line as its controlling keyword (`if`,
`else`, `always_comb`, `always_ff @(...)`, etc.). If the source has `begin` on a
separate line from the keyword, Verible moves it up to the keyword's line.

slang-format only adds newlines via `forceNewline()`; it never removes them or
joins lines. This means it cannot move a `begin` that is on its own line back up
to the controlling keyword's line.

This requires trivia manipulation to suppress newlines before `begin` in
specific syntactic contexts. It interacts with token spacing normalization,
since the whitespace between the keyword and `begin` needs to be normalized to a
single space.

### Mandatory Line Break Before `else`

Verible forces a line break before `else` when it is not preceded by `end` on
the same line. For example, `if (a) x = 1; else y = 2;` all on one line would
be reformatted with `else` on a new line.

slang-format preserves original `else` placement. If the source has `else` on
the same line as a bare statement, it stays there.

The `ConditionalStatementSyntax` handler needs logic to detect whether `else`
follows a bare statement (not an `end` token) and force a newline in that case.

### Scope Construct Coverage

While `ModuleDeclarationSyntax` covers `module`, `interface`, `package`, and
`program`, `ClassDeclarationSyntax` and `CovergroupDeclarationSyntax` are
separate types in slang's syntax tree. Neither has an explicit `handle()` method
in `FormatPrinter`, so their bodies fall through to `visitDefault` without depth
tracking. This means class and covergroup member indentation is not managed.

Other potentially unhandled scope constructs include checker declarations and
constraint declarations. The pattern for adding these is identical to the
existing `visitScopedBlock` helper.

### Hierarchical Instance Formatting

Verible indents named port connections (`.port(signal)`) and named parameter
assignments (`.PARAM(value)`) at `wrap_spaces` (4 spaces by default) from the
instantiation. These are specific to module/interface instantiation syntax.

slang-format has no `handle()` method for `HierarchyInstantiationSyntax`. These
constructs fall through to `visitDefault` with no depth tracking for the
port/parameter connection lists.

A handler for instantiation syntax with depth tracking for named port and
parameter lists would close this gap.

### Format-Off Pragma Compatibility

Verible recognizes `// verilog_format: off` and `// verilog_format: on` pragmas
to control formatting regions. slang-format only recognizes `// slang-format
off` and `// slang-format on`.

For codebases that use Verible's pragma syntax, slang-format would need to
accept both pragma formats. This requires extending `matchesPragma()` in
`Format.cpp` and the corresponding detection in `Align.cpp`'s `classifyLines`.

### Line Terminator Auto-Detection

Verible's `line_terminator: auto` default detects whether the source file uses
LF or CRLF line endings and preserves the detected style. slang-format
unconditionally emits `\n` (LF).

This requires scanning the input for `\r\n` sequences, a style option for the
line terminator, and using the detected or configured terminator throughout the
formatting output.

### Preprocessor Directive Line Breaks

Verible inserts mandatory line breaks before preprocessor directives
(`` `define``, `` `include``, `` `ifdef``, etc.). slang-format has no special
handling for preprocessor directives; they pass through `visitDefault` with
their original whitespace.

Detection of preprocessor directive tokens in the formatting pass with forced
newline insertion would address this.

## Not Applicable

The following Verible features are disabled by default and do not require
emulation:

| Feature                           | Reason                                           |
| --------------------------------- | ------------------------------------------------ |
| `column_limit: 100`               | Only enforced when `try_wrap_long_lines` is true |
| `try_wrap_long_lines: false`      | Line wrapping is disabled by default             |
| Line break and penalty parameters | Only used by the wrapping optimizer              |
| Port dimension right-alignment    | Disabled by default                              |
| `expand_coverpoints`              | Disabled by default (CLI override)               |

These features are part of Verible's optimizer-based line wrapping system, which
is not active under default CLI settings. If Verible users enable
`try_wrap_long_lines`, the column limit and penalty system become relevant, but
that represents a non-default configuration outside the scope of this analysis.

[1]: https://github.com/chipsalliance/verible
