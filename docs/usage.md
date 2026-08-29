# Usage

slang-format is a standalone tool that formats SystemVerilog source code. It can
be used to format files in place, write formatted output to stdout, or read from
stdin as part of a pipeline.

## Standalone Tool

When invoked with one or more file arguments, slang-format formats each file and
writes the result to stdout. Use `-i` to edit files in place.

```
OVERVIEW: SystemVerilog code formatter

USAGE: slang-format [options] files...

OPTIONS:
  --dump-config       Dump configuration options to stdout and exit
  --files <filename>  A file containing a list of files to process, one per line.
                      Blank lines and lines starting with '#' are ignored
  -h,--help           Display available options
  -i                  Inplace edit <files>, if specified
  --list-ignored      List ignored files
  --version           Display version information and exit
```

A file argument of `-` causes slang-format to read from stdin:

```shell
echo "module foo; endmodule" | slang-format -
```

A file argument prefixed with `@` is treated as a response file containing a
list of paths to process, one per line. Blank lines and lines starting with `#`
are ignored:

```shell
slang-format @filelist.txt
```

## Configuration

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

To generate a configuration file from the current defaults, issue:

```shell
slang-format --dump-config > .slang-format
```

See [Style Options][1] for a complete list of supported options.

## Ignoring Files

slang-format supports `.slang-format-ignore` (or `_slang-format-ignore`) files
to make the formatter ignore certain files. A `.slang-format-ignore` file
consists of patterns of file path names. It has the following format:

- A blank line is skipped.
- Leading and trailing spaces of a line are trimmed.
- A line starting with a hash (`#`) is a comment.
- A non-comment line is a single pattern.
- The slash (`/`) is used as the directory separator.
- Patterns are relative to the directory of the `.slang-format-ignore` file.
- Patterns follow the rules specified in [POSIX 2.13.1, 2.13.2, and Rule 1 of
  2.13.3][2].
- Both `**` (bash globstar) and `...` (LRM 33.3.1) are supported for recursive
  directory matching.
- A pattern is negated if it starts with a bang (`!`).

Use `--list-ignored` to see which files would be ignored:

```shell
slang-format --list-ignored *.sv
```

[1]: https://github.com/sstallion/slang-format/blob/main/docs/style-options.md
[2]: https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_13
