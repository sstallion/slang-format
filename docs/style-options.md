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
