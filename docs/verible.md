# Verible Compatibility

This document tracks the compatibility state between [Verible][1]'s default
formatting behavior and slang-format's output. The golden reference file is
`tests/fixtures/verible.sv.expected`, which is the canonical output of
`verible-verilog-format` run with no flags against the torture test fixture
`tests/fixtures/verible.sv`.

## Status

654 of 655 tests pass. The single remaining failure is the Verible fixture test,
which has 13 differing hunks across 3 root cause categories:

| Category                                | Hunks | Lines                              |
| --------------------------------------- | ----- | ---------------------------------- |
| InferFromSource rejects alignment       | 6     | 123, 262, 391, 396, 524, 706-723   |
| Verible multi-space range inconsistency | 4     | 243, 290, 320, 324                 |
| Missing keyword padding and dim PadLeft | 3     | 301-303                            |

Each category is described below with root cause analysis and the refactoring
required to resolve it.

## Deviation: InferFromSource Rejects Alignment

### Affected Lines

| Line(s) | Context              | Actual           | Expected            | Displacement |
| ------- | -------------------- | ---------------- | ------------------- | ------------ |
| 123     | Class constructor    | `this.id = ...`  | `this.id   = ...`   | 2            |
| 262     | Clocking block       | `cb.req <= ...`  | `cb.req  <= ...`    | 1            |
| 391     | Case statement body  | `rdy_o = ...`    | `rdy_o   = ...`     | 2            |
| 396     | Case statement body  | `rdy_o = ...`    | `rdy_o   = ...`     | 2            |
| 524     | Port connection list | `.b(data_i)`     | `.b  (data_i)`      | 2            |
| 706-709 | Bind port list       | `.clk(clk)`      | `.clk  (clk)`       | 2            |
| 723     | Instance port list   | `.clk(clk)`      | `.clk  (clk)`       | 2            |

### Root Cause

The `InferFromSource` alignment policy examines `groupMaxExcess` - the maximum
difference between source and output inter-token spacing across a group. When
the source has no excess spacing (the author wrote exactly one space between
tokens), InferFromSource concludes the author did not intend alignment and
returns `FlushLeft`.

Verible does not have an InferFromSource concept; it unconditionally aligns
assignments and port connections within groups when the displacement is small.

### Why Not Fixed

Disabling InferFromSource for assignments and port connections
(`AlignConsecutiveAssignments`, `AlignConsecutivePortConnections`) or adding a
`minGuarantee` for these passes causes regressions in existing tests:

- `PortListBoundaryBreaksGroup` fails because MaxPadding=1 with displacement=2
  should reject alignment.
- `Rule1SmallPaddingAligns` fails because InferFromSource with no source excess
  should reject alignment.

The `minGuarantee` mechanism works for declarations (where bare-type groups have
no keywords and benefit from unconditional small-displacement alignment) but
cannot be extended to assignments and port connections without breaking the
MaxPadding contract those tests verify.

### Possible Resolution

Introduce per-pass alignment defaults that separate declaration behavior
(always-align for small displacements) from assignment and port connection
behavior (InferFromSource decides). This would require either:

1. A separate `AlwaysAlignSmallDisplacements` field on `AlignConsecutiveStyle`
   that bypasses InferFromSource for displacements at or below a threshold, or
2. Changing the default `AlignConsecutiveAssignments` and
   `AlignConsecutivePortConnections` to `InferFromSource: false`, which matches
   Verible but changes slang-format's default behavior for all users.

Option 2 is the simpler change but has broader impact. Option 1 is more
surgical.

## Deviation: Verible Multi-Space Range Inconsistency

### Affected Lines

| Line | Actual        | Expected           |
| ---- | ------------- | ------------------ |
| 243  | `[DW-1 : 0]`  | `[DW-1  :    0]`   |
| 290  | `[W-1 : 0]`   | `[W-1    :    0]`  |
| 320  | `[AW-1 : 0]`  | `[AW-1   :    0]`  |
| 324  | `[DW-1 : 0]`  | `[DW-1    :    0]` |

### Root Cause

slang-format clamps range spacing to at most 1 space in `Preserve` mode
(`std::min(leadingWhitespaceLength(...), size_t{1})`). Verible normalizes most
multi-space ranges to single space but inconsistently preserves multi-space in
certain contexts.

Testing with `verible-verilog-format` confirms the inconsistency is
context-dependent. The same source pattern `[W-1    :    0]` is:

- Normalized to `[W-1 : 0]` when the port list has a single declaration.
- Preserved as `[W-1    :    0]` when the port list has multiple declarations
  with a preceding parameter section that has excess source spacing.

This behavior is not documented in Verible and appears to be an unintentional
interaction between its spacing normalization and alignment passes.

### Why Not Fixed

Removing the `std::min` clamp to achieve true preservation produces correct
results for these 4 lines but introduces 6 new failures at lines where Verible
does normalize multi-space ranges (lines 43, 61, 119, 260, 350, 556). The net
result is worse overall compatibility.

The current clamping behavior (`Preserve` normalizes to 0-or-1 space) matches
Verible's intended behavior for the majority of cases.

### Possible Resolution

If Verible's inconsistency is accepted as a bug in the golden reference:

1. Update the 4 affected lines in `verible.sv.expected` to use single-space
   range notation (matching slang-format's output), or
2. No code change required; document the deviation as a known Verible
   inconsistency.

If exact replication is required, the formatting pass would need to detect the
specific contexts where Verible preserves multi-space ranges. The relevant
context appears to be a port list group where the preceding parameter section
contributes excess source spacing to the group's metadata. This is fragile and
couples the formatter to an undocumented Verible implementation detail.

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
