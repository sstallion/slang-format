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

A configuration file may start from a predefined style by naming it with
`BasedOnStyle`; any options that follow override the predefined values:

```yaml
BasedOnStyle: Default
IndentWidth: 4
```

## Disabling Formatting

slang-format understands special comments that suspend formatting in a delimited
range. Use `// slang-format off` to disable and `// slang-format on` to
re-enable. Block comment forms (`/* slang-format off */` and
`/* slang-format on */`) are also supported.

For compatibility with [Verible][1], the `// verilog_format: off` and
`// verilog_format: on` pragmas (and their block comment equivalents) are also
recognized.

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

### AlignAfterOpenParen (BracketAlignmentStyle)

Controls alignment and wrapping of arguments when a parenthesized argument list
exceeds [`ColumnLimit`](#columnlimit-unsigned). When wrapping is triggered, this
option determines how continuation lines are indented. All constructs with
argument lists are affected, including function calls, system task calls, method
calls, elaboration system tasks, and class constructors.

This option has no effect when `ColumnLimit` is `0`.

**Default:** `Align`

| Value        | Behavior                                                                                                |
| ------------ | ------------------------------------------------------------------------------------------------------- |
| Align        | Continuation lines align to the column after the opening parenthesis.                                   |
| AlwaysBreak  | Break after the opening parenthesis; continuation lines use `ContinuationIndentWidth`.                  |
| BlockIndent  | Like AlwaysBreak, but the closing parenthesis is placed on its own line at the primary indentation.     |
| DontAlign    | No active wrapping is performed; existing breaks use `ContinuationIndentWidth`.                         |

`AlignAfterOpenParen: Align` (default):

```sv
$display("alpha", bravo, charlie,
         delta, echo_val);
```

`AlignAfterOpenParen: AlwaysBreak`:

```sv
$display(
    "alpha", bravo, charlie, delta,
    echo_val);
```

`AlignAfterOpenParen: BlockIndent`:

```sv
$display(
    "alpha", bravo, charlie, delta,
    echo_val
);
```

`AlignAfterOpenParen: DontAlign`:

```sv
// Existing source breaks are preserved.
$display("alpha", bravo, charlie, delta, echo_val);
```

---

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
  Enabled: true
  AcrossComments: false
  AcrossEmptyLines: false
  AcrossParameterPortList: false
  InferFromSource: true
  MaxPadding: 2
  MinPadding: 2
```

#### Enabled (bool)

If `false`, disables all alignment regardless of other options.

**Default:** `true`

```sv
// Enabled: true, InferFromSource: false, MaxPadding: 0 (declaration initializers)
module foo;
  logic a       = 1;
  logic [7:0] b = 2;
endmodule

// Enabled: true, InferFromSource: false, MaxPadding: 0 (continuous assignments)
module foo;
  assign x        = 1;
  assign longname = 2;
endmodule

// Enabled: true, InferFromSource: false, MaxPadding: 0 (blocking assignments)
always_comb begin
  a        = 1;
  longname = 2;
end

// Enabled: true, InferFromSource: false, MaxPadding: 0 (nonblocking assignments)
always_ff @(posedge clk) begin
  a        <= 1;
  longname <= 2;
end

// Enabled: true, InferFromSource: false, MaxPadding: 0 (continuation lines)
module foo;
  localparam [1:0] STATE_0 = 1,
    STATE_1                = 2,
    STATE_2                = 3;
endmodule

// Enabled: false
module foo;
  logic a = 1;
  logic [7:0] b = 2;
endmodule
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

#### InferFromSource (bool)

If `true`, the formatter examines original source spacing to infer whether the
author intended alignment. Groups where the source is flush-left are left
unaligned; groups where the source shows significant excess spacing are aligned.
Groups where the alignment displacement falls within `MinPadding` are always
aligned regardless of this setting.

**Default:** `true`

```sv
// InferFromSource: true (flush-left source stays flush-left)
module foo;
  assign x = 1;
  assign longname = 2;
endmodule

// InferFromSource: true (pre-aligned source triggers alignment)
module foo;
  assign x        = 1;
  assign longname = 2;
endmodule

// InferFromSource: false (always aligns when MaxPadding allows)
module foo;
  assign x        = 1;
  assign longname = 2;
endmodule
```

#### MaxPadding (unsigned)

Maximum number of spaces any line in a group may be padded to reach the
alignment column. If the required padding for any line in a group exceeds
this limit, the entire group is left unaligned. A value of `0` means
unlimited; all groups are aligned regardless of padding.

**Default:** `2`

```sv
// MaxPadding: 2 (padding is 2, within limit)
module foo #(
    parameter CLKS_PER_BIT = 4,
    parameter DATA_WIDTH   = 8
) (
    input clk_i
);
endmodule

// MaxPadding: 2 (padding is 6, exceeds limit)
module foo;
  assign x = 1;
  assign longname = 2;
endmodule
```

#### MinPadding (unsigned)

Minimum padding guarantee: groups with displacement at or below this threshold
are always aligned when `InferFromSource` is `true`, bypassing the
source-spacing inference. A value of `0` disables the guarantee. This option
has no effect when `InferFromSource` is `false`.

**Default:** `2`

```sv
// MinPadding: 2 (displacement is 2, within guarantee)
always_comb begin
  state_d = state_q;
  rdy_o   = 1'b0;
end

// MinPadding: 0 (no guarantee, InferFromSource decides)
always_comb begin
  state_d = state_q;
  rdy_o = 1'b0;
end
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
  Enabled: true
  AcrossComments: false
  AcrossEmptyLines: false
  AcrossParameterPortList: false
  InferFromSource: true
  MaxPadding: 2
```

#### Enabled (bool)

If `false`, disables all alignment regardless of other options.

**Default:** `true`

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

// Enabled: false
module foo;
  logic a;
  logic [7:0] b;
  logic [15:0] c;
endmodule
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

#### InferFromSource (bool)

See `AlignConsecutiveAssignments.InferFromSource`.

**Default:** `true`

#### MaxPadding (unsigned)

See `AlignConsecutiveAssignments.MaxPadding`.

**Default:** `2`

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
  Enabled: true
  AcrossComments: false
  AcrossEmptyLines: false
  AcrossParameterPortList: false
  AlignColon: false
  InferFromSource: true
  MaxPadding: 2
  PadLeft: true
  PadRight: false
```

#### Enabled (bool)

If `false`, disables all alignment regardless of other options.

**Default:** `true`

```sv
// Enabled: true
module foo;
  bit   [7:0] a;
  logic [7:0] b;
endmodule

// Enabled: false
module foo;
  bit [7:0] a;
  logic [7:0] b;
endmodule
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

#### InferFromSource (bool)

See `AlignConsecutiveAssignments.InferFromSource`.

**Default:** `true`

#### MaxPadding (unsigned)

See `AlignConsecutiveAssignments.MaxPadding`.

**Default:** `2`

#### PadLeft (bool)

If `true`, left-pad content within brackets to right-justify values. Takes
precedence over `PadRight` when both are set.

**Default:** `true`

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

### AlignConsecutivePortConnections (AlignConsecutiveStyle)

Controls alignment of opening parentheses in consecutive named port
connections and parameter assignments. When enabled, slang-format aligns
the `(` in adjacent `.name(expr)` lines by padding with trailing spaces
after the port name. Alignment groups are scoped by AST depth, and port
list boundaries naturally separate parameter port groups from connection
port groups. Short-form ports (`.name` without parentheses) and positional
ports are excluded. `Enabled` is a master switch; the remaining
sub-options take effect only when `Enabled` is `true`.

**Default:**

```yaml
AlignConsecutivePortConnections:
  Enabled: true
  AcrossComments: false
  AcrossEmptyLines: false
  AcrossParameterPortList: false
  InferFromSource: true
  MaxPadding: 2
  MinPadding: 2
```

#### Enabled (bool)

If `false`, disables all alignment regardless of other options.

**Default:** `true`

```sv
// Enabled: true, MaxPadding: 2
dff u_chk (
    .clk  (clk),
    .rst_n(rst_n),
    .req  (req_int),
    .gnt  (gnt_int)
);

// Enabled: false
dff u_chk (
    .clk(clk),
    .rst_n(rst_n),
    .req(req_int),
    .gnt(gnt_int)
);
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

#### InferFromSource (bool)

See `AlignConsecutiveAssignments.InferFromSource`.

**Default:** `true`

#### MaxPadding (unsigned)

Maximum number of spaces any line in a group may be padded to reach the
alignment column. If the required padding for any line in a group exceeds
this limit, the entire group is left unaligned. A value of `0` means
unlimited; all groups are aligned regardless of padding.

**Default:** `2`

```sv
// MaxPadding: 2 (padding is 2, within limit)
dff u_chk (
    .clk  (clk),
    .rst_n(rst_n)
);

// MaxPadding: 2 (padding is 5, exceeds limit)
dff u_dff_named (
    .clk(clk),
    .data_in(data_in),
    .rst_n(rst_n),
    .data_out(data_out)
);
```

#### MinPadding (unsigned)

See `AlignConsecutiveAssignments.MinPadding`.

**Default:** `2`

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
  Enabled: false
  AcrossComments: false
  AcrossEmptyLines: false
```

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

#### AcrossComments (bool)

If `true`, comments do not break alignment groups.

**Default:** `false`

#### AcrossEmptyLines (bool)

If `true`, empty lines do not break alignment groups.

**Default:** `false`

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
  Enabled: false
  AcrossComments: false
  AcrossEmptyLines: false
  AcrossParameterPortList: false
```

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

#### AcrossComments (bool)

If `true`, comments do not break alignment groups.

**Default:** `false`

#### AcrossEmptyLines (bool)

If `true`, empty lines do not break alignment groups.

**Default:** `false`

#### AcrossParameterPortList (bool)

If `true`, port list boundaries do not break alignment groups.

**Default:** `false`

---

### AllowShortConstraintBlocksOnASingleLine (bool)

If `true`, constraint blocks with a single item are joined onto one line when the
result fits within [`ColumnLimit`](#columnlimit-unsigned). Multi-item constraint
blocks are always expanded regardless of this setting. Applies to both named
constraints and `randomize() with` blocks.

**Default:** `true`

`AllowShortConstraintBlocksOnASingleLine: true` (default):

```sv
class packet;
  constraint c {kind inside {[0:7]};}
endclass
```

`AllowShortConstraintBlocksOnASingleLine: false`:

```sv
class packet;
  constraint c {
    kind inside {[0:7]};
  }
endclass
```

---

### AllowShortCoverpointsOnASingleLine (bool)

If `true`, coverpoint and cover cross bodies are joined onto one line when the
result fits within [`ColumnLimit`](#columnlimit-unsigned). Multi-item bodies that
exceed the column limit are always expanded regardless of this setting.

**Default:** `true`

`AllowShortCoverpointsOnASingleLine: true` (default):

```sv
covergroup cg;
  cp_addr: coverpoint addr {option.auto_bin_max = 8;}
endgroup
```

`AllowShortCoverpointsOnASingleLine: false`:

```sv
covergroup cg;
  cp_addr: coverpoint addr {
    option.auto_bin_max = 8;
  }
endgroup
```

---

### BasedOnStyle (string)

The style used for all options not specifically set in the configuration file.
The named style is applied first; every other option in the file then overrides
it. Style names are matched case-insensitively. An unknown style name is
reported as a configuration error.

This option is accepted only in configuration files and is not emitted by
`--dump-config`.

**Default:** `Default`

| Value     | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `Default` | The built-in style used when no configuration file is found. |

`BasedOnStyle: Default` with `IndentWidth: 4` - the default style with a
wider indent:

```yaml
BasedOnStyle: Default
IndentWidth: 4
```

---

### BinPackArguments (bool)

Controls whether arguments are bin-packed or placed one per line when wrapping
is triggered by [`AlignAfterOpenParen`](#alignafteropenparen-bracketalignmentstyle).
When `true`, the formatter packs as many arguments as fit within
[`ColumnLimit`](#columnlimit-unsigned) on each line. When `false`, each argument
is placed on its own line.

This option has no effect when `AlignAfterOpenParen` is `DontAlign` or when
`ColumnLimit` is `0`.

**Default:** `true`

`BinPackArguments: true` (default):

```sv
$display("alpha", bravo, charlie,
         delta, echo_val);
```

`BinPackArguments: false`:

```sv
$display("alpha",
         bravo,
         charlie,
         delta,
         echo_val);
```

---

### BinPackParameters (bool)

Controls whether function and task port parameters are bin-packed or placed one
per line when wrapping is triggered by
[`AlignAfterOpenParen`](#alignafteropenparen-bracketalignmentstyle). When `true`,
the formatter packs as many parameters as fit within
[`ColumnLimit`](#columnlimit-unsigned) on each line. When `false`, each parameter
is placed on its own line.

This option has no effect when `AlignAfterOpenParen` is `DontAlign` or when
`ColumnLimit` is `0`.

**Default:** `true`

`BinPackParameters: true` (default):

```sv
function automatic logic [DW-1:0] mask_bytes(
    input logic [DW-1:0] value, input logic [STRB_W-1:0] strb,
    input bit invert = 1'b0);
endfunction
```

`BinPackParameters: false`:

```sv
function automatic logic [DW-1:0] mask_bytes(
    input logic [DW-1:0] value,
    input logic [STRB_W-1:0] strb,
    input bit invert = 1'b0
);
endfunction
```

---

### BinPackPortConnections (bool)

Controls whether instance port connections are bin-packed or placed one per line.
When `true`, named port connections in module instantiations are packed on as few
lines as possible. When `false`, each port connection is placed on its own line.

**Default:** `false`

`BinPackPortConnections: false` (default):

```sv
dff u_dff (
    .clk(clk),
    .rst_n(rst_n),
    .d(data_i),
    .q(dff_q)
);
```

`BinPackPortConnections: true`:

```sv
dff u_dff (
    .clk(clk), .rst_n(rst_n), .d(data_i), .q(dff_q)
);
```

---

### BreakAfterAlways (BlockBreakStyle)

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

`OnlyMultiline` (default) - break inserted before a multi-statement body:

```sv
module foo;
  always_comb if (a) x = 1;
endmodule
```

---

### BreakAfterAssertionLabel (bool)

If `true`, insert a newline after assertion labels on concurrent assertions
(`assert property`, `assume property`, `cover property`, etc.) and immediate
assertions with `else` clauses. Immediate assertions without an `else` clause
are not affected.

**Default:** `true`

`BreakAfterAssertionLabel: true` (default):

```sv
// Concurrent assertion
check_valid:
  assert property (@(posedge clk) req |-> ack) else $error("fail");

// Immediate assertion with else
check_valid:
  assert (valid) else $error("invalid");

// Immediate assertion without else (no break)
check_valid: assert (valid);
```

`BreakAfterAssertionLabel: false`:

```sv
check_valid: assert property (@(posedge clk) req |-> ack) else $error("fail");
check_valid: assert (valid) else $error("invalid");
```

---

### BreakAfterBegin (bool)

If `true`, enforce a newline immediately after every `begin` token whose first
item is not already on the next line.

**Default:** `true`

`BreakAfterBegin: false`:

```sv
module foo;
  always_comb begin x = 1; end
endmodule
```

`BreakAfterBegin: true` (default):

```sv
module foo;
  always_comb begin
    x = 1;
  end
endmodule
```

---

### BreakAfterInitial (BlockBreakStyle)

Controls whether slang-format inserts a newline between an `initial` or `final`
header and its body. Accepts the same values as
[`BreakAfterAlways`](#breakafteralways-blockbreakstyle).

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

`OnlyMultiline` (default) - break inserted before a multi-statement body:

```sv
module foo;
  initial begin
    x = 1;
    y = 2;
  end
endmodule
```

---

### BreakBeforeAlways (BlockBreakStyle)

Controls whether slang-format inserts a blank line before an `always`,
`always_comb`, `always_ff`, or `always_latch` block. Comments immediately
preceding the block are kept together with it; the blank line is inserted before
the first attached comment. Accepts the same values as
[`BreakAfterAlways`](#breakafteralways-blockbreakstyle).

**Default:** `Never`

`Always` - blank line always inserted:

```sv
module foo;

  always_comb begin
    x = 1;
  end
endmodule
```

`Never` (default) - no blank line inserted:

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

### BreakBeforeEnd (bool)

If `true`, enforce a newline before every `end` token that is not already on its
own line.

**Default:** `true`

`BreakBeforeEnd: false`:

```sv
module foo;
  always_comb begin x = 1; end
endmodule
```

`BreakBeforeEnd: true` (default):

```sv
module foo;
  always_comb begin
    x = 1;
  end
endmodule
```

---

### BreakBeforeFunction (bool)

If `true`, insert a blank line before `function` declarations. Comments
immediately preceding the declaration are kept together with it; the blank line
is inserted before the first attached comment.

**Default:** `false`

`BreakBeforeFunction: false` (default):

```sv
module foo;
  assign x = 1;
  function void bar;
    x = 1;
  endfunction
endmodule
```

`BreakBeforeFunction: true`:

```sv
module foo;
  assign x = 1;

  function void bar;
    x = 1;
  endfunction
endmodule
```

---

### BreakBeforeInitial (BlockBreakStyle)

Controls whether slang-format inserts a blank line before an `initial` or
`final` block. Comments immediately preceding the block are kept together with
it; the blank line is inserted before the first attached comment. Accepts the
same values as
[`BreakAfterAlways`](#breakafteralways-blockbreakstyle).

**Default:** `Never`

`Always` - blank line always inserted:

```sv
module foo;

  initial begin
    x = 1;
  end
endmodule
```

`Never` (default) - no blank line inserted:

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

**Default:** `false`

`BreakBeforeSpecifyBlock: false` (default):

```sv
module foo;
  assign x = 1;
  specify
    $setup(posedge clk, data, 10);
  endspecify
endmodule
```

`BreakBeforeSpecifyBlock: true`:

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

**Default:** `false`

`BreakBeforeTask: false` (default):

```sv
module foo;
  assign x = 1;
  task bar;
    x = 1;
  endtask
endmodule
```

`BreakBeforeTask: true`:

```sv
module foo;
  assign x = 1;

  task bar;
    x = 1;
  endtask
endmodule
```

---

### ColumnLimit (unsigned)

The column limit. A column limit of `0` means that no column limit is applied.

This option is consumed by
[`AlignAfterOpenParen`](#alignafteropenparen-bracketalignmentstyle) for argument
list wrapping and by
[`CompactControlStatements`](#compactcontrolstatements-bool) for conditional
compaction.

**Default:** `100`

---

### CompactControlStatements (bool)

Compacts bare conditional statement bodies onto the same line as their `if`,
`else if`, or `else` keyword when the resulting line fits within
[`ColumnLimit`](#columnlimit-unsigned). Only applies to bare statement bodies
(not `begin`/`end` blocks). Requires
[`OneStatementPerLine`](#onestatementperline-bool) to be enabled.

`CompactConditionals` is accepted as a configuration alias for this option.

**Default:** `true`

`CompactControlStatements: true` (default):

```sv
always_comb
  if (a) x = 1;
  else if (b) y = 2;
  else z = 3;
```

`CompactControlStatements: false`:

```sv
always_comb
  if (a)
    x = 1;
  else if (b)
    y = 2;
  else
    z = 3;
```

When the compacted line would exceed the column limit, the body falls back to a
new indented line:

```sv
always_comb
  if (some_very_long_condition_expression)
    some_very_long_assignment_target = some_very_long_value;
```

---

### CompactTimeUnits (bool)

Keeps consecutive `timeunit` and `timeprecision` declarations on the same line.
When disabled, each declaration is placed on its own line per
[`OneStatementPerLine`](#onestatementperline-bool).

**Default:** `true`

`CompactTimeUnits: true` (default):

```sv
module foo;
  timeunit 1ns; timeprecision 1ps;
endmodule
```

`CompactTimeUnits: false`:

```sv
module foo;
  timeunit 1ns;
  timeprecision 1ps;
endmodule
```

---

### ContinuationIndentWidth (unsigned)

Number of spaces added for continuation lines - lines that syntactically
continue the previous line (for example, an operand in a multi-line expression)
- relative to the enclosing statement.

If not explicitly set, defaults to `IndentWidth`.

**Default:** Inherits from `IndentWidth`

`ContinuationIndentWidth: 2`:

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

### EventSeparator (EventSeparatorStyle)

Controls the separator used between signals in event expressions (sensitivity
lists). When set to `Comma` or `Or`, slang-format rewrites the separator in
`@(...)` event expressions to use the specified style.

**Default:** `Preserve`

| Value      | Description                         |
| ---------- | ----------------------------------- |
| `Comma`    | Rewrite `or` separators to `,`.     |
| `Or`       | Rewrite `,` separators to `or`.     |
| `Preserve` | Separators are left as-is.          |

`Comma` - `or` keywords rewritten to commas:

```sv
// Before
module foo;
  always_ff @(posedge clk or negedge rst)
    x <= 1;
endmodule

// After
module foo;
  always_ff @(posedge clk, negedge rst)
    x <= 1;
endmodule
```

`Or` - commas rewritten to `or` keywords:

```sv
// Before
module foo;
  always_ff @(posedge clk, negedge rst)
    x <= 1;
endmodule

// After
module foo;
  always_ff @(posedge clk or negedge rst)
    x <= 1;
endmodule
```

`Preserve` (default) - separators are left unchanged:

```sv
module foo;
  always_ff @(posedge clk or negedge rst)
    x <= 1;
endmodule
```

---

### IndentCaseItem (bool)

If `true`, indent statements that follow a `case` label when they appear on the
next line. Has no effect when the statement appears on the same line as the
label or when the label is immediately followed by `begin`.

**Default:** `false`

`IndentCaseItem: false` (default):

```sv
module foo;
  always_comb
    case (x)
      2'b00: y = 0;
      default: y = 1;
    endcase
endmodule
```

`IndentCaseItem: true`:

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
  AlwaysStatements: false
  ControlStatements: false
  InitialStatements: false
```

#### Enabled (bool)

If `false`, disables all insertion regardless of other options.

**Default:** `false`

#### AlwaysStatements (bool)

If `true`, wrap bare statement bodies of `always`, `always_comb`, `always_ff`,
and `always_latch` blocks with `begin`/`end`.

**Default:** `false`

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

**Default:** `false`

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

**Default:** `false`

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

### InsertParens (InsertParensStyle)

Controls whether slang-format inserts parentheses around timing constructs.
Each sub-option independently controls its scope; there is no master switch.

**Default:**

```yaml
InsertParens:
  Delays: false
  ExpressionEvents: false
  ImplicitEvents: false
  NamedEvents: false
```

#### Delays (bool)

If `true`, insert parentheses around bare delay expressions.

**Default:** `false`

```sv
// Before
module foo;
  always #5 clk = ~clk;
endmodule

// After
module foo;
  always #(5) clk = ~clk;
endmodule
```

#### ExpressionEvents (bool)

If `true`, insert parentheses around bare event expressions.

**Default:** `false`

```sv
// Before
module foo;
  always_ff @posedge clk
    x <= 1;
endmodule

// After
module foo;
  always_ff @(posedge clk)
    x <= 1;
endmodule
```

#### ImplicitEvents (bool)

If `true`, insert parentheses around bare implicit event lists.

**Default:** `false`

```sv
// Before
module foo;
  always @* x = 1;
endmodule

// After
module foo;
  always @(*) x = 1;
endmodule
```

#### NamedEvents (bool)

If `true`, insert parentheses around bare named event references.

**Default:** `false`

```sv
// Before
module foo;
  always @signal x = 1;
endmodule

// After
module foo;
  always @(signal) x = 1;
endmodule
```

---

### LineEnding (LineEndingStyle)

Controls the line ending characters used in formatted output. When set to a
derive variant, slang-format counts `\r\n` and bare `\n` occurrences in the
input to determine the dominant style.

**Default:** `DeriveLF`

| Value      | Behavior                                                   |
| ---------- | ---------------------------------------------------------- |
| CRLF       | Always use `\r\n`.                                         |
| DeriveCRLF | Use `\r\n` unless the input has more `\n` than `\r\n`.     |
| DeriveLF   | Use `\n` unless the input has more `\r\n` than `\n`.       |
| LF         | Always use `\n`.                                           |

---

### MaxEmptyLinesToKeep (unsigned)

Maximum number of consecutive empty lines to keep. When `RemoveEmptyLines` is
`true`, slang-format collapses any run of empty lines exceeding this limit.
Empty lines inside a `// slang-format off` region are not affected.

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

### OneStatementPerLine (bool)

Enforces one statement per line. When `true`, consecutive statements that appear
on the same source line are separated by line breaks.

**Default:** `true`

`OneStatementPerLine: true` (default):

```sv
// Before
always_comb begin
  x = 1; y = 2; z = 3;
end

// After
always_comb begin
  x = 1;
  y = 2;
  z = 3;
end
```

`OneStatementPerLine: false`:

```sv
// Before
always_comb begin
  x = 1; y = 2; z = 3;
end

// After
always_comb begin
  x = 1; y = 2; z = 3;
end
```

---

### PackedDimensionBounds (DimensionBoundsStyle)

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

**Default:** Inherits from `ContinuationIndentWidth`

`ParameterPortListIndentWidth: 2`:

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

---

### RangeSpacing (RangeSpacingStyle)

Controls whitespace around the colon in packed dimension range expressions such
as `[7:0]` or `[7 : 0]`.

`MatchSource` preserves the source style (compact or spaced) and normalizes
multi-space ranges to single-space in most contexts. Ranges adjacent to a
parameter port list group with excess alignment spacing are left unchanged,
preserving the author's intended visual alignment.

**Default:** `MatchSource`

| Value         | Description                                                 |
| ------------- | ----------------------------------------------------------- |
| `Always`      | A space is always inserted on each side of the colon.       |
| `MatchSource` | Source style is preserved; multi-space ranges are normalized.|
| `Never`       | Spaces around the colon are always removed.                 |
| `Preserve`    | Source style is preserved; multi-space ranges are clamped.  |

`Always` - spaces are inserted around the colon:

```sv
// Before
module foo;
  logic [7:0] x;
endmodule

// After
module foo;
  logic [7 : 0] x;
endmodule
```

`MatchSource` (default) - source style is preserved:

```sv
module foo;
  logic [7:0] x;
  logic [15 : 0] y;
endmodule
```

`Never` - spaces around the colon are removed:

```sv
// Before
module foo;
  logic [7 : 0] x;
endmodule

// After
module foo;
  logic [7:0] x;
endmodule
```

`Preserve` - source style is preserved, multi-space ranges clamped to
single-space:

```sv
module foo;
  logic [7:0] x;
  logic [15 : 0] y;
endmodule
```

---

### RemoveEmptyLines (bool)

Enables enforcement of `MaxEmptyLinesToKeep`. When `true`, slang-format
collapses any run of consecutive empty lines exceeding the limit. When `false`,
empty lines in the source are preserved as-is. Empty lines inside a
`// slang-format off` region are never affected.

**Default:** `false`

`RemoveEmptyLines: false` (default) - all empty lines preserved:

```sv
module foo;
  assign x = 1;



  assign y = 2;
endmodule
```

`RemoveEmptyLines: true` (with `MaxEmptyLinesToKeep: 1`) - three empty lines
collapsed to one:

```sv
module foo;
  assign x = 1;

  assign y = 2;
endmodule
```

---

### SpaceAfterAlways (bool)

Normalizes whitespace after an always keyword (`always`, `always_comb`,
`always_ff`, `always_latch`). If `true`, a single space is inserted after the
keyword; multiple spaces are collapsed to a single space. If `false`, whitespace
after the keyword is removed. Newlines are not affected.

**Default:** `true`

`SpaceAfterAlways: false`:

```sv
module foo;
  always_ff@(posedge clk)
    q <= d;
endmodule
```

`SpaceAfterAlways: true` (default):

```sv
module foo;
  always_ff @(posedge clk)
    q <= d;
endmodule
```

---

### SpaceAfterBrackets (SpaceAfterBracketsStyle)

Controls whitespace after a closing square bracket in declarations. Each
sub-option independently controls its scope. Brackets in expressions are not
affected.

**Default:**

```yaml
SpaceAfterBrackets:
  PackedDimensions: true
  UnpackedDimensions: false
```

#### PackedDimensions (bool)

If `true`, a single space is inserted after `]` in packed dimensions; multiple
spaces are collapsed to a single space. If `false`, whitespace after `]` is
removed. Newlines are not affected.

**Default:** `true`

`PackedDimensions: false`:

```sv
module foo;
  logic [7:0]data;
endmodule
```

`PackedDimensions: true` (default):

```sv
module foo;
  logic [7:0] data;
endmodule
```

#### UnpackedDimensions (bool)

If `true`, a single space is inserted after `]` in unpacked dimensions; multiple
spaces are collapsed to a single space. If `false`, whitespace after `]` is
removed. Newlines are not affected.

**Default:** `false`

`UnpackedDimensions: false` (default):

```sv
module foo;
  logic data[0:255];
endmodule
```

`UnpackedDimensions: true`:

```sv
module foo;
  logic data[0:255] [0:127];
endmodule
```

---

### SpaceAfterCaseColon (bool)

Normalizes whitespace after a case item colon. If `true`, a single space is
inserted after `:`; multiple spaces are collapsed to a single space. If `false`,
whitespace after `:` is removed. Newlines are not affected.

**Default:** `true`

`SpaceAfterCaseColon: false`:

```sv
module foo;
  always_comb
    case (x)
      2'b00:y = 0;
      default:y = 1;
    endcase
endmodule
```

`SpaceAfterCaseColon: true` (default):

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

### SpaceAfterComma (bool)

Normalizes whitespace around commas. Whitespace preceding a comma is removed.
If `true`, a single space is inserted after the comma; multiple spaces following
a comma are collapsed to a single space. If `false`, whitespace following a
comma is removed. Newlines are not affected.

**Default:** `true`

`SpaceAfterComma: false`:

```sv
module foo;
  assign {a,b,c} = d;
endmodule
```

`SpaceAfterComma: true` (default):

```sv
module foo;
  assign {a, b, c} = d;
endmodule
```

---

### SpaceAfterParens (SpaceAfterParensStyle)

Controls whitespace after a closing parenthesis. Each sub-option independently
controls its scope.

**Default:**

```yaml
SpaceAfterParens:
  ControlStatements: true
  EventControls: true
```

#### ControlStatements (bool)

If `true`, a single space is inserted between a control statement closing
parenthesis and the following token (`if`, `case`, `casex`, `casez`, `for`,
`while`, `repeat`, `foreach`); multiple spaces are collapsed to a single space.
If `false`, whitespace after the closing parenthesis is removed. Newlines are not
affected. The `do`-`while` statement is excluded because its closing parenthesis
is followed by a semicolon.

**Default:** `true`

`ControlStatements: false`:

```sv
module foo;
  always_comb
    if (a)x = 1;
endmodule
```

`ControlStatements: true` (default):

```sv
module foo;
  always_comb
    if (a) x = 1;
endmodule
```

#### EventControls (bool)

If `true`, a single space is inserted between an event control closing
parenthesis and the following token (`@(posedge clk)`, `@(*)`, etc.); multiple
spaces are collapsed to a single space. If `false`, whitespace after the closing
parenthesis is removed. Newlines are not affected. Event controls without
parentheses (`@*`) are not affected.

**Default:** `true`

`EventControls: false`:

```sv
module foo;
  always_ff @(posedge clk)begin
  end
endmodule
```

`EventControls: true` (default):

```sv
module foo;
  always_ff @(posedge clk) begin
  end
endmodule
```

---

### SpaceAfterSemicolon (bool)

Normalizes whitespace around semicolons. Whitespace preceding a semicolon is
removed. If `true`, a single space is inserted after the semicolon; multiple
spaces following a semicolon are collapsed to a single space. If `false`,
whitespace following a semicolon is removed. Newlines are not affected.

**Default:** `true`

`SpaceAfterSemicolon: false`:

```sv
module foo;
  initial begin
    for (int i = 0;i < 4;i++) begin
    end
  end
endmodule
```

`SpaceAfterSemicolon: true` (default):

```sv
module foo;
  initial begin
    for (int i = 0; i < 4; i++) begin
    end
  end
endmodule
```

---

### SpaceAroundOperators (bool)

Normalizes whitespace around assignment, binary, and ternary operators. If
`true`, a single space is inserted before and after the operator; multiple spaces
are collapsed to a single space. If `false`, whitespace before and after the
operator is removed. Newlines are not affected.

**Default:** `true`

`SpaceAroundOperators: false`:

```sv
module foo;
  assign x=a+b;
  assign y=a?b:c;
endmodule
```

`SpaceAroundOperators: true` (default):

```sv
module foo;
  assign x = a + b;
  assign y = a ? b : c;
endmodule
```

---

### SpaceBeforeBrackets (SpaceBeforeBracketsStyle)

Controls whitespace before an opening square bracket in declarations. Each
sub-option independently controls its scope. Brackets in expressions are not
affected.

**Default:**

```yaml
SpaceBeforeBrackets:
  PackedDimensions: true
  UnpackedDimensions: false
```

#### PackedDimensions (bool)

If `true`, a single space is inserted before `[` in packed dimensions; multiple
spaces are collapsed to a single space. If `false`, whitespace before `[` is
removed. Newlines are not affected.

**Default:** `true`

`PackedDimensions: false`:

```sv
module foo;
  logic[7:0] data;
endmodule
```

`PackedDimensions: true` (default):

```sv
module foo;
  logic [7:0] data;
endmodule
```

#### UnpackedDimensions (bool)

If `true`, a single space is inserted before `[` in unpacked dimensions;
multiple spaces are collapsed to a single space. If `false`, whitespace before
`[` is removed. Newlines are not affected.

**Default:** `false`

`UnpackedDimensions: false` (default):

```sv
module foo;
  logic data[0:255];
endmodule
```

`UnpackedDimensions: true`:

```sv
module foo;
  logic data [0:255];
endmodule
```

---

### SpaceBeforeCaseColon (bool)

Normalizes whitespace before a case item colon. If `true`, a single space is
inserted before `:`; multiple spaces are collapsed to a single space. If
`false`, whitespace before `:` is removed. Newlines are not affected.

**Default:** `false`

`SpaceBeforeCaseColon: false` (default):

```sv
module foo;
  always_comb
    case (x)
      2'b00:y = 0;
      default:y = 1;
    endcase
endmodule
```

`SpaceBeforeCaseColon: true`:

```sv
module foo;
  always_comb
    case (x)
      2'b00 :y = 0;
      default :y = 1;
    endcase
endmodule
```

---

### SpaceBeforeParens (SpaceBeforeParensStyle)

Controls whitespace before an opening parenthesis. Each sub-option independently
controls its scope.

**Default:**

```yaml
SpaceBeforeParens:
  ControlStatements: true
  ParameterList: true
  PortList: true
```

#### ControlStatements (bool)

If `true`, a single space is inserted between a control statement keyword (`if`,
`case`, `casex`, `casez`, `for`, `while`, `repeat`, `foreach`, `do`-`while`) and
its opening parenthesis; multiple spaces are collapsed to a single space. If
`false`, whitespace before the parenthesis is removed. Newlines are not affected.

**Default:** `true`

`ControlStatements: false`:

```sv
module foo;
  always_comb
    if(a) x = 1;
endmodule
```

`ControlStatements: true` (default):

```sv
module foo;
  always_comb
    if (a) x = 1;
endmodule
```

#### ParameterList (bool)

If `true`, a single space is inserted before a parameter port list `#(`;
multiple spaces are collapsed to a single space. If `false`, whitespace before
`#(` is removed. Newlines are not affected.

**Default:** `true`

`ParameterList: false`:

```sv
module foo#(
    parameter N = 4
)(
    input a
);
endmodule
```

`ParameterList: true` (default):

```sv
module foo #(
    parameter N = 4
) (
    input a
);
endmodule
```

#### PortList (bool)

If `true`, a single space is inserted before a port list `(`; multiple spaces are
collapsed to a single space. If `false`, whitespace before `(` is removed.
Newlines are not affected.

**Default:** `true`

`PortList: false`:

```sv
module foo(
    input a
);
endmodule
```

`PortList: true` (default):

```sv
module foo (
    input a
);
endmodule
```

---

### SpacesBeforeTrailingComments (unsigned)

Minimum number of spaces before a trailing end-of-line comment. Both `//` and
`/* */` comments that follow code on the same line are affected. Standalone
comment lines are not affected.

**Default:** `2`

`SpacesBeforeTrailingComments: 1`:

```sv
module foo;
  assign x = 1; // comment
endmodule
```

`SpacesBeforeTrailingComments: 2` (default):

```sv
module foo;
  assign x = 1;  // comment
endmodule
```

---

### SpacesInBraces (bool)

Normalizes whitespace inside curly braces. If `true`, whitespace following an
opening brace is replaced with a single space and a single space is inserted
before a closing brace; multiple spaces are collapsed to a single space. If
`false`, whitespace following an opening brace and preceding a closing brace is
removed. Newlines are not affected. Both `{` and `'{` tokens are treated as
opening braces.

**Default:** `false`

`SpacesInBraces: false` (default):

```sv
module foo;
  assign x = {a, b, c};
endmodule
```

`SpacesInBraces: true`:

```sv
module foo;
  assign x = { a, b, c };
endmodule
```

---

### SpacesInBrackets (bool)

Normalizes whitespace inside square brackets in declarations. If `true`,
whitespace following an opening bracket is replaced with a single space and a
single space is inserted before a closing bracket; multiple spaces are collapsed
to a single space. If `false`, whitespace following an opening bracket and
preceding a closing bracket is removed. Newlines are not affected. Brackets in
expressions are not affected.

**Default:** `false`

`SpacesInBrackets: false` (default):

```sv
module foo;
  logic [7:0] data;
  logic data [0:255];
endmodule
```

`SpacesInBrackets: true`:

```sv
module foo;
  logic [ 7:0 ] data;
  logic data [ 0:255 ];
endmodule
```

---

### SpacesInParens (bool)

Normalizes whitespace inside parentheses. If `true`, whitespace following an
opening parenthesis is replaced with a single space and a single space is
inserted before a closing parenthesis; multiple spaces are collapsed to a single
space. If `false`, whitespace following an opening parenthesis and preceding a
closing parenthesis is removed. Newlines are not affected.

**Default:** `false`

`SpacesInParens: false` (default):

```sv
module foo;
  initial begin
    if (a) begin
    end
  end
endmodule
```

`SpacesInParens: true`:

```sv
module foo;
  initial begin
    if ( a ) begin
    end
  end
endmodule
```

---

### UnpackedDimensionBounds (DimensionBoundsStyle)

Controls the ordering of bounds in unpacked dimension ranges. When set to
`MSBFirst` or `LSBFirst`, slang-format swaps the left and right bounds of
unpacked dimension ranges so that the larger or smaller value appears first,
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
  logic x [0:7];
endmodule

// After
module foo;
  logic x [7:0];
endmodule
```

`LSBFirst` - smaller value placed on the left:

```sv
// Before
module foo;
  logic x [7:0];
endmodule

// After
module foo;
  logic x [0:7];
endmodule
```

`Preserve` (default) - bounds are left unchanged:

```sv
module foo;
  logic x [0:7];
endmodule
```

[1]: https://github.com/chipsalliance/verible
