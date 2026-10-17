# Verible Compatibility

This document tracks the compatibility state between [Verible][1]'s default
formatting behavior and slang-format's output. The golden reference file is
`tests/fixtures/verible.sv.expected`, which is the canonical output of
`verible-verilog-format` run with no flags against the torture test fixture
`tests/fixtures/verible.sv`.

## Status

660 of 661 tests pass. The single remaining failure is the Verible fixture test,
which has 1 differing hunk across 1 root cause category:

| Category                                | Hunks | Lines   |
| --------------------------------------- | ----- | ------- |
| Missing keyword padding and dim PadLeft | 1     | 301-303 |

The category is described below with root cause analysis and the refactoring
required to resolve it.

## Deviation: Missing Keyword Padding and Dim PadLeft

### Affected Lines

| Line | Actual                     | Expected                    |
| ---- | -------------------------- | --------------------------- |
| 301  | `input wire [W-1:0] a,`    | `input  wire [  W-1:0] a,`  |
| 302  | `input wire [W-1 : 0] b,`  | `input  wire [W-1 : 0] b,`  |
| 303  | `output wire [W-1:0] sum`  | `output wire [  W-1:0] sum` |

### Root Cause

The expected output requires two simultaneous alignment features:

1. **Keyword padding**: `input` (5 chars) padded to `output` (6 chars) width,
   producing `input  wire` (2 spaces between keyword and type body).
2. **Dim PadLeft**: `[W-1:0]` padded to `[  W-1:0]` to align `:` positions
   across the group.

Both features exist in slang-format, but the `InferFromSource` policy prevents
them from activating for this group. The adder module's source has multi-space
range spacing (`[W-1     :    0]`) whose excess is captured by
`captureTokenSpacing`. However, the `skipSpacingCapture` flag - which prevents
range normalization from inflating `groupMaxExcess` - also hides this excess
from the InferFromSource decision.

Without source excess, InferFromSource returns `FlushLeft` for both the packed
dimension alignment pass and the subsequent declaration alignment pass.

### Why Not Fixed

The `skipSpacingCapture` mechanism is load-bearing for other groups. Removing it
causes the counter module (dff) port list to incorrectly align declarations
because the range spacing excess would inflate `groupMaxExcess` above the
InferFromSource threshold.

The fundamental tension: range spacing excess is relevant for dimension
alignment decisions (where it indicates the author intended dimensional
structure) but irrelevant for declaration alignment decisions (where only
type-to-identifier spacing matters).

### Required Refactoring

Resolve the tension by tracking range and non-range excess separately:

1. Add a `rangeExcess` field to `LineMetadata::TokenSpacing` (or a parallel
   `rangeTokenSpacings` vector) that captures excess specifically from range
   operator tokens.
2. Compute `groupMaxExcess` separately for range tokens and non-range tokens.
3. Use range excess for packed dimension alignment decisions
   (`AlignConsecutivePackedDimensions`).
4. Use non-range excess for declaration alignment decisions
   (`AlignConsecutiveDeclarations`).

This separation allows the dim alignment pass to detect author intent from range
spacing while preventing range spacing from polluting declaration alignment
decisions. The implementation touches `captureTokenSpacing` in `Format.cpp` and
`computeGroupMaxExcess` in `Align.cpp`.

## Implementation Details

### Keyword Padding

Declaration keyword padding aligns direction keywords (`input`, `output`,
`inout`, `ref`) and parameter keywords (`parameter`, `localparam`) within a
group by padding shorter keywords with spaces. The implementation:

- `classifyLine` in `Align.cpp` tracks `keywordWidth` per line.
- `computeDeclGroupSummary` computes `maxKeywordWidth` across the group.
- `alignGroupDeclLine` emits the keyword, pads to `maxKeywordWidth`, then emits
  the type body.

Keyword padding is disabled when `maxKeywordWidth + 1 + maxBodyWidth` exceeds
the natural `maxTypeWidth` (the largest `typeWidth` observed in the group). This
prevents keyword padding from inflating the identifier column beyond what the
widest line requires.

### minGuarantee

The `minGuarantee` flag in `InferContext` causes `inferAlignPolicy` to return
`Align` for displacements of 1 or 2 regardless of InferFromSource excess. It is
enabled only for declaration groups with no direction or parameter keywords
(bare-type groups such as `logic x; logic [7:0] y;`). The flag is gated on the
`hasKeywords` field of `DeclGroupSummary`, not on `maxKeywordWidth`, because
keyword padding may disable `maxKeywordWidth` for space reasons without changing
whether the group semantically contains keywords.

### Struct and Union Member Classification

`StructUnionMemberSyntax` nodes do not inherit from `DataDeclarationSyntax` in
slang's syntax tree. A dedicated handler in `Format.cpp` sets
`nextLineKind = Declaration` so that the alignment pass correctly classifies
members of `struct` and `union` types (including user-defined types like
`beat_t`) as declarations.

When the text-based classifier in `Align.cpp` cannot parse a line as a
declaration (e.g., unrecognized user-defined types), the
`reclassifyAsDeclaration` helper extracts `typeWidth` and `identPos` from the
first-word/second-word pattern, provided the metadata has flagged the line as a
declaration.

[1]: https://github.com/chipsalliance/verible
