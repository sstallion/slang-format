---
paths:
  - include/slang/format/styles/*.h
  - include/slang/format/Style.h
---

# Style Option Rules

## Add a Style Option

A style option is mirrored in several places, all of which change together:

1. `Style.h` - Add the field with its documentation. The header carries
   documentation and types only, never values.
2. Every style header under `include/slang/format/styles/` - Add the value
   appropriate for that style.
3. `ConfigFile.cpp` - Parse the key and insert it at the correct alphabetical
   position in `dumpConfig()`.
4. `ConfigFileTest.cpp` - Add a round-trip test.
5. `docs/style-options.md` - Document the option.
