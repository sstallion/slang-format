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

@note Code examples presented in this document are taken with permission
(request pending) from [IEEE Std 1800-2023][2], *IEEE Standard for
SystemVerilog--Unified Hardware Design, Specification, and Verification Language*.

## Disabling Formatting

slang-format understands special comments that suspend formatting in a delimited
range. Use `// slang-format off` to disable and `// slang-format on` to
re-enable. Block comment forms (`/* slang-format off */` and `/* slang-format on */`)
are also supported.

For compatibility with [Verible][2], the `// verilog_format: off` and
`// verilog_format: on` pragmas (and their block comment equivalents) are also
recognized.

```sv
module memMod (simple_bus a);
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
  // AlignAfterOpenParen: Align
  $display("%0d %0d",
           C#()::f(),
           C#(5)::f());
  ```

- `AlwaysBreak` Break after the opening parenthesis; continuation lines use
  [ContinuationIndentWidth](#continuationindentwidth).

  ```sv
  // AlignAfterOpenParen: AlwaysBreak
  $display(
     "%0d %0d",
     C#()::f(),
     C#(5)::f());
  ```

- `BlockIndent` Like `AlwaysBreak`, but the closing parenthesis is placed on its
  own line at the primary indentation.

  ```sv
  // AlignAfterOpenParen: BlockIndent
  $display(
     "%0d %0d",
     C#()::f(),
     C#(5)::f()
  );
  ```

- `DontAlign` No active wrapping is performed; existing breaks use
  [ContinuationIndentWidth](#continuationindentwidth).

  ```sv
  // AlignAfterOpenParen: DontAlign
  $display("%0d %0d", C#()::f(),
     C#(5)::f());
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
  AcrossComments: true
  AcrossEmptyLines: true
  AcrossParameterPortList: true
  InferFromSource: false
  MaxPadding: 0
  MinPadding: 0
```

Example output:

```sv
a   = blue * 3;    // aligned
col = yellow;
b   = col + green; // aligned
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
  AcrossComments: true
  AcrossEmptyLines: true
  AcrossParameterPortList: true
  InferFromSource: false
  MaxPadding: 0
  MinPadding: 0
```

Example output:

```sv
time         a; // aligned
integer      b; // aligned
logic [31:0] c;
```

Nested style options:

See [AlignConsecutiveAssignments](#alignconsecutiveassignments) for shared
nested style options.

### AlignConsecutivePackedDimensions

Type: [AlignConsecutiveStyle](@ref slang::format::AlignConsecutiveStyle)

Controls alignment of packed dimensions in consecutive declarations. When
enabled, the opening bracket `[` of packed dimensions in adjacent declarations
are aligned by padding the type specifier with trailing spaces. Only
declarations that have packed dimensions participate in alignment groups;
declarations without packed dimensions are skipped.

Example configuration:

```yaml
AlignConsecutivePackedDimensions:
  Enabled: true
  AcrossComments: true
  AcrossEmptyLines: true
  AcrossParameterPortList: true
  InferFromSource: false
  MaxPadding: 0
  MinPadding: 0
  AlignColon: true
  PadLeft: true
  PadRight: false
```

Example output:

```sv
logic        [7:0] regA; // aligned
logic signed [7:0] regS;
```

Nested style options:

- `bool AlignColon` If `true`, right-justify the left-side value of packed
  dimension ranges to align `:` separators across a group.

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

See [AlignConsecutiveAssignments](#alignconsecutiveassignments) for shared
nested style options.

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
  AcrossComments: true
  AcrossEmptyLines: true
  AcrossParameterPortList: true
  InferFromSource: false
  MaxPadding: 0
  MinPadding: 0
```

Example output:

```sv
accum accum (
   .dataout(dataout[7:0]),
   .datain (alu_out),      // aligned
   .clk    (clk)           // aligned
);
```

Nested style options:

See [AlignConsecutiveAssignments](#alignconsecutiveassignments) for shared
nested style options.

### AlignConsecutiveTimingControls

Type: [AlignConsecutiveStyle](@ref slang::format::AlignConsecutiveStyle)

Controls alignment of signal names and assignment operators in consecutive
timing controls. When enabled, signal names and assignment operators in adjacent
statements with delay expressions are aligned by padding with trailing spaces.

Example configuration:

```yaml
AlignConsecutiveTimingControls:
  Enabled: true
  AcrossComments: true
  AcrossEmptyLines: true
  AcrossParameterPortList: true
  InferFromSource: false
  MaxPadding: 0
  MinPadding: 0
```

Example output:

```sv
#d         rega = regb;     // aligned
#((d+e)/2) rega = regb;
#regr      regr = regr + 1; // aligned
```

Nested style options:

See [AlignConsecutiveAssignments](#alignconsecutiveassignments) for shared
nested style options.

### AlignTrailingComments

Type: [AlignConsecutiveStyle](@ref slang::format::AlignConsecutiveStyle)

Controls alignment of trailing comments in consecutive lines. When enabled,
trailing comments (`//` and `/* */`) on adjacent lines are aligned by padding
with spaces. Only comments that appear after code on the same line are
considered; standalone comment lines are not affected.

Example configuration:

```yaml
AlignTrailingComments:
  AcrossComments: true
  AcrossEmptyLines: true
  AcrossParameterPortList: true
  InferFromSource: false
  MaxPadding: 0
  MinPadding: 0
```

Example output:

```sv
regA = unsigned'(-4);    // regA = 8'b11111100
regS = signed'(4'b1100); // regS = -4
```

Nested style options:

See [AlignConsecutiveAssignments](#alignconsecutiveassignments) for shared
nested style options.

### AllowShortConstraintBlocksOnASingleLine

Type: `bool`

If `true`, constraint blocks with a single item are joined onto one line when
the result fits within [ColumnLimit](#columnlimit). Multi-item constraint blocks
are always expanded. This option also applies to `randomize() with` blocks.

Example output:

```sv
constraint c {(a == 0) -> (b == 1);}
```

### AllowShortCoverpointsOnASingleLine

Type: `bool`

If `true`, coverpoint and cover cross bodies are joined onto one line when the
result fits within [ColumnLimit](#columnlimit). Multi-item bodies are always
expanded.

Example output:

```sv
covergroup cg;
   cp_addr: coverpoint addr {option.auto_bin_max = 8;}
endgroup
```

### BasedOnStyle

Type: `string`

The style used for all options not specifically set in the configuration file.
The named style is applied first; every other option in the file then overrides
it. Style names are matched case-insensitively. An unknown style name is
reported as a configuration error.

```yaml
BasedOnStyle: Default
IndentWidth: 4
```

### BinPackArguments

Type: `bool`

If `true`, pack as many arguments as fit within [ColumnLimit](#columnlimit) on
each line when wrapping is triggered by [AlignAfterOpenParen](#alignafteropenparen).
This option has no effect when `AlignAfterOpenParen` is `DontAlign` or when
`ColumnLimit` is `0`.

Example output:

```sv
$display("ID = %s, W = %d, D = %d",
         ID, W, D);
```

### BinPackParameters

Type: `bool`

If `true`, pack as many function and task port parameters as fit within
[ColumnLimit](#columnlimit) on each line when wrapping is triggered by
[AlignAfterOpenParen](#alignafteropenparen). This option has no effect when
`AlignAfterOpenParen` is `DontAlign` or when `ColumnLimit` is `0`.

Example output:

```sv
function logic [15:0] myfunc3(int a, int b,
                              output logic [15:0] u,
                              v);
  ...
endfunction
```

### BinPackPortConnections

Type: `bool`

If `true`, pack named port connections in module instantiations on as few lines
as possible.

Example output:

```sv
dff u_dff (.clk(clk), .rst_n(rst_n), .d(data_i), .q(dff_q));
```

### BreakAfterAlways

Type: [BlockBreakStyle](@ref slang::format::BlockBreakStyle)

Controls whether slang-format inserts a newline between an `always`,
`always_comb`, `always_ff`, or `always_latch` keyword and its statement.

Possible values:

- `Always` Always insert a newline between the keyword and its statement.

  ```sv
  // BreakAfterAlways: Always
  always_comb
     a = b & c;
  ```

- `Never` Never insert a newline; keyword follows the statement on the same
  line.

  ```sv
  // BreakAfterAlways: Never
  always_comb a = b & c;
  ```

- `OnlyMultiline` Insert a newline only when the statement spans multiple lines.

  ```sv
  // BreakAfterAlways: OnlyMultiline
  always_comb
     unique case (1'b1)
        a : z = b;
        not_a : z = c;
     endcase
  ```

### BreakAfterAssertionLabel

Type: `bool`

If `true`, insert a newline after assertion labels on concurrent assertions and
immediate assertions with `else` clauses. Immediate assertions without an `else`
clause are not affected.

Example output:

```sv
a1_bad:
   assert property (@clk a == b)
```

### BreakAfterBegin

Type: `bool`

If `true`, enforce a newline immediately after every `begin` keyword whose first
item is not already on the next line.

Example output:

```sv
always_comb begin
   if (sel) y = a;
   else y = b;
end
```

### BreakAfterInitial

Type: [BlockBreakStyle](@ref slang::format::BlockBreakStyle)

Controls whether a newline is inserted between an `initial` or `final` keyword
and its statement.

Possible values:

See [BreakAfterAlways](#breakafteralways) for shared possible values.

### BreakBeforeAlways

Type: [BlockBreakStyle](@ref slang::format::BlockBreakStyle)

Controls whether a newline is inserted before an `always`, `always_comb`,
`always_ff`, or `always_latch` procedure.

Possible values:

See [BreakAfterAlways](#breakafteralways) for shared possible values.

### BreakBeforeEnd

Type: `bool`

If `true`, insert a newline before each `end` keyword that is not already on its
own line.

Example output:

```sv
always_comb begin
   if (sel) y = a;
   else y = b;
end
```

### BreakBeforeFunction

Type: `bool`

If `true`, insert a newline before `function` declarations.

Example output:

```sv
function void post_randomize;
   crc = payload.sum;
endfunction
```

### BreakBeforeInitial

Type: [BlockBreakStyle](@ref slang::format::BlockBreakStyle)

Controls whether slang-format inserts a newline before an `initial` or `final`
procedure.

Possible values:

See [BreakAfterAlways](#breakafteralways) for shared possible values.

### BreakBeforeSpecifyBlock

Type: `bool`

If `true`, insert a newline before `specify` blocks.

Example output:

```sv
always @(posedge clk)
   Q = DAT;

specify
   $setup (DAT, posedge CLK, 10);
endspecify
```

### BreakBeforeTask

Type: `bool`

If `true`, insert a newline before `task` declarations.

Example output:

```sv
event done, blast;
event done_too = done;

task trigger (event ev);
   -> ev;
endtask
```

### ColumnLimit

Type: `unsigned`

The column limit. A value of `0` means that no column limit is applied.

### CompactControlStatements

Type: `bool`

If `true`, compact conditional statements onto the same line as the keyword when
the resulting line fits within [ColumnLimit](#columnlimit). Only applies to
simple statements (i.e. those without `begin`/`end` blocks). Requires
[OneStatementPerLine](#onestatementperline) to be enabled.

Example output:

```sv
if (!clear) assign q = 0;
else if (!preset) assign q = 1;
else deassign q;
```

### CompactTimeUnits

Type: `bool`

If `true`, place consecutive `timeunit` and `timeprecision` declarations on the
same line.

Example output:

```sv
module D (...);
  timeunit 100ps; timeprecision 10fs;
  ...
endmodule
```

### ContinuationIndentWidth

Type: `unsigned`

Number of spaces added for continuation lines relative to the enclosing
statement. If not set, defaults to `IndentWidth`.

### EventSeparator

Type: [EventSeparatorStyle](@ref slang::format::EventSeparatorStyle)

Controls the separator used between signals in event expressions lists.

Possible values:

- `Comma` Rewrite `or` separators to `,`.

  ```sv
  // EventSeparator: Comma
  always @(a, b, c, d, e)
  ```

- `Or` Rewrite `,` separators to `or`.

  ```sv
  // EventSeparator: Or
  always @(a or b or c or d or e)
  ```

- `Preserve` Separators are left unchanged.

### IndentCaseItem

Type: `bool`

If `true`, indent statements that follow a `case` label when they appear on the
next line. Has no effect when the statement appears on the same line as the
label or when the label is immediately followed by `begin`.

Example output:

```sv
unique case (1'b1)
   a:
      z = b;
   not_a:
      z = c;
endcase
```

### IndentWidth

Type: `unsigned`

Number of spaces for each indentation level.

### InsertBeginEnd

Type: [InsertBeginEndStyle](@ref slang::format::InsertBeginEndStyle)

Controls whether `begin`/`end` keywords are inserted around single statements.

Example configuration:

```yaml
InsertBeginEnd:
  Enabled: true
  AlwaysStatements: true
  ControlStatements: true
  InitialStatements: true
```

Example output:

```sv
if (!clear) begin
   assign q = 0;
end else if (!preset) begin
   assign q = 1;
end else begin
   deassign q;
end
```

Nested style options:

- `bool Enabled` If `true`, enables insertion behavior detailed below.

- `bool AlwaysStatements` If `true`, insert `begin`/`end` keywords around
  `always` statements.

- `bool ControlStatements` If `true`, insert `begin`/`end` keywords around
  control statements.

- `bool InitialStatements` If `true`, insert `begin`/`end` keywords around
  `initial` and `final` statements.

### InsertParens

Type: [InsertParensStyle](@ref slang::format::InsertParensStyle)

Controls whether parentheses are inserted around various constructs.

Example configuration:

```yaml
InsertParens:
  Delays: true
  ExpressionEvents: true
  ImplicitEvents: true
  NamedEvents: true
```

Nested style options:

- `bool Delays` If `true`, insert parentheses around delay expressions.

  ```sv
  // Delays: true
  #(10) inputs = 'b011001;
  ```

- `bool ExpressionEvents` If `true`, insert parentheses around event
  expressions.

  ```sv
  // ExpressionEvents: true
  always @(posedge clk, negedge rstn) begin
     ...
  end
  ```

- `bool ImplicitEvents` If `true`, insert parenthesis around implicit events.

  ```sv
  // ImplicitEvents: true
  always @(*) x = a ^ b;
  ```

- `bool NamedEvents` If `true`, insert parentheses around named events.

  ```sv
  // NamedEvents: true
  @(done_too);
  ```

### LineEnding

Type: [LineEndingStyle](@ref slang::format::LineEndingStyle)

Controls the line ending characters used in formatted output. When set to a
derive variant, newlines are counted to determine the most common style.

Possible values:

- `CRLF` Always use `\r\n`.

- `DeriveCRLF` Use `\r\n` unless the input has more `\n` than `\r\n`.

- `DeriveLF` Use `\n` unless the input has more `\r\n` than `\n`.

- `LF` Always use `\n`.

### MaxEmptyLinesToKeep

Type: `unsigned`

Maximum number of consecutive empty lines to keep. When `RemoveEmptyLines` is
`true`, empty lines exceeding this limit will be removed. Empty lines inside a
`// slang-format off` region are not affected.

### OneLineFormatOffRegex

Type: `string`

Disables formatting for any line whose content matches this regular expression.
An empty string disables this feature.

### OneStatementPerLine

Type: `bool`

If `true`, separate consecutive statements that appear on the same source line
with newlines.

### PackedDimensionBounds

Type: [DimensionBoundsStyle](@ref slang::format::DimensionBoundsStyle)

Controls the ordering of bounds in packed dimension ranges. When set to
`MSBFirst` or `LSBFirst`, left and right bounds of packed dimension ranges are
swapped so that the larger or smaller value appears first, respectively. Only
dimensions where both bounds are integer literals are affected; dimensions with
non-literal expressions (e.g. parameters, binary expressions) are left unchanged.

Possible values:

- `LSBFirst` The smaller value appears as the left bound.

  ```sv
  // LSBFirst: true
  bit [0:7] VPI;
  bit [0:11] VCI;
  ```

- `MSBFirst` The larger value appears as the left bound.

  ```sv
  // MSBFirst: true
  bit [7:0] VPI;
  bit [11:0] VCI;
  ```

- `Preserve` Bounds are left as-is.

### ParameterPortListIndentWidth

Type: `unsigned`

Number of spaces used to indent items in a module port declaration. When the
module has a parameter port list, this value governs both the parameter list and
the following port list. If not set, defaults to `ContinuationIndentWidth`.

Example configuration:

```yaml
IndentWidth: 2
ParameterPortListIndentWidth: 2
```

Example output:

```sv
module test #(
  N = 1
) (
  input [N-1:0] in,
  output [N-1:0] out
);
  ...
endmodule
```

### RangeSpacing

Type: [RangeSpacingStyle](@ref slang::format::RangeSpacingStyle)

Controls whitespace around the colon in packed dimension range expressions.

Possible values:

- `Always` A space is always inserted on each side of the colon.

  ```sv
  // RangeSpacing: Always
  bit [0 : 7] VPI;
  bit [0 : 11] VCI;
  ```

- `Never` Spaces around the colon are always removed.

  ```sv
  // RangeSpacing: Never
  bit [0:7] VPI;
  bit [0:11] VCI;
  ```

- `MatchSource` Multiple spaces are normalized to a single space.

- `Preserve` Source style is preserved.

### RemoveEmptyLines

Type: `bool`

If `true`, remove consecutive empty lines exceeding
[MaxEmptyLinesToKeep](#maxemptylinestokeep). Empty lines inside a
`// slang-format off` region are not affected.

### SpaceAfterAlways

Type: `bool`

If `true`, insert a single space after an always keyword (`always`, `always_comb`,
`always_ff`, `always_latch`). Multiple spaces are normalized to a single space.
Newlines are not affected.

Example output:

```sv
always @(posedge clk)
   Q = DAT;
```

### SpaceAfterBrackets

Type: [SpaceAfterBracketsStyle](@ref slang::format::SpaceAfterBracketsStyle)

Controls whitespace after a closing square bracket in declarations. Brackets in
expressions are not affected. This option can be combined with
[SpaceBeforeBrackets](#spacebeforebrackets) to control whitespace on both sides.

Example configuration:

```yaml
SpaceAfterBrackets:
  PackedDimensions: true
  UnpackedDimensions: true
```

Nested style options:

- `bool PackedDimensions` If `true`, insert a single space after `]` in packed
  dimensions; multiple spaces are normalized to a single space.

  ```sv
  // PackedDimensions: true
  bit[7:0] [31:0] v7[1:5][1:10], v8[0:255];
  ```

- `bool UnpackedDimensions` If `true`, insert a single space after `]` in
  unpacked dimensions; multiple spaces are normalized to a single space.

  ```sv
  // UnpackedDimensions: true
  bit[7:0][31:0]v7[1:5] [1:10], v8[0:255];
  ```

### SpaceAfterCaseColon

Type: `bool`

If `true`, insert a single space after a case item colon; multiple spaces are
normalized to a single space. Newlines are not affected. This option can be
combined with [SpaceBeforeCaseColon](#spacebeforecasecolon) to control
whitespace on both sides.

Example output:

```sv
unique case (1'b1)
   a: z = b;
   not_a: z = c;
endcase
```

### SpaceAfterComma

Type: `bool`

If `true`, insert a single space after commas; multiple spaces are normalized to
a single space. Whitespace preceding a comma is always removed. Newlines are not
affected.

Example output:

```sv
always @(a, b, c, d, e)
```

### SpaceAfterParens

Type: [SpaceAfterParensStyle](@ref slang::format::SpaceAfterParensStyle)

Controls whitespace after a closing parenthesis. This option can be combined
with [SpaceBeforeParens](#spacebeforeparens) to control whitespace on both
sides.

Example configuration:

```yaml
SpaceAfterParens:
  ControlStatements: true
  EventControls: true
```

Nested style options:

- `bool ControlStatements` If `true`, insert a single space between a control
  statement closing parenthesis and the following token; multiple spaces are
  normalized to a single space. Newlines are not affected.

  ```sv
  // ControlStatements: true
  if(!clear) assign q = 0;
  else if(!preset) assign q = 1;
  else deassign q;
  ```

- `bool EventControls` If `true`, insert a single space between an event control
  closing parenthesis and the following token; multiple spaces are normalized to
  a single space. Newlines are not affected. Event controls without parentheses
  are not affected.

  ```sv
  // EventControls: true
  always @(posedge clk, negedge rstn) begin
     ...
  end
  ```

### SpaceAfterSemicolon

Type: `bool`

If `true`, insert a single space after semicolons; multiple spaces are
normalized to a single space. Whitespace preceding a semicolon is always
removed. Newlines are not affected.

Example output:

```sv
for (clogb2 = 0; value > 0; clogb2 = clogb2 + 1)
  value = value >> 1;
```

### SpaceAroundOperators

Type: `bool`

If `true`, insert a single space before and after assignment, binary, and
ternary operators; multiple spaces are normalized to a single space. Newlines
are not affected.

Example output:

```sv
result = flaga ? 'x : 0;
```

### SpaceBeforeBrackets

Type: [SpaceBeforeBracketsStyle](@ref slang::format::SpaceBeforeBracketsStyle)

Controls whitespace before an opening square bracket in declarations. Brackets
in expressions are not affected. This option can be combined with
[SpaceAfterBrackets](#spaceafterbrackets) to control whitespace on both sides.

Example configuration:

```yaml
SpaceBeforeBrackets:
  PackedDimensions: true
  UnpackedDimensions: true
```

Nested style options:

- `bool PackedDimensions` If `true`, insert a single space before `[` in packed
  dimensions; multiple spaces are normalized to a single space.

  ```sv
  // PackedDimensions: true
  bit [7:0] [31:0] v7[1:5][1:10], v8 [0:255];
  ```

- `bool UnpackedDimensions` If `true`, insert a single space before `[` in
  unpacked dimensions; multiple spaces are normalized to a single space.

  ```sv
  // UnpackedDimensions: true
  bit[7:0][31:0]v7 [1:5] [1:10], v8[0:255];
  ```

### SpaceBeforeCaseColon

Type: `bool`

If `true`, insert a single space before a case item colon; multiple spaces are
normalized to a single space. Newlines are not affected. This option can be
combined with [SpaceAfterCaseColon](#spaceaftercasecolon) to control whitespace
on both sides.

Example output:

```sv
unique case (1'b1)
   a :z = b;
   not_a :z = c;
endcase
```

### SpaceBeforeParens

Type: [SpaceBeforeParensStyle](@ref slang::format::SpaceBeforeParensStyle)

Controls whitespace before an opening parenthesis. This option can be combined
with [SpaceAfterParens](#spaceafterparens) to control whitespace on both sides.

Example configuration:

```yaml
SpaceBeforeParens:
  ControlStatements: true
  ParameterList: true
  PortList: true
```

Nested style options:

- `bool ControlStatements` If `true`, insert a single space between a control
  statement keyword and its opening parenthesis; multiple spaces are normalized
  to a single space. Newlines are not affected.

  ```sv
  // ControlStatements: true
  if (!clear)assign q = 0;
  else if (!preset)assign q = 1;
  else deassign q;
  ```

- `bool ParameterList` If `true`, insert a single space before a parameter list;
  multiple spaces are normalized to a single space. Newlines are not affected.

  ```sv
  // ParameterList: true
  module test #(N = 1)(input [N-1:0] in, output [N-1:0] out);
    ...
  endmodule
  ```

- `bool PortList` If `true`, insert a single space before a port list; multiple
  spaces are normalized to a single space. Newlines are not affected.

  ```sv
  // ParameterList: true
  module test#(N = 1) (input [N-1:0] in, output [N-1:0] out);
    ...
  endmodule
  ```

### SpacesBeforeTrailingComments

Type: `unsigned`

Number of spaces before a trailing end-of-line comment. Both `//` and `/* */`
comments that follow code on the same line are affected. Standalone comment
lines are not affected.

### SpacesInBraces

Type: `bool`

If `true`, insert a single space after opening braces and before closing braces;
multiple spaces are normalized to a single space. Newlines are not affected.

Example output:

```sv
B = { 4'h6, 4'hf };
```

### SpacesInBrackets

Type: `bool`

If `true`, insert a single space after opening brackets and before closing
brackets in declarations; multiple spaces are normalized to a single space.
Newlines are not affected. Brackets in expressions are not affected.

Example output:

```sv
bit [ 7:0 ] VPI;
bit [ 11:0 ] VCI;
```

### SpacesInParens

Type: `bool`

If `true`, insert a single space after opening parentheses and before closing
parentheses; multiple spaces are normalized to a single space. Newlines are not
affected.

Example output:

```sv
if ( !clear ) assign q = 0;
else if ( !preset ) assign q = 1;
else deassign q;
```

### UnpackedDimensionBounds

Type: [DimensionBoundsStyle](@ref slang::format::DimensionBoundsStyle)

Controls the ordering of bounds in unpacked dimension ranges. When set to
`MSBFirst` or `LSBFirst`, left and right bounds of unpacked dimension ranges are
swapped so that the larger or smaller value appears first, respectively. Only
dimensions where both bounds are integer literals are affected; dimensions with
non-literal expressions (e.g. parameters, binary expressions) are left unchanged.

Possible values:

- `LSBFirst` The smaller value appears as the left bound.

  ```sv
  // LSBFirst: true
  logic [7:0] mema [0:255];
  ```

- `MSBFirst` The larger value appears as the left bound.

  ```sv
  // MSBFirst: true
  logic [7:0] mema [255:0];
  ```

- `Preserve` Bounds are left as-is.

[1]: https://ieeexplore.ieee.org/document/10458102
[2]: https://chipsalliance.github.io/verible/
