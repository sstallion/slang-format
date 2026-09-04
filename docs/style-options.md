# Style Options

slang-format reads style options from a YAML configuration file named
`.slang-format` or `_slang-format`. When formatting a file, slang-format
searches for configuration starting from the directory containing the source
file and walks up the directory hierarchy to the filesystem root. The first file
found is used; default settings are applied when none is found.

The configuration file uses the format:

```yaml
key1: value1
key2: value2
# A comment.
...
```

An example of a configuration file:

```yaml
IndentWidth: 4
MaxEmptyLinesToKeep: 2
```

## Disabling Formatting

slang-format understands special comments that suspend formatting in a delimited
range. Use `// slang-format off` to disable and `// slang-format on` to
re-enable. Block comment forms (`/* slang-format off */` and
`/* slang-format on */`) are also supported.

```sv
module foo;
  // slang-format off
assign x = 1;   // emitted verbatim
  // slang-format on
  assign y = 2;  // formatted normally
endmodule
```

`OneLineFormatOffRegex` offers a complementary mechanism: it suppresses
indentation on lines matching a regular expression, without requiring inline
comments. See [`OneLineFormatOffRegex`](#onelineformatoffregex-string) for
details.

## Configurable Options

### AlignConsecutiveAssignments (AlignConsecutiveStyle)

Controls alignment of assignment operators in consecutive assignments. When
enabled, slang-format aligns assignment operators in adjacent lines by padding
with trailing spaces. All assignment types are supported: declaration
initializers (e.g. `logic a = 1;`), continuous assignments
(e.g. `assign x = 1;`), blocking assignments (`=`), nonblocking assignments
(`<=`), and compound assignments (`+=`, `-=`, `<<=`, etc.). Continuation lines
of comma-separated declaration lists are included in alignment groups. Alignment
groups are scoped by AST depth; scope boundaries such as `module`/`endmodule`
and `always`/`end` break groups, while control flow at the same depth
(`if`/`else`, `case`/`endcase`) does not. `Enabled` is a master switch; the
remaining sub-options take effect only when `Enabled` is `true`.

**Default:**

```yaml
AlignConsecutiveAssignments:
  AcrossComments: false
  AcrossEmptyLines: false
  AcrossParameterPortList: false
  Enabled: false
```

#### AcrossComments (bool)

If `true`, comments do not break alignment groups.

**Default:** `false`

#### AcrossEmptyLines (bool)

If `true`, empty lines do not break alignment groups.

**Default:** `false`

#### AcrossParameterPortList (bool)

If `true`, port list boundaries do not break alignment groups.

**Default:** `false`

#### Enabled (bool)

If `false`, disables all alignment regardless of other options.

**Default:** `false`

```sv
// Enabled: true (declaration initializers)
module foo;
  logic a       = 1;
  logic [7:0] b = 2;
endmodule

// Enabled: true (continuous assignments)
module foo;
  assign x        = 1;
  assign longname = 2;
endmodule

// Enabled: true (blocking assignments)
always_comb begin
  a        = 1;
  longname = 2;
end

// Enabled: true (nonblocking assignments)
always_ff @(posedge clk) begin
  a        <= 1;
  longname <= 2;
end

// Enabled: true (continuation lines)
module foo;
  localparam [1:0] STATE_0 = 1,
    STATE_1                = 2,
    STATE_2                = 3;
endmodule

// Enabled: false (default)
module foo;
  logic a = 1;
  logic [7:0] b = 2;
endmodule
```

---

### AlignConsecutiveDeclarations (AlignConsecutiveStyle)

Controls alignment of signal names in consecutive declarations. When enabled,
slang-format aligns identifiers in adjacent declarations by padding the type
specifier with trailing spaces. Continuation lines of comma-separated
declaration lists are included in alignment groups. Non-declaration statements
always break alignment groups. `Enabled` is a master switch; the remaining
sub-options take effect only when `Enabled` is `true`.

**Default:**

```yaml
AlignConsecutiveDeclarations:
  AcrossComments: false
  AcrossEmptyLines: false
  AcrossParameterPortList: false
  Enabled: false
```

#### AcrossComments (bool)

If `true`, comments do not break alignment groups.

**Default:** `false`

#### AcrossEmptyLines (bool)

If `true`, empty lines do not break alignment groups.

**Default:** `false`

#### AcrossParameterPortList (bool)

If `true`, port list boundaries do not break alignment groups.

**Default:** `false`

```sv
// AcrossParameterPortList: true
module foo #(
  parameter             N = 4,
  parameter logic [7:0] M = 8
) (
  input                 a,
  input logic [7:0]     b
);
  logic                 c;
  logic [15:0]          d;
endmodule
```

#### Enabled (bool)

If `false`, disables all alignment regardless of other options.

**Default:** `false`

```sv
// Enabled: true
module foo;
  logic        a;
  logic [7:0]  b;
  logic [15:0] c;
endmodule

// Enabled: true (continuation lines)
module foo;
  localparam [1:0] STATE_0 = 1,
                   STATE_1 = 2,
                   STATE_2 = 3;
endmodule

// Enabled: false (default)
module foo;
  logic a;
  logic [7:0] b;
  logic [15:0] c;
endmodule
```

---

### AlignConsecutivePackedDimensions (AlignConsecutiveStyle)

Controls alignment of packed dimensions in consecutive declarations. When
enabled, slang-format aligns the opening bracket `[` of packed dimensions in
adjacent declarations by padding the type specifier with trailing spaces. Only
declarations that have packed dimensions participate in alignment groups;
declarations without packed dimensions are skipped. Content within brackets can
be independently padded and colon-aligned using the `AlignColon`, `PadLeft`, and
`PadRight` sub-options. `Enabled` is a master switch; the remaining sub-options
take effect only when `Enabled` is `true`.

**Default:**

```yaml
AlignConsecutivePackedDimensions:
  AcrossComments: false
  AcrossEmptyLines: false
  AcrossParameterPortList: false
  AlignColon: false
  Enabled: false
  PadLeft: false
  PadRight: false
```

#### AcrossComments (bool)

If `true`, comments do not break alignment groups.

**Default:** `false`

#### AcrossEmptyLines (bool)

If `true`, empty lines do not break alignment groups.

**Default:** `false`

#### AcrossParameterPortList (bool)

If `true`, port list boundaries do not break alignment groups.

**Default:** `false`

#### AlignColon (bool)

If `true`, right-justify the left-side value of packed dimension ranges to align
`:` separators across a group. When combined with `PadLeft` or `PadRight`, those
options independently pad the right-side value after the `:`. For dimensions
without a `:`, this option has no effect and `PadLeft`/`PadRight` apply to the
whole content.

**Default:** `false`

```sv
// AlignColon: true
module foo;
  logic [ 7:0] a;
  logic [15:0] b;
endmodule

// AlignColon: true, PadLeft: true
module foo;
  logic [ 7: 0] a;
  logic [15: 0] b;
endmodule
```

#### Enabled (bool)

If `false`, disables all alignment regardless of other options.

**Default:** `false`

```sv
// Enabled: true
module foo;
  bit   [7:0] a;
  logic [7:0] b;
endmodule

// Enabled: false (default)
module foo;
  bit [7:0] a;
  logic [7:0] b;
endmodule
```

#### PadLeft (bool)

If `true`, left-pad content within brackets to right-justify values. Takes
precedence over `PadRight` when both are set.

**Default:** `false`

```sv
// PadLeft: true
module foo;
  logic [  7:0] a;
  logic [ 15:0] b;
  logic [127:0] c;
endmodule
```

#### PadRight (bool)

If `true`, right-pad content within brackets to left-justify values. `PadLeft`
takes precedence when both are `true`.

**Default:** `false`

```sv
// PadRight: true
module foo;
  logic [7:0  ] a;
  logic [15:0 ] b;
  logic [127:0] c;
endmodule
```

---

### AlignConsecutiveTimingControls (AlignConsecutiveStyle)

Controls alignment of signal names and assignment operators in consecutive
timing controls. When enabled, slang-format aligns signal names and assignment
operators in adjacent `always` statements with delay expressions
(e.g. `always #5 clk = ~clk;`) by padding with trailing spaces. `Enabled` is a
master switch; the remaining sub-options take effect only when `Enabled` is
`true`.

**Default:**

```yaml
AlignConsecutiveTimingControls:
  AcrossComments: false
  AcrossEmptyLines: false
  Enabled: false
```

#### AcrossComments (bool)

If `true`, comments do not break alignment groups.

**Default:** `false`

#### AcrossEmptyLines (bool)

If `true`, empty lines do not break alignment groups.

**Default:** `false`

#### Enabled (bool)

If `false`, disables all alignment regardless of other options.

**Default:** `false`

```sv
// Enabled: true
module foo;
  always #5  clk_i  = ~clk_i;
  always #20 dclk_i = ~dclk_i;
endmodule

// Enabled: false (default)
module foo;
  always #5 clk_i = ~clk_i;
  always #20 dclk_i = ~dclk_i;
endmodule
```

---

### AlignTrailingComments (AlignConsecutiveStyle)

Controls alignment of trailing comments in consecutive lines. When enabled,
slang-format aligns trailing comments (`//` and `/* */`) on adjacent code lines
by padding with spaces. Only comments that appear after code on the same line
are considered; standalone comment lines are not affected. `Enabled` is a master
switch; the remaining sub-options take effect only when `Enabled` is `true`.

**Default:**

```yaml
AlignTrailingComments:
  AcrossComments: false
  AcrossEmptyLines: false
  AcrossParameterPortList: false
  Enabled: false
```

#### AcrossComments (bool)

If `true`, comments do not break alignment groups.

**Default:** `false`

#### AcrossEmptyLines (bool)

If `true`, empty lines do not break alignment groups.

**Default:** `false`

#### AcrossParameterPortList (bool)

If `true`, port list boundaries do not break alignment groups.

**Default:** `false`

#### Enabled (bool)

If `false`, disables all alignment regardless of other options.

**Default:** `false`

```sv
// Enabled: true
module foo;
  assign x = 1;        // first
  assign longname = 2; // second
endmodule

// Enabled: false (default)
module foo;
  assign x = 1; // first
  assign longname = 2; // second
endmodule
```

---

### BreakAfterAlways (BreakAfterBlockStyle)

Controls whether slang-format inserts a newline between an `always`,
`always_comb`, `always_ff`, or `always_latch` header and its body.

**Default:** `OnlyMultiline`

| Value           | Description                                                       |
| --------------- | ----------------------------------------------------------------- |
| `Always`        | Always insert a newline between the header and its body.          |
| `Never`         | Never insert a newline; body follows the header on the same line. |
| `OnlyMultiline` | Insert a newline only when the body spans multiple source lines.  |

`Always` - break always inserted even for a single-statement body:

```sv
module foo;
  always_comb
    x = y;
endmodule
```

`Never` - no break inserted even for a multi-statement body:

```sv
module foo;
  always_comb begin
    x = 1;
    y = 2;
  end
endmodule
```

`OnlyMultiline` - break inserted before a multi-statement body:

```sv
module foo;
  always_comb if (a) x = 1;
endmodule
```

---

### BreakAfterBegin (bool)

If `true`, enforce a newline immediately after every `begin` token whose first
item is not already on the next line.

**Default:** `true`

`BreakAfterBegin: true` (default):

```sv
module foo;
  always_comb begin
    x = 1;
  end
endmodule
```

`BreakAfterBegin: false`:

```sv
module foo;
  always_comb begin x = 1; end
endmodule
```

---

### BreakAfterInitial (BreakAfterBlockStyle)

Controls whether slang-format inserts a newline between an `initial` or `final`
header and its body. Accepts the same values as
[`BreakAfterAlways`](#breakafteralways-breakafterblockstyle).

**Default:** `OnlyMultiline`

`Always` - break always inserted even for a single-statement body:

```sv
module foo;
  initial begin
    x = 1;
  end
endmodule
```

`Never` - no break inserted even for a multi-statement body:

```sv
module foo;
  initial begin x = 1; y = 2; end
endmodule
```

`OnlyMultiline` - break inserted before a multi-statement body:

```sv
module foo;
  initial begin
    x = 1;
    y = 2;
  end
endmodule
```

---

### BreakBeforeAlways (BreakAfterBlockStyle)

Controls whether slang-format inserts a blank line before an `always`,
`always_comb`, `always_ff`, or `always_latch` block. Comments immediately
preceding the block are kept together with it; the blank line is inserted before
the first attached comment. Accepts the same values as
[`BreakAfterAlways`](#breakafteralways-breakafterblockstyle).

**Default:** `OnlyMultiline`

`Always` - blank line always inserted:

```sv
module foo;

  always_comb begin
    x = 1;
  end
endmodule
```

`Never` - no blank line inserted:

```sv
module foo;
  always_comb begin
    x = 1;
    y = 2;
  end
endmodule
```

`OnlyMultiline` - blank line inserted before a multi-statement body:

```sv
module foo;

  always_comb begin
    x = 1;
    y = 2;
  end
endmodule
```

---

### BreakBeforeInitial (BreakAfterBlockStyle)

Controls whether slang-format inserts a blank line before an `initial` or
`final` block. Comments immediately preceding the block are kept together with
it; the blank line is inserted before the first attached comment. Accepts the
same values as
[`BreakAfterAlways`](#breakafteralways-breakafterblockstyle).

**Default:** `OnlyMultiline`

`Always` - blank line always inserted:

```sv
module foo;

  initial begin
    x = 1;
  end
endmodule
```

`Never` - no blank line inserted:

```sv
module foo;
  initial begin
    x = 1;
    y = 2;
  end
endmodule
```

`OnlyMultiline` - blank line inserted before a multi-statement body:

```sv
module foo;

  initial begin
    x = 1;
    y = 2;
  end
endmodule
```

---

### BreakBeforeSpecifyBlock (bool)

If `true`, insert a blank line before `specify` blocks. Comments immediately
preceding the block are kept together with it; the blank line is inserted before
the first attached comment.

**Default:** `true`

`BreakBeforeSpecifyBlock: true` (default):

```sv
module foo;
  assign x = 1;

  specify
    $setup(posedge clk, data, 10);
  endspecify
endmodule
```

`BreakBeforeSpecifyBlock: false`:

```sv
module foo;
  assign x = 1;
  specify
    $setup(posedge clk, data, 10);
  endspecify
endmodule
```

---

### BreakBeforeTask (bool)

If `true`, insert a blank line before `task` declarations. Comments immediately
preceding the declaration are kept together with it; the blank line is inserted
before the first attached comment.

**Default:** `true`

`BreakBeforeTask: true` (default):

```sv
module foo;
  assign x = 1;

  task bar;
    x = 1;
  endtask
endmodule
```

`BreakBeforeTask: false`:

```sv
module foo;
  assign x = 1;
  task bar;
    x = 1;
  endtask
endmodule
```

---

### BreakBeforeEnd (bool)

If `true`, enforce a newline before every `end` token that is not already on its
own line.

**Default:** `true`

`BreakBeforeEnd: true` (default):

```sv
module foo;
  always_comb begin
    x = 1;
  end
endmodule
```

`BreakBeforeEnd: false`:

```sv
module foo;
  always_comb begin x = 1; end
endmodule
```

---

### BreakBeforeFunction (bool)

If `true`, insert a blank line before `function` declarations. Comments
immediately preceding the declaration are kept together with it; the blank line
is inserted before the first attached comment.

**Default:** `true`

`BreakBeforeFunction: true` (default):

```sv
module foo;
  assign x = 1;

  function void bar;
    x = 1;
  endfunction
endmodule
```

`BreakBeforeFunction: false`:

```sv
module foo;
  assign x = 1;
  function void bar;
    x = 1;
  endfunction
endmodule
```

---

### ContinuationIndentWidth (unsigned)

Number of spaces added for continuation lines - lines that syntactically
continue the previous line (for example, an operand in a multi-line expression)
- relative to the enclosing statement.

If not explicitly set, defaults to `IndentWidth`.

**Default:** `2`

`ContinuationIndentWidth: 2` (default):

```sv
module foo;
  assign x = a +
    b;
endmodule
```

`ContinuationIndentWidth: 4`:

```sv
module foo;
  assign x = a +
      b;
endmodule
```

---

### IndentCaseItem (bool)

If `true`, indent statements that follow a `case` label when they appear on the
next line. Has no effect when the statement appears on the same line as the
label or when the label is immediately followed by `begin`.

**Default:** `true`

`IndentCaseItem: true` (default):

```sv
module foo;
  always_comb
    case (x)
      2'b00:
        y = 0;
      default:
        y = 1;
    endcase
endmodule
```

`IndentCaseItem: false`:

```sv
module foo;
  always_comb
    case (x)
      2'b00: y = 0;
      default: y = 1;
    endcase
endmodule
```

---

### IndentWidth (unsigned)

Number of spaces for each indentation level.

**Default:** `2`

`IndentWidth: 2` (default):

```sv
module foo;
  assign x = 1;
endmodule
```

`IndentWidth: 4`:

```sv
module foo;
    assign x = 1;
endmodule
```

---

### InsertBeginEnd (InsertBeginEndStyle)

Controls whether slang-format inserts `begin`/`end` blocks around bare
single-statement bodies. `Enabled` is a master switch; the remaining sub-options
take effect only when `Enabled` is `true`.

**Default:**

```yaml
InsertBeginEnd:
  Enabled: false
  AlwaysStatements: true
  ControlStatements: true
  InitialStatements: true
```

#### AlwaysStatements (bool)

If `true`, wrap bare statement bodies of `always`, `always_comb`, `always_ff`,
and `always_latch` blocks with `begin`/`end`.

**Default:** `true`

```sv
// Before
module foo;
  always_ff @(posedge clk)
    x <= 1;
endmodule

// After
module foo;
  always_ff @(posedge clk) begin
    x <= 1;
  end
endmodule
```

#### ControlStatements (bool)

If `true`, wrap bare single-statement bodies of control constructs - `if`,
`else`, `for`, `while`, `foreach`, `repeat`, `forever`, `do`-`while`, and `case`
items - with `begin`/`end`.

**Default:** `true`

```sv
// Before
module foo;
  always_comb
    if (a)
      x = 1;
endmodule

// After
module foo;
  always_comb
    if (a) begin
      x = 1;
    end
endmodule
```

#### InitialStatements (bool)

If `true`, wrap bare statement bodies of `initial` and `final` blocks with
`begin`/`end`.

**Default:** `true`

```sv
// Before
module foo;
  initial
    x = 1;
endmodule

// After
module foo;
  initial begin
    x = 1;
  end
endmodule
```

---

### MaxEmptyLinesToKeep (unsigned)

Maximum number of consecutive empty lines to keep. slang-format collapses any
run of empty lines exceeding this limit. Empty lines inside a
`// slang-format off` region are not affected.

**Default:** `1`

`MaxEmptyLinesToKeep: 1` (default) - two empty lines collapsed to one:

```sv
module foo;
  assign x = 1;

  assign y = 2;
endmodule
```

`MaxEmptyLinesToKeep: 0` - all empty lines removed:

```sv
module foo;
  assign x = 1;
  assign y = 2;
endmodule
```

---

### OneLineFormatOffRegex (string)

slang-format emits any formatted output line whose entire content matches this
regular expression verbatim - without the indentation it would ordinarily apply.

An empty string disables this feature.

**Default:** `""`

`OneLineFormatOffRegex: ".*STATE_.*"`:

```sv
// Before
module foo;
assign x = STATE_RESET;
assign y = 2;
endmodule

// After
module foo;
assign x = STATE_RESET;
  assign y = 2;
endmodule
```

---

### PackedDimensionBounds (PackedDimensionBoundsStyle)

Controls the ordering of bounds in packed dimension ranges. When set to
`MSBFirst` or `LSBFirst`, slang-format swaps the left and right bounds of
packed dimension ranges so that the larger or smaller value appears first,
respectively. Only dimensions where both bounds are integer literals are
affected; dimensions with non-literal expressions (e.g. parameters, binary
expressions) are left unchanged.

**Default:** `Preserve`

| Value      | Description                                    |
| ---------- | ---------------------------------------------- |
| `LSBFirst` | The smaller value appears as the left bound.   |
| `MSBFirst` | The larger value appears as the left bound.    |
| `Preserve` | Bounds are left as-is.                         |

`MSBFirst` - larger value placed on the left:

```sv
// Before
module foo;
  logic [0:7] x;
endmodule

// After
module foo;
  logic [7:0] x;
endmodule
```

`LSBFirst` - smaller value placed on the left:

```sv
// Before
module foo;
  logic [7:0] x;
endmodule

// After
module foo;
  logic [0:7] x;
endmodule
```

`Preserve` (default) - bounds are left unchanged:

```sv
module foo;
  logic [0:7] x;
endmodule
```

---

### ParameterPortListIndentWidth (unsigned)

Number of spaces used to indent items in a module ANSI port declaration. When
the module has a parameter port list (`#( ... )`), this value governs both the
parameter list and the following port list. When only a port list is present,
`IndentWidth` applies instead.

If not explicitly set, defaults to `ContinuationIndentWidth`, which in turn
defaults to `IndentWidth`.

**Default:** `2`

`ParameterPortListIndentWidth: 2` (default):

```sv
module foo #(
  parameter N = 4
) (
  input a
);
endmodule
```

`ParameterPortListIndentWidth: 4`:

```sv
module foo #(
    parameter N = 4
) (
    input a
);
endmodule
```

[1]: https://sv-lang.com/
[2]: https://clang.llvm.org/docs/ClangFormat.html
