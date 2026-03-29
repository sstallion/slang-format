# Architecture

slang-format is an experimental SystemVerilog code formatter based on [slang][1]
and heavily inspired by [clang-format][2]. Verilog-2005 (IEEE 1364-2005) is
fully supported as a subset of the SystemVerilog language. All major operating
systems are supported, including Linux, macOS, and Windows.

## Directory Structure

```
slang-format/
├── source/
│   ├── Format.cpp        # Single-pass formatting and post-processing
│   ├── Format.h          # Public interface for formatting and post-processing
│   ├── Rewriter.cpp      # Iterative syntax rewriting
│   ├── Rewriter.h        # Public interface for syntax rewriting
│   ├── Style.cpp         # Style configuration and YAML parsing
│   ├── Style.h           # Public interface for style configuration
│   ├── SyntaxHelper.h    # Shared predicates for syntax node classification
│   └── main.cpp          # Entry point; writes formatted output
└── test/
    ├── FormatTest.cpp    # Formatting and post-processing tests
    ├── RewriterTest.cpp  # Syntax rewriting tests
    ├── StyleTest.cpp     # Style configuration and YAML parsing tests
    └── TestHelper.h      # Utilities for writing tests
```

## Dependencies

- **[slang][1]** - SystemVerilog compilation, elaboration, and analysis
- **[yaml-cpp][3]** - Parses configuration files written in YAML
- **[GoogleTest][4]** - Testing and mocking framework

## Formatting Flow

```
input
  │
  ▼
getStyle(cwd)                              # Walks directory hierarchy for configuration
  │
  ▼
reformat(source, style)
  │
  ├─ slang::SyntaxTree::fromText(source)   # Parses input
  │
  ├─ applyBeginEndInsertion(tree, style)   # Iterative SyntaxRewriter
  │   └─ Wrap bare statement bodies in begin/end blocks
  │
  ├─ FormatPrinter::print(tree)            # Single-pass tree walk
  │   ├─ Emit tokens with computed indentation
  │   ├─ Enforce break rules
  │   ├─ Limit consecutive empty lines
  │   └─ Honor formatting pragmas
  │
  └─ applyOneLineFormatOff()               # Optional post-processing
  │
  ▼
output
```

## Architectural Decisions

### Configuration Search

The configuration loader walks upward from the current directory until it
finds a `.slang-format` file or reaches the filesystem root, returning default
settings if none is found. This mirrors clang-format's `.clang-format` lookup
strategy, allowing per-project configuration without explicit path arguments.

### Separate Structural and Formatting Passes

Structural AST changes (begin/end insertion) are handled by a dedicated
rewrite pass that produces a modified syntax tree before any formatting
occurs. All other formatting - indentation, break insertion, empty line
limiting, pragma handling - is performed by a read-only pass that emits
formatted output directly from the rewritten tree.

This separation keeps each concern in its natural abstraction: the rewrite
pass operates on tree structure, the formatting pass operates on token
emission.

### Iterative Rewriting

slang's syntax rewriter applies all registered changes atomically. When a
node is replaced, the replacement is not re-visited for further changes.
This means wrapping nested constructs (e.g., an `if` body nested inside an
`always` body) cannot be done in a single pass.

The begin/end insertion pass handles this by iterating: each pass wraps the
outermost bare statements, and subsequent passes handle newly exposed inner
ones. Convergence is detected when a pass produces no changes. Typical code
converges in 1-2 iterations; worst case is O(nesting depth).

### Single-Pass Formatting

All formatting - indentation, break insertion, empty line limiting, pragma
handling - is performed in a single traversal of the already-rewritten tree.
Carrying formatting state across the walk is straightforward to reason about,
and the single-pass constraint keeps the implementation linear and
predictable.

### Post-Processing

The `OneLineFormatOffRegex` option strips indentation from lines matching a
regex. This is applied as a second pass over the already-formatted string
rather than inline during the tree walk, because the regex operates on final
output content that is not known until after the walk is complete.

[1]: https://sv-lang.com/
[2]: https://clang.llvm.org/docs/ClangFormat.html
[3]: https://github.com/jbeder/yaml-cpp
[4]: https://github.com/google/googletest
