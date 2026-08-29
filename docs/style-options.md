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

The `OneLineFormatOffRegex` option provides a complementary mechanism for
suppressing indentation on lines that match a regular expression without
requiring inline comments. See [`OneLineFormatOffRegex`](#onelineformatoffregex-string)
for details.

## Configurable Options

### AlignConsecutiveAssignments (AlignConsecutiveStyle)

Controls alignment of assignment operators in consecutive declarations. When
enabled, the `=` signs in adjacent declaration initializers are aligned by
padding with trailing spaces. Continuation lines of comma-separated declaration
lists are included in alignment groups. Non-declaration statements are never
affected; only assignments that are part of a declaration (e.g. `logic a = 1;`)
are aligned. Standalone assignments (e.g. `assign x = 1;`) are not affected.
This is a nested mapping with four sub-options. `Enabled` is a master switch;
the remaining sub-options take effect only when `Enabled` is `true`.

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

Has no effect when `Enabled` is `false`.

**Default:** `false`

#### AcrossEmptyLines (bool)

If `true`, empty lines do not break alignment groups.

Has no effect when `Enabled` is `false`.

**Default:** `false`

#### AcrossParameterPortList (bool)

If `true`, port list boundaries do not break alignment groups.

Has no effect when `Enabled` is `false`.

**Default:** `false`

#### Enabled (bool)

If `false`, disables all alignment regardless of other options.

**Default:** `false`

```sv
// Enabled: true
module foo;
  logic a       = 1;
  logic [7:0] b = 2;
endmodule

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
identifiers in adjacent declarations are aligned by padding the type specifier
with trailing spaces. Continuation lines of comma-separated declaration lists
are included in alignment groups. Non-declaration statements always break
alignment groups. This is a nested mapping with four sub-options. `Enabled` is a
master switch; the remaining sub-options take effect only when `Enabled` is
`true`.

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

Has no effect when `Enabled` is `false`.

**Default:** `false`

#### AcrossEmptyLines (bool)

If `true`, empty lines do not break alignment groups.

Has no effect when `Enabled` is `false`.

**Default:** `false`

#### AcrossParameterPortList (bool)

If `true`, port list boundaries do not break alignment groups.

Has no effect when `Enabled` is `false`.

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
enabled, the opening bracket `[` of packed dimensions in adjacent declarations
is aligned by padding the type specifier with trailing spaces. Only declarations
that have packed dimensions participate in alignment groups; declarations without
packed dimensions are skipped. Content within brackets can be independently
padded and colon-aligned using the `AlignColon`, `PadLeft`, and `PadRight`
sub-options. This is a nested mapping with seven sub-options. `Enabled` is a
master switch; the remaining sub-options take effect only when `Enabled` is
`true`.

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

Has no effect when `Enabled` is `false`.

**Default:** `false`

#### AcrossEmptyLines (bool)

If `true`, empty lines do not break alignment groups.

Has no effect when `Enabled` is `false`.

**Default:** `false`

#### AcrossParameterPortList (bool)

If `true`, port list boundaries do not break alignment groups.

Has no effect when `Enabled` is `false`.

**Default:** `false`

#### AlignColon (bool)

If `true`, right-justify the left-side value of packed dimension ranges to align
`:` separators across a group. When combined with `PadLeft` or `PadRight`, those
options independently pad the right-side value after the `:`. For dimensions
without a `:`, this option has no effect and `PadLeft`/`PadRight` apply to the
whole content.

Has no effect when `Enabled` is `false`.

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

Has no effect when `Enabled` is `false`.

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

If `true`, right-pad content within brackets to left-justify values. Has no
effect when `PadLeft` is also `true`.

Has no effect when `Enabled` is `false`.

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

### BreakAfterAlways (BreakAfterBlockStyle)

Controls whether a newline is inserted between an `always`, `always_comb`,
`always_ff`, or `always_latch` header and its body.

**Default:** `OnlyMultiline`

| Value           | Description                                                       |
| --------------- | ----------------------------------------------------------------- |
| `Always`        | Newline always inserted between the header and its body.          |
| `Never`         | No newline inserted; body follows the header on the same line.    |
| `OnlyMultiline` | Newline inserted only when the body spans multiple source lines.  |

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

Controls whether a newline is inserted between an `initial` or `final` header
and its body. Accepts the same values as [`BreakAfterAlways`](#breakafteralways-breakafterblockstyle).

**Default:** `OnlyMultiline`

| Value           | Description                                                       |
| --------------- | ----------------------------------------------------------------- |
| `Always`        | Newline always inserted between the header and its body.          |
| `Never`         | No newline inserted; body follows the header on the same line.    |
| `OnlyMultiline` | Newline inserted only when the body spans multiple source lines.  |

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

### ContinuationIndentWidth (unsigned)

Number of spaces added for continuation lines relative to the enclosing
statement. A continuation line is a syntactic continuation of the previous line
(for example, the operand of a multi-line expression).

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

Controls automatic insertion of `begin`/`end` blocks around bare single-statement
bodies. This is a nested mapping with four sub-options. `Enabled` is a master
switch; the remaining sub-options take effect only when `Enabled` is `true`.

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

Has no effect when `Enabled` is `false`.

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

Has no effect when `Enabled` is `false`.

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

Has no effect when `Enabled` is `false`.

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

Maximum number of consecutive empty lines to keep. Any run of empty lines
exceeding this limit is collapsed to the limit. Empty lines inside a
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

Any formatted output line whose entire content matches this regular expression
is emitted verbatim - without the indentation that slang-format would ordinarily
apply.

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

### ParameterPortListIndentWidth (unsigned)

Number of spaces used to indent items in a module ANSI port declaration. When a
module has a parameter port list (`#( ... )`), this option controls the
indentation of items in both the parameter list and the following port list.
When a module has a port list only, items are indented by `IndentWidth`.

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
