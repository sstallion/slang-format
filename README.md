# slang-format

[![](https://github.com/sstallion/slang-format/actions/workflows/ci.yaml/badge.svg?branch=main)][1]
[![](https://img.shields.io/github/v/release/sstallion/slang-format)][2]
[![](https://img.shields.io/github/license/sstallion/slang-format.svg)][3]

slang-format is an experimental SystemVerilog code formatter based on [slang][4]
and heavily inspired by [clang-format][5]. Verilog-2005 (IEEE 1364-2005) is
fully supported as a subset of the SystemVerilog language. All major operating
systems are supported, including Linux, macOS, and Windows.

## Getting Started

> [!IMPORTANT]
> CMake 3.31 is required to build this repository; minimum supported compilers
> are GCC 11, Clang 14, and Visual Studio 2019.

Building release binaries differs slightly between platforms when using the
default CMake generator.

On Linux and macOS, issue:

```shell
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build
```

On Windows, issue:

```shell
cmake -B build && cmake --build build --config Release
```

> [!NOTE]
> Once built, binaries can be found in the `build` directory.

### Configuration

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

See [Style Options][6] for a complete list of supported options.

## Contributing

Pull requests are welcome! See [Contributing][7] for details.

## License

Source code in this repository is licensed under the MIT License. See
[LICENSE][3] for details.

[1]: https://github.com/sstallion/slang-format/actions/workflows/ci.yml
[2]: https://github.com/sstallion/slang-format/releases/latest
[3]: https://github.com/sstallion/slang-format/blob/main/LICENSE
[4]: https://sv-lang.com/
[5]: https://clang.llvm.org/docs/ClangFormat.html
[6]: https://github.com/sstallion/slang-format/blob/main/docs/style-options.md
[7]: https://github.com/sstallion/slang-format/blob/main/CONTRIBUTING.md
