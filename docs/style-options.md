# Style Options                                                 {#style-options}

@tableofcontents

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

A configuration file may start from a predefined style by specifying
[BasedOnStyle](#basedonstyle). Any options that follow override predefined
values:

```yaml
BasedOnStyle: Default
IndentWidth: 4
```

@note Code examples presented in this document are taken with (TODO permission)
from IEEE Std 1800-2023, *IEEE Standard for SystemVerilog--Unified Hardware
Design, Specification, and Verification Language* ([SystemVerilog LRM][2]).

## Disabling Formatting

slang-format understands special comments that suspend formatting in a delimited
range. Use `// slang-format off` to disable and `// slang-format on` to
re-enable. Block comment forms (`/* slang-format off */` and `/* slang-format on */`)
are also supported.

For compatibility with [Verible][2], the `// verilog_format: off` and
`// verilog_format: on` pragmas (and their block comment equivalents) are also
recognized.

```sv
module memMod(simple_bus a);
// slang-format off
logic avail;                                       // unformatted
   // slang-format on
   always @(posedge a.clk) a.gnt <= a.req & avail; // formatted
endmodule
```

`OneLineFormatOffRegex` offers a complementary mechanism: it suppresses
formatting on lines matching a regular expression, without requiring inline
comments. See [OneLineFormatOffRegex](#onelineformatoffregex) for details.

## Configurable Options

### AlignAfterOpenParen

Type: [ParenAlignmentStyle](@ref slang::format::ParenAlignmentStyle)

Controls alignment and wrapping of arguments when a parenthesized argument list
exceeds [ColumnLimit](#columnlimit). When wrapping is triggered, this option
determines how continuation lines are indented. All constructs with argument
lists are affected, including function calls, system task calls, method calls,
elaboration system tasks, and class constructors. This option has no effect when
`ColumnLimit` is `0`.

Possible values:

- `Align` Continuation lines align to the column after the opening parenthesis.

  ```sv
  $display("%0d %0d",
           C#()::f(),
           C#(5)::f());
  ```

- `AlwaysBreak` Break after the opening parenthesis; continuation lines use
  [ContinuationIndentWidth](#continuationindentwidth).

  ```sv
  $display(
     "%0d %0d",
     C#()::f(),
     C#(5)::f());
  ```

- `BlockIndent` Like `AlwaysBreak`, but the closing parenthesis is placed on its
  own line at the primary indentation.

  ```sv
  $display(
     "%0d %0d",
     C#()::f(),
     C#(5)::f()
  );
  ```

- `DontAlign` No active wrapping is performed; existing breaks use
  [ContinuationIndentWidth](#continuationindentwidth).

  ```sv
  // Existing source breaks are preserved.
  $display("%0d %0d", C#()::f(), C#(5)::f());
  ```

### AlignConsecutiveAssignments

Type: [AlignConsecutiveStyle](@ref slang::format::AlignConsecutiveStyle)

Controls alignment of assignment operators in consecutive assignments. When
enabled, slang-format aligns assignment operators in adjacent lines by padding
with trailing whitespace. Alignment groups are scoped; boundaries such as
`module`/`endmodule` and `always`/`end` break groups, while control statements
at the same depth (`if`/`else`, `case`/`endcase`) does not.

Example configuration:

```yaml
AlignConsecutiveAssignments:
  Enabled: true
```

Example output:

```sv
TODO
```

Nested style options:

- `bool Enabled` If `true`, enables alignment behavior detailed below.

- `bool AcrossComments` If `true`, comments do not break alignment groups.

- `bool AcrossEmptyLines` If `true`, empty lines do not break alignment groups.

- `bool AcrossParameterPortList` If `true`, port list boundaries do not break
  alignment groups.

- `bool InferFromSource` If `true`, the formatter examines original source
  spacing to infer intended alignment. Groups where alignment falls within
  `MinPadding` are always aligned regardless of this setting.

- `unsigned MaxPadding` Maximum number of spaces any line in a group may be
  padded to reach alignment. If the required padding for any line in a group
  exceeds this limit, the group is left unaligned. A value of `0` means
  unlimited; all groups will be aligned regardless of padding.

  ```sv
  // MaxPadding: 2
  module m1 (a,b);
     real r1,r2;
     parameter [2:0] A = 3'h2;
     parameter B = 3'h2;       // unaligned
     ...
  endmodule
  ```

  ```sv
  // MaxPadding: 0
  module m1 (a,b);
     real r1,r2;
     parameter [2:0] A = 3'h2;
     parameter B       = 3'h2; // aligned
     ...
  endmodule
  ```

- `unsigned MinPadding` Minimum number of spaces any line in a group may be
  padded to reach alignment. If the required padding for all lines in a group is
  at or below this threshold, the group is always aligned, bypassing source
  inference. A value of `0` disables alignment. This option has no effect when
  `InferFromSource` is `false`.

  ```sv
  // MinPadding: 0
  module my_mem (addr, data);
     parameter addr_width = 16;
     localparam mem_size = 1 << addr_width;  // unaligned
     parameter data_width = 8;
     ...
  endmodule
  ```

  ```sv
  // MinPadding: 2
  module my_mem (addr, data);
     parameter addr_width = 16;
     localparam mem_size  = 1 << addr_width; // aligned
     parameter data_width = 8;
     ...
  endmodule
  ```

### AlignConsecutiveDeclarations

Type: [AlignConsecutiveStyle](@ref slang::format::AlignConsecutiveStyle)

Controls alignment of signal names in consecutive declarations. When enabled,
identifiers are aligned in adjacent declarations by padding the type specifier
with trailing spaces. Continuation lines of comma-separated declaration lists
are included in alignment groups. Non-declaration statements always break
alignment groups.

Example configuration:

```yaml
AlignConsecutiveDeclarations:
  Enabled: true
```

Example output:

```sv
// Enabled: false
time a;         // unaligned
integer b;      // unaligned
logic [31:0] c;
```

```sv
// Enabled: true
time         a; // aligned
integer      b; // aligned
logic [31:0] c;
```

Nested style options:

See [AlignConsecutiveAssignments](#alignconsecutiveassignments) for common
alignment options.

### AlignConsecutivePackedDimensions

Type: [AlignConsecutiveStyle](@ref slang::format::AlignConsecutiveStyle)

Controls alignment of packed dimensions in consecutive declarations. When
enabled, the opening bracket `[` of packed dimensions in
adjacent declarations are aligned by padding the type specifier with trailing spaces. Only
declarations that have packed dimensions participate in alignment groups;
declarations without packed dimensions are skipped.

Example configuration:

```yaml
AlignConsecutivePackedDimensions:
  Enabled: true
```

Example output:

```sv
TODO
```

Nested style options:

- `bool AlignColon` If `true`, right-justify the left-side value of packed
  dimension ranges to align `:` separators across a group.

  ```sv
  // AlignColon: false
  bit [7:0] VPI;  // unaligned
  bit [11:0] VCI;
  ```

  ```sv
  // AlignColon: true
  bit [ 7:0] VPI; // aligned
  bit [11:0] VCI;
  ```

- `bool PadLeft` If `true`, left-pad content within brackets to right-justify
  values. Takes precedence over `PadRight` when both are set.

  ```sv
  // PadLeft: true
  bit [ 7:0] VPI;
  bit [11:0] VCI;
  ```

- `bool PadRight` If `true`, right-pad content within brackets to left-justify
  values. `PadLeft` takes precedence when both are `true`.

  ```sv
  // PadRight: true
  bit [7:0 ] VPI;
  bit [11:0] VCI;
  ```

See [AlignConsecutiveAssignments](#alignconsecutiveassignments) for common
alignment options.

### AlignConsecutivePortConnections

Type: [AlignConsecutiveStyle](@ref slang::format::AlignConsecutiveStyle)

Controls alignment of opening parentheses in consecutive named port connections
and parameter assignments. When enabled, the `(` in adjacent `.name(expr)` lines
are aligned by padding with trailing spaces after the port name. Alignment
groups are scoped, and port list boundaries naturally separate parameter port
groups from connection port groups.

Example configuration:

```yaml
AlignConsecutivePortConnections:
  Enabled: true
```

Example output:

```sv
// Enabled: false
accum accum (
   .dataout(dataout[7:0]),
   .datain(alu_out),       // unaligned
   .clk(clk)               // unaligned
);
```

```sv
// Enabled: true
accum accum (
   .dataout(dataout[7:0]),
   .datain (alu_out),      // aligned
   .clk    (clk)           // aligned
);
```

Nested style options:

See [AlignConsecutiveAssignments](#alignconsecutiveassignments) for common
alignment options.

### AlignConsecutiveTimingControls

Type: [AlignConsecutiveStyle](@ref slang::format::AlignConsecutiveStyle)

Controls alignment of signal names and assignment operators in consecutive
timing controls. When enabled, signal names and assignment operators in adjacent
statements with delay expressions are aligned by padding with trailing spaces.

Example configuration:

```yaml
AlignConsecutiveTimingControls:
  Enabled: true
```

Example output:

```sv
// Enabled: false
#d rega = regb;             // unaligned
#((d+e)/2) rega = regb;
#regr regr = regr + 1;      // unaligned
```

```sv
// Enabled: true
#d         rega = regb;     // aligned
#((d+e)/2) rega = regb;
#regr      regr = regr + 1; // aligned
```

Nested style options:

See [AlignConsecutiveAssignments](#alignconsecutiveassignments) for common
alignment options.

### AlignTrailingComments

Type: [AlignConsecutiveStyle](@ref slang::format::AlignConsecutiveStyle)

Controls alignment of trailing comments in consecutive lines. When enabled,
trailing comments (`//` and `/* */`) on adjacent lines are aligned by padding
with spaces. Only comments that appear after code on the same line are
considered; standalone comment lines are not affected.

Example configuration:

```yaml
AlignTrailingComments:
  Enabled: true
```

Example output:
```sv
TODO
```

Nested style options:

See [AlignConsecutiveAssignments](#alignconsecutiveassignments) for common
alignment options.

### AllowShortConstraintBlocksOnASingleLine

Type: `bool`

If `true`, constraint blocks with a single item are joined onto one line when the
result fits within [ColumnLimit](#columnlimit). Multi-item constraint
blocks are always expanded regardless of this setting. Applies to both named
constraints and `randomize() with` blocks.

Possible values:

- `true` Single-item constraint blocks are joined onto one line.

  ```sv
  class packet;
    constraint c {kind inside {[0:7]};}
  endclass
  ```

- `false` Constraint blocks are always expanded.

  ```sv
  class packet;
    constraint c {
      kind inside {[0:7]};
    }
  endclass
  ```

### AllowShortCoverpointsOnASingleLine

Type: `bool`

If `true`, coverpoint and cover cross bodies are joined onto one line when the
result fits within [ColumnLimit](#columnlimit). Multi-item bodies that
exceed the column limit are always expanded regardless of this setting.

Possible values:

- `true` Coverpoint and cover cross bodies are joined onto one line.

  ```sv
  covergroup cg;
    cp_addr: coverpoint addr {option.auto_bin_max = 8;}
  endgroup
  ```

- `false` Coverpoint and cover cross bodies are always expanded.

  ```sv
  covergroup cg;
    cp_addr: coverpoint addr {
      option.auto_bin_max = 8;
    }
  endgroup
  ```

### BasedOnStyle

Type: `string`

The style used for all options not specifically set in the configuration file.
The named style is applied first; every other option in the file then overrides
it. Style names are matched case-insensitively. An unknown style name is
reported as a configuration error.

This option is accepted only in configuration files and is not emitted by
`--dump-config`.

```yaml
BasedOnStyle: Default
IndentWidth: 4
```

### BinPackArguments

Type: `bool`

Controls whether arguments are bin-packed or placed one per line when wrapping
is triggered by [AlignAfterOpenParen](#alignafteropenparen).
When `true`, the formatter packs as many arguments as fit within
[ColumnLimit](#columnlimit) on each line. When `false`, each argument
is placed on its own line.

This option has no effect when `AlignAfterOpenParen` is `DontAlign` or when
`ColumnLimit` is `0`.

Possible values:

- `true` Arguments are packed onto as few lines as possible.

  ```sv
  $display("alpha", bravo, charlie,
           delta, echo_val);
  ```

- `false` Each argument is placed on its own line.

  ```sv
  $display("alpha",
           bravo,
           charlie,
           delta,
           echo_val);
  ```

### BinPackParameters

Type: `bool`

Controls whether function and task port parameters are bin-packed or placed one
per line when wrapping is triggered by
[AlignAfterOpenParen](#alignafteropenparen). When `true`,
the formatter packs as many parameters as fit within
[ColumnLimit](#columnlimit) on each line. When `false`, each parameter
is placed on its own line.

This option has no effect when `AlignAfterOpenParen` is `DontAlign` or when
`ColumnLimit` is `0`.

Possible values:

- `true` Parameters are packed onto as few lines as possible.

  ```sv
  function automatic logic [DW-1:0] mask_bytes(
      input logic [DW-1:0] value, input logic [STRB_W-1:0] strb,
      input bit invert = 1'b0);
  endfunction
  ```

- `false` Each parameter is placed on its own line.

  ```sv
  function automatic logic [DW-1:0] mask_bytes(
      input logic [DW-1:0] value,
      input logic [STRB_W-1:0] strb,
      input bit invert = 1'b0
  );
  endfunction
  ```

### BinPackPortConnections

Type: `bool`

Controls whether instance port connections are bin-packed or placed one per line.
When `true`, named port connections in module instantiations are packed on as few
lines as possible. When `false`, each port connection is placed on its own line.

Possible values:

- `false` Each port connection is placed on its own line.

  ```sv
  dff u_dff (
      .clk(clk),
      .rst_n(rst_n),
      .d(data_i),
      .q(dff_q)
  );
  ```

- `true` Port connections are packed onto as few lines as possible.

  ```sv
  dff u_dff (
      .clk(clk), .rst_n(rst_n), .d(data_i), .q(dff_q)
  );
  ```

### BreakAfterAlways

Type: [BlockBreakStyle](@ref slang::format::BlockBreakStyle)

Controls whether slang-format inserts a newline between an `always`,
`always_comb`, `always_ff`, or `always_latch` header and its body.

Possible values:

- `Always` Always insert a newline between the header and its body.

  ```sv
  module foo;
    always_comb
      x = y;
  endmodule
  ```

- `Never` Never insert a newline; body follows the header on the same line.

  ```sv
  module foo;
    always_comb begin
      x = 1;
      y = 2;
    end
  endmodule
  ```

- `OnlyMultiline` Insert a newline only when the body spans multiple source
  lines.

  ```sv
  module foo;
    always_comb if (a) x = 1;
  endmodule
  ```

### BreakAfterAssertionLabel

Type: `bool`

If `true`, insert a newline after assertion labels on concurrent assertions
(`assert property`, `assume property`, `cover property`, etc.) and immediate
assertions with `else` clauses. Immediate assertions without an `else` clause
are not affected.

Possible values:

- `true` A newline is inserted after assertion labels.

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

- `false` Assertion labels remain on the same line.

  ```sv
  check_valid: assert property (@(posedge clk) req |-> ack) else $error("fail");
  check_valid: assert (valid) else $error("invalid");
  ```

### BreakAfterBegin

Type: `bool`

If `true`, enforce a newline immediately after every `begin` token whose first
item is not already on the next line.

Possible values:

- `false` No newline is enforced after `begin`.

  ```sv
  module foo;
    always_comb begin x = 1; end
  endmodule
  ```

- `true` A newline is enforced after `begin`.

  ```sv
  module foo;
    always_comb begin
      x = 1;
    end
  endmodule
  ```

### BreakAfterInitial

Type: [BlockBreakStyle](@ref slang::format::BlockBreakStyle)

Controls whether slang-format inserts a newline between an `initial` or `final`
header and its body.

Possible values:

- `Always` Always insert a newline between the header and its body.

  ```sv
  module foo;
    initial begin
      x = 1;
    end
  endmodule
  ```

- `Never` Never insert a newline; body follows the header on the same line.

  ```sv
  module foo;
    initial begin x = 1; y = 2; end
  endmodule
  ```

- `OnlyMultiline` Insert a newline only when the body spans multiple source
  lines.

  ```sv
  module foo;
    initial begin
      x = 1;
      y = 2;
    end
  endmodule
  ```

### BreakBeforeAlways

Type: [BlockBreakStyle](@ref slang::format::BlockBreakStyle)

Controls whether slang-format inserts a blank line before an `always`,
`always_comb`, `always_ff`, or `always_latch` block. Comments immediately
preceding the block are kept together with it; the blank line is inserted before
the first attached comment.

Possible values:

- `Always` Always insert a blank line before the block.

  ```sv
  module foo;

    always_comb begin
      x = 1;
    end
  endmodule
  ```

- `Never` Never insert a blank line.

  ```sv
  module foo;
    always_comb begin
      x = 1;
      y = 2;
    end
  endmodule
  ```

- `OnlyMultiline` Insert a blank line only when the body spans multiple source
  lines.

  ```sv
  module foo;

    always_comb begin
      x = 1;
      y = 2;
    end
  endmodule
  ```

### BreakBeforeEnd

Type: `bool`

If `true`, enforce a newline before every `end` token that is not already on its
own line.

Possible values:

- `false` No newline is enforced before `end`.

  ```sv
  module foo;
    always_comb begin x = 1; end
  endmodule
  ```

- `true` A newline is enforced before `end`.

  ```sv
  module foo;
    always_comb begin
      x = 1;
    end
  endmodule
  ```

### BreakBeforeFunction

Type: `bool`

If `true`, insert a blank line before `function` declarations. Comments
immediately preceding the declaration are kept together with it; the blank line
is inserted before the first attached comment.

Possible values:

- `false` No blank line is inserted before `function` declarations.

  ```sv
  module foo;
    assign x = 1;
    function void bar;
      x = 1;
    endfunction
  endmodule
  ```

- `true` A blank line is inserted before `function` declarations.

  ```sv
  module foo;
    assign x = 1;

    function void bar;
      x = 1;
    endfunction
  endmodule
  ```

### BreakBeforeInitial

Type: [BlockBreakStyle](@ref slang::format::BlockBreakStyle)

Controls whether slang-format inserts a blank line before an `initial` or
`final` block. Comments immediately preceding the block are kept together with
it; the blank line is inserted before the first attached comment.

Possible values:

- `Always` Always insert a blank line before the block.

  ```sv
  module foo;

    initial begin
      x = 1;
    end
  endmodule
  ```

- `Never` Never insert a blank line.

  ```sv
  module foo;
    initial begin
      x = 1;
      y = 2;
    end
  endmodule
  ```

- `OnlyMultiline` Insert a blank line only when the body spans multiple source
  lines.

  ```sv
  module foo;

    initial begin
      x = 1;
      y = 2;
    end
  endmodule
  ```

### BreakBeforeSpecifyBlock

Type: `bool`

If `true`, insert a blank line before `specify` blocks. Comments immediately
preceding the block are kept together with it; the blank line is inserted before
the first attached comment.

Possible values:

- `false` No blank line is inserted before `specify` blocks.

  ```sv
  module foo;
    assign x = 1;
    specify
      $setup(posedge clk, data, 10);
    endspecify
  endmodule
  ```

- `true` A blank line is inserted before `specify` blocks.

  ```sv
  module foo;
    assign x = 1;

    specify
      $setup(posedge clk, data, 10);
    endspecify
  endmodule
  ```

### BreakBeforeTask

Type: `bool`

If `true`, insert a blank line before `task` declarations. Comments immediately
preceding the declaration are kept together with it; the blank line is inserted
before the first attached comment.

Possible values:

- `false` No blank line is inserted before `task` declarations.

  ```sv
  module foo;
    assign x = 1;
    task bar;
      x = 1;
    endtask
  endmodule
  ```

- `true` A blank line is inserted before `task` declarations.

  ```sv
  module foo;
    assign x = 1;

    task bar;
      x = 1;
    endtask
  endmodule
  ```

### ColumnLimit

Type: `unsigned`

The column limit. A column limit of `0` means that no column limit is applied.

This option is consumed by
[AlignAfterOpenParen](#alignafteropenparen) for argument
list wrapping and by
[CompactControlStatements](#compactcontrolstatements) for conditional
compaction.

### CompactControlStatements

Type: `bool`

Compacts bare conditional statement bodies onto the same line as their `if`,
`else if`, or `else` keyword when the resulting line fits within
[ColumnLimit](#columnlimit). Only applies to bare statement bodies
(not `begin`/`end` blocks). Requires
[OneStatementPerLine](#onestatementperline) to be enabled.

`CompactConditionals` is accepted as a configuration alias for this option.

Possible values:

- `true` Bare conditional statement bodies are compacted onto one line.

  ```sv
  always_comb
    if (a) x = 1;
    else if (b) y = 2;
    else z = 3;
  ```

- `false` Bare conditional statement bodies are placed on separate lines.

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

### CompactTimeUnits

Type: `bool`

Keeps consecutive `timeunit` and `timeprecision` declarations on the same line.
When disabled, each declaration is placed on its own line per
[OneStatementPerLine](#onestatementperline).

Possible values:

- `true` Consecutive time declarations are kept on the same line.

  ```sv
  module foo;
    timeunit 1ns; timeprecision 1ps;
  endmodule
  ```

- `false` Each time declaration is placed on its own line.

  ```sv
  module foo;
    timeunit 1ns;
    timeprecision 1ps;
  endmodule
  ```

### ContinuationIndentWidth

Type: `unsigned`

Number of spaces added for continuation lines - lines that syntactically
continue the previous line (for example, an operand in a multi-line expression)
- relative to the enclosing statement.

If not explicitly set, defaults to `IndentWidth`.

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

### EventSeparator

Type: [EventSeparatorStyle](@ref slang::format::EventSeparatorStyle)

Controls the separator used between signals in event expressions (sensitivity
lists). When set to `Comma` or `Or`, slang-format rewrites the separator in
`@(...)` event expressions to use the specified style.

Possible values:

- `Comma` Rewrite `or` separators to `,`.

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

- `Or` Rewrite `,` separators to `or`.

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

- `Preserve` Separators are left unchanged.

  ```sv
  module foo;
    always_ff @(posedge clk or negedge rst)
      x <= 1;
  endmodule
  ```

### IndentCaseItem

Type: `bool`

If `true`, indent statements that follow a `case` label when they appear on the
next line. Has no effect when the statement appears on the same line as the
label or when the label is immediately followed by `begin`.

Possible values:

- `false` Statements after a `case` label are not indented.

  ```sv
  module foo;
    always_comb
      case (x)
        2'b00: y = 0;
        default: y = 1;
      endcase
  endmodule
  ```

- `true` Statements after a `case` label are indented.

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

### IndentWidth

Type: `unsigned`

Number of spaces for each indentation level.

`IndentWidth: 2`:

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

### InsertBeginEnd

Type: [InsertBeginEndStyle](@ref slang::format::InsertBeginEndStyle)

Controls whether slang-format inserts `begin`/`end` blocks around bare
single-statement bodies. `Enabled` is a master switch; the remaining sub-options
take effect only when `Enabled` is `true`.

#### Enabled

Type: `bool`

If `false`, disables all insertion regardless of other options.

#### AlwaysStatements

Type: `bool`

If `true`, wrap bare statement bodies of `always`, `always_comb`, `always_ff`,
and `always_latch` blocks with `begin`/`end`.

Possible values:

- `true` Bare statement bodies are wrapped with `begin`/`end`.

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

#### ControlStatements

Type: `bool`

If `true`, wrap bare single-statement bodies of control constructs - `if`,
`else`, `for`, `while`, `foreach`, `repeat`, `forever`, `do`-`while`, and `case`
items - with `begin`/`end`.

Possible values:

- `true` Bare single-statement bodies are wrapped with `begin`/`end`.

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

#### InitialStatements

Type: `bool`

If `true`, wrap bare statement bodies of `initial` and `final` blocks with
`begin`/`end`.

Possible values:

- `true` Bare statement bodies are wrapped with `begin`/`end`.

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

### InsertParens

Type: [InsertParensStyle](@ref slang::format::InsertParensStyle)

Controls whether slang-format inserts parentheses around timing constructs.
Each sub-option independently controls its scope; there is no master switch.

#### Delays

Type: `bool`

If `true`, insert parentheses around bare delay expressions.

Possible values:

- `true` Parentheses are inserted around bare delay expressions.

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

#### ExpressionEvents

Type: `bool`

If `true`, insert parentheses around bare event expressions.

Possible values:

- `true` Parentheses are inserted around bare event expressions.

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

#### ImplicitEvents

Type: `bool`

If `true`, insert parentheses around bare implicit event lists.

Possible values:

- `true` Parentheses are inserted around bare implicit event lists.

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

#### NamedEvents

Type: `bool`

If `true`, insert parentheses around bare named event references.

Possible values:

- `true` Parentheses are inserted around bare named event references.

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

### LineEnding

Type: [LineEndingStyle](@ref slang::format::LineEndingStyle)

Controls the line ending characters used in formatted output. When set to a
derive variant, slang-format counts `\r\n` and bare `\n` occurrences in the
input to determine the dominant style.

Possible values:

- `CRLF` Always use `\r\n`.
- `DeriveCRLF` Use `\r\n` unless the input has more `\n` than `\r\n`.
- `DeriveLF` Use `\n` unless the input has more `\r\n` than `\n`.
- `LF` Always use `\n`.

### MaxEmptyLinesToKeep

Type: `unsigned`

Maximum number of consecutive empty lines to keep. When `RemoveEmptyLines` is
`true`, slang-format collapses any run of empty lines exceeding this limit.
Empty lines inside a `// slang-format off` region are not affected.

`MaxEmptyLinesToKeep: 1` - two empty lines collapsed to one:

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

### OneLineFormatOffRegex

Type: `string`

Disables formatting for any line whose entire content matches this regular
expression.

An empty string disables this feature.

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

### OneStatementPerLine

Type: `bool`

Enforces one statement per line. When `true`, consecutive statements that appear
on the same source line are separated by line breaks.

Possible values:

- `true` Consecutive statements on the same line are separated by line breaks.

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

- `false` Consecutive statements on the same line are left as-is.

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

### PackedDimensionBounds

Type: [DimensionBoundsStyle](@ref slang::format::DimensionBoundsStyle)

Controls the ordering of bounds in packed dimension ranges. When set to
`MSBFirst` or `LSBFirst`, slang-format swaps the left and right bounds of
packed dimension ranges so that the larger or smaller value appears first,
respectively. Only dimensions where both bounds are integer literals are
affected; dimensions with non-literal expressions (e.g. parameters, binary
expressions) are left unchanged.

Possible values:

- `LSBFirst` The smaller value appears as the left bound.

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

- `MSBFirst` The larger value appears as the left bound.

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

- `Preserve` Bounds are left as-is.

  ```sv
  module foo;
    logic [0:7] x;
  endmodule
  ```

### ParameterPortListIndentWidth

Type: `unsigned`

Number of spaces used to indent items in a module ANSI port declaration. When
the module has a parameter port list (`#( ... )`), this value governs both the
parameter list and the following port list. When only a port list is present,
`IndentWidth` applies instead.

If not explicitly set, defaults to `ContinuationIndentWidth`.

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

### RangeSpacing

Type: [RangeSpacingStyle](@ref slang::format::RangeSpacingStyle)

Controls whitespace around the colon in packed dimension range expressions such
as `[7:0]` or `[7 : 0]`.

`MatchSource` preserves the source style (compact or spaced) and normalizes
multi-space ranges to single-space in most contexts. Ranges adjacent to a
parameter port list group with excess alignment spacing are left unchanged,
preserving the author's intended visual alignment.

Possible values:

- `Always` A space is always inserted on each side of the colon.

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

- `MatchSource` Source style is preserved; multi-space ranges are normalized.

  ```sv
  module foo;
    logic [7:0] x;
    logic [15 : 0] y;
  endmodule
  ```

- `Never` Spaces around the colon are always removed.

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

- `Preserve` Source style is preserved; multi-space ranges are clamped.

  ```sv
  module foo;
    logic [7:0] x;
    logic [15 : 0] y;
  endmodule
  ```

### RemoveEmptyLines

Type: `bool`

Enables enforcement of `MaxEmptyLinesToKeep`. When `true`, slang-format
collapses any run of consecutive empty lines exceeding the limit. When `false`,
empty lines in the source are preserved as-is. Empty lines inside a
`// slang-format off` region are never affected.

Possible values:

- `false` All empty lines are preserved.

  ```sv
  module foo;
    assign x = 1;

    assign y = 2;
  endmodule
  ```

- `true` Empty lines exceeding `MaxEmptyLinesToKeep` are collapsed.

  ```sv
  // MaxEmptyLinesToKeep: 1 - three empty lines collapsed to one
  module foo;
    assign x = 1;

    assign y = 2;
  endmodule
  ```

### SpaceAfterAlways

Type: `bool`

Normalizes whitespace after an always keyword (`always`, `always_comb`,
`always_ff`, `always_latch`). If `true`, a single space is inserted after the
keyword; multiple spaces are collapsed to a single space. If `false`, whitespace
after the keyword is removed. Newlines are not affected.

Possible values:

- `false` Whitespace after the keyword is removed.

  ```sv
  module foo;
    always_ff@(posedge clk)
      q <= d;
  endmodule
  ```

- `true` A single space is inserted after the keyword.

  ```sv
  module foo;
    always_ff @(posedge clk)
      q <= d;
  endmodule
  ```

### SpaceAfterBrackets

Type: [SpaceAfterBracketsStyle](@ref slang::format::SpaceAfterBracketsStyle)

Controls whitespace after a closing square bracket in declarations. Each
sub-option independently controls its scope. Brackets in expressions are not
affected.

#### PackedDimensions

Type: `bool`

If `true`, a single space is inserted after `]` in packed dimensions; multiple
spaces are collapsed to a single space. If `false`, whitespace after `]` is
removed. Newlines are not affected.

Possible values:

- `false` Whitespace after `]` is removed.

  ```sv
  module foo;
    logic [7:0]data;
  endmodule
  ```

- `true` A single space is inserted after `]`.

  ```sv
  module foo;
    logic [7:0] data;
  endmodule
  ```

#### UnpackedDimensions

Type: `bool`

If `true`, a single space is inserted after `]` in unpacked dimensions; multiple
spaces are collapsed to a single space. If `false`, whitespace after `]` is
removed. Newlines are not affected.

Possible values:

- `false` Whitespace after `]` is removed.

  ```sv
  module foo;
    logic data[0:255];
  endmodule
  ```

- `true` A single space is inserted after `]`.

  ```sv
  module foo;
    logic data[0:255] [0:127];
  endmodule
  ```

### SpaceAfterCaseColon

Type: `bool`

Normalizes whitespace after a case item colon. If `true`, a single space is
inserted after `:`; multiple spaces are collapsed to a single space. If `false`,
whitespace after `:` is removed. Newlines are not affected.

Possible values:

- `false` Whitespace after `:` is removed.

  ```sv
  module foo;
    always_comb
      case (x)
        2'b00:y = 0;
        default:y = 1;
      endcase
  endmodule
  ```

- `true` A single space is inserted after `:`.

  ```sv
  module foo;
    always_comb
      case (x)
        2'b00: y = 0;
        default: y = 1;
      endcase
  endmodule
  ```

### SpaceAfterComma

Type: `bool`

Normalizes whitespace around commas. Whitespace preceding a comma is removed.
If `true`, a single space is inserted after the comma; multiple spaces following
a comma are collapsed to a single space. If `false`, whitespace following a
comma is removed. Newlines are not affected.

Possible values:

- `false` Whitespace following a comma is removed.

  ```sv
  module foo;
    assign {a,b,c} = d;
  endmodule
  ```

- `true` A single space is inserted after commas.

  ```sv
  module foo;
    assign {a, b, c} = d;
  endmodule
  ```

### SpaceAfterParens

Type: [SpaceAfterParensStyle](@ref slang::format::SpaceAfterParensStyle)

Controls whitespace after a closing parenthesis. Each sub-option independently
controls its scope.

#### ControlStatements

Type: `bool`

If `true`, a single space is inserted between a control statement closing
parenthesis and the following token (`if`, `case`, `casex`, `casez`, `for`,
`while`, `repeat`, `foreach`); multiple spaces are collapsed to a single space.
If `false`, whitespace after the closing parenthesis is removed. Newlines are not
affected. The `do`-`while` statement is excluded because its closing parenthesis
is followed by a semicolon.

Possible values:

- `false` Whitespace after the closing parenthesis is removed.

  ```sv
  module foo;
    always_comb
      if (a)x = 1;
  endmodule
  ```

- `true` A single space is inserted after the closing parenthesis.

  ```sv
  module foo;
    always_comb
      if (a) x = 1;
  endmodule
  ```

#### EventControls

Type: `bool`

If `true`, a single space is inserted between an event control closing
parenthesis and the following token (`@(posedge clk)`, `@(*)`, etc.); multiple
spaces are collapsed to a single space. If `false`, whitespace after the closing
parenthesis is removed. Newlines are not affected. Event controls without
parentheses (`@*`) are not affected.

Possible values:

- `false` Whitespace after the closing parenthesis is removed.

  ```sv
  module foo;
    always_ff @(posedge clk)begin
    end
  endmodule
  ```

- `true` A single space is inserted after the closing parenthesis.

  ```sv
  module foo;
    always_ff @(posedge clk) begin
    end
  endmodule
  ```

### SpaceAfterSemicolon

Type: `bool`

Normalizes whitespace around semicolons. Whitespace preceding a semicolon is
removed. If `true`, a single space is inserted after the semicolon; multiple
spaces following a semicolon are collapsed to a single space. If `false`,
whitespace following a semicolon is removed. Newlines are not affected.

Possible values:

- `false` Whitespace following a semicolon is removed.

  ```sv
  module foo;
    initial begin
      for (int i = 0;i < 4;i++) begin
      end
    end
  endmodule
  ```

- `true` A single space is inserted after semicolons.

  ```sv
  module foo;
    initial begin
      for (int i = 0; i < 4; i++) begin
      end
    end
  endmodule
  ```

### SpaceAroundOperators

Type: `bool`

Normalizes whitespace around assignment, binary, and ternary operators. If
`true`, a single space is inserted before and after the operator; multiple spaces
are collapsed to a single space. If `false`, whitespace before and after the
operator is removed. Newlines are not affected.

Possible values:

- `false` Whitespace around operators is removed.

  ```sv
  module foo;
    assign x=a+b;
    assign y=a?b:c;
  endmodule
  ```

- `true` A single space is inserted around operators.

  ```sv
  module foo;
    assign x = a + b;
    assign y = a ? b : c;
  endmodule
  ```

### SpaceBeforeBrackets

Type: [SpaceBeforeBracketsStyle](@ref slang::format::SpaceBeforeBracketsStyle)

Controls whitespace before an opening square bracket in declarations. Each
sub-option independently controls its scope. Brackets in expressions are not
affected.

#### PackedDimensions

Type: `bool`

If `true`, a single space is inserted before `[` in packed dimensions; multiple
spaces are collapsed to a single space. If `false`, whitespace before `[` is
removed. Newlines are not affected.

Possible values:

- `false` Whitespace before `[` is removed.

  ```sv
  module foo;
    logic[7:0] data;
  endmodule
  ```

- `true` A single space is inserted before `[`.

  ```sv
  module foo;
    logic [7:0] data;
  endmodule
  ```

#### UnpackedDimensions

Type: `bool`

If `true`, a single space is inserted before `[` in unpacked dimensions;
multiple spaces are collapsed to a single space. If `false`, whitespace before
`[` is removed. Newlines are not affected.

Possible values:

- `false` Whitespace before `[` is removed.

  ```sv
  module foo;
    logic data[0:255];
  endmodule
  ```

- `true` A single space is inserted before `[`.

  ```sv
  module foo;
    logic data [0:255];
  endmodule
  ```

### SpaceBeforeCaseColon

Type: `bool`

Normalizes whitespace before a case item colon. If `true`, a single space is
inserted before `:`; multiple spaces are collapsed to a single space. If
`false`, whitespace before `:` is removed. Newlines are not affected.

Possible values:

- `false` Whitespace before `:` is removed.

  ```sv
  module foo;
    always_comb
      case (x)
        2'b00:y = 0;
        default:y = 1;
      endcase
  endmodule
  ```

- `true` A single space is inserted before `:`.

  ```sv
  module foo;
    always_comb
      case (x)
        2'b00 :y = 0;
        default :y = 1;
      endcase
  endmodule
  ```

### SpaceBeforeParens

Type: [SpaceBeforeParensStyle](@ref slang::format::SpaceBeforeParensStyle)

Controls whitespace before an opening parenthesis. Each sub-option independently
controls its scope.

#### ControlStatements

Type: `bool`

If `true`, a single space is inserted between a control statement keyword (`if`,
`case`, `casex`, `casez`, `for`, `while`, `repeat`, `foreach`, `do`-`while`) and
its opening parenthesis; multiple spaces are collapsed to a single space. If
`false`, whitespace before the parenthesis is removed. Newlines are not affected.

Possible values:

- `false` Whitespace before the parenthesis is removed.

  ```sv
  module foo;
    always_comb
      if(a) x = 1;
  endmodule
  ```

- `true` A single space is inserted before the parenthesis.

  ```sv
  module foo;
    always_comb
      if (a) x = 1;
  endmodule
  ```

#### ParameterList

Type: `bool`

If `true`, a single space is inserted before a parameter port list `#(`;
multiple spaces are collapsed to a single space. If `false`, whitespace before
`#(` is removed. Newlines are not affected.

Possible values:

- `false` Whitespace before `#(` is removed.

  ```sv
  module foo#(
      parameter N = 4
  )(
      input a
  );
  endmodule
  ```

- `true` A single space is inserted before `#(`.

  ```sv
  module foo #(
      parameter N = 4
  ) (
      input a
  );
  endmodule
  ```

#### PortList

Type: `bool`

If `true`, a single space is inserted before a port list `(`; multiple spaces are
collapsed to a single space. If `false`, whitespace before `(` is removed.
Newlines are not affected.

Possible values:

- `false` Whitespace before `(` is removed.

  ```sv
  module foo(
      input a
  );
  endmodule
  ```

- `true` A single space is inserted before `(`.

  ```sv
  module foo (
      input a
  );
  endmodule
  ```

### SpacesBeforeTrailingComments

Type: `unsigned`

Minimum number of spaces before a trailing end-of-line comment. Both `//` and
`/* */` comments that follow code on the same line are affected. Standalone
comment lines are not affected.

`SpacesBeforeTrailingComments: 1`:

```sv
module foo;
  assign x = 1; // comment
endmodule
```

`SpacesBeforeTrailingComments: 2`:

```sv
module foo;
  assign x = 1;  // comment
endmodule
```

### SpacesInBraces

Type: `bool`

Normalizes whitespace inside curly braces. If `true`, whitespace following an
opening brace is replaced with a single space and a single space is inserted
before a closing brace; multiple spaces are collapsed to a single space. If
`false`, whitespace following an opening brace and preceding a closing brace is
removed. Newlines are not affected. Both `{` and `'{` tokens are treated as
opening braces.

Possible values:

- `false` Whitespace inside braces is removed.

  ```sv
  module foo;
    assign x = {a, b, c};
  endmodule
  ```

- `true` A single space is inserted inside braces.

  ```sv
  module foo;
    assign x = { a, b, c };
  endmodule
  ```

### SpacesInBrackets

Type: `bool`

Normalizes whitespace inside square brackets in declarations. If `true`,
whitespace following an opening bracket is replaced with a single space and a
single space is inserted before a closing bracket; multiple spaces are collapsed
to a single space. If `false`, whitespace following an opening bracket and
preceding a closing bracket is removed. Newlines are not affected. Brackets in
expressions are not affected.

Possible values:

- `false` Whitespace inside brackets is removed.

  ```sv
  module foo;
    logic [7:0] data;
    logic data [0:255];
  endmodule
  ```

- `true` A single space is inserted inside brackets.

  ```sv
  module foo;
    logic [ 7:0 ] data;
    logic data [ 0:255 ];
  endmodule
  ```

### SpacesInParens

Type: `bool`

Normalizes whitespace inside parentheses. If `true`, whitespace following an
opening parenthesis is replaced with a single space and a single space is
inserted before a closing parenthesis; multiple spaces are collapsed to a single
space. If `false`, whitespace following an opening parenthesis and preceding a
closing parenthesis is removed. Newlines are not affected.

Possible values:

- `false` Whitespace inside parentheses is removed.

  ```sv
  module foo;
    initial begin
      if (a) begin
      end
    end
  endmodule
  ```

- `true` A single space is inserted inside parentheses.

  ```sv
  module foo;
    initial begin
      if ( a ) begin
      end
    end
  endmodule
  ```

### UnpackedDimensionBounds

Type: [DimensionBoundsStyle](@ref slang::format::DimensionBoundsStyle)

Controls the ordering of bounds in unpacked dimension ranges. When set to
`MSBFirst` or `LSBFirst`, slang-format swaps the left and right bounds of
unpacked dimension ranges so that the larger or smaller value appears first,
respectively. Only dimensions where both bounds are integer literals are
affected; dimensions with non-literal expressions (e.g. parameters, binary
expressions) are left unchanged.

Possible values:

- `LSBFirst` The smaller value appears as the left bound.

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

- `MSBFirst` The larger value appears as the left bound.

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

- `Preserve` Bounds are left as-is.

  ```sv
  module foo;
    logic x [0:7];
  endmodule
  ```

[1]: https://ieeexplore.ieee.org/document/10458102
[2]: https://chipsalliance.github.io/verible/
