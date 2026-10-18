# Verible Compatibility

This document tracks the compatibility state between [Verible][1]'s default
formatting behavior and slang-format's output. The golden reference file is
`tests/fixtures/verible.sv.expected`, which is the canonical output of
`verible-verilog-format` run with no flags against the torture test fixture
`tests/fixtures/verible.sv`.

## Status

668 of 668 tests pass.

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

When keyword padding is active on lines with packed dimensions, the pre-dim
spacing is normalized to a single space. This prevents dim-alignment padding
from inflating the identifier column after keyword padding shifts the type body.

### minGuarantee

The `minGuarantee` flag in `InferContext` causes `inferAlignPolicy` to return
`Align` for displacements of 1 or 2 regardless of InferFromSource excess. It is
enabled only for declaration groups with no direction or parameter keywords
(bare-type groups such as `logic x; logic [7:0] y;`). The flag is gated on the
`hasKeywords` field of `DeclGroupSummary`, not on `maxKeywordWidth`, because
keyword padding may disable `maxKeywordWidth` for space reasons without changing
whether the group semantically contains keywords.

### Range Excess in InferFromSource

The `computeGroupMaxExcess` function includes range spacing excess from port
list lines that are not adjacent to a parameter group with alignment evidence.
This allows the InferFromSource heuristic to detect author intent from
multi-space range spacing (e.g., `[W-1     :    0]`) in port declarations where
token spacing alone would indicate flush-left alignment. The
`computeRangeExcessLines` function determines which lines qualify.

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
