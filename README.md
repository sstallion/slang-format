# slang-format

[![](https://github.com/sstallion/slang-format/actions/workflows/ci.yml/badge.svg?branch=main)][1]
[![](https://codecov.io/github/sstallion/slang-format/graph/badge.svg?token=CX8I80Q0DU)][2]
[![](https://img.shields.io/github/v/release/sstallion/slang-format)][3]
[![](https://img.shields.io/github/license/sstallion/slang-format.svg)][4]

slang-format is an experimental SystemVerilog code formatter based on [slang][5]
and heavily inspired by [clang-format][6]. Verilog-2005 (IEEE 1364-2005) is
fully supported as a subset of the SystemVerilog language. All major operating
systems are supported, including Linux, macOS, and Windows.

## Getting Started

> [!IMPORTANT]
> CMake 3.31 is required to build this repository; minimum supported compilers
> are GCC 11, Clang 14, and Visual Studio 2022.

Building release binaries differs slightly between platforms when using the
default CMake generator.

On Linux and macOS, issue:

```
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build
```

On Windows, issue:

```
cmake -B build && cmake --build build --config Release
```

Once built, binaries can be found in the `build` directory.

> [!TIP]
> See [Usage][7] for command-line options, configuration, and supported file
> patterns.

## Contributing

Pull requests are welcome! See [Contributing][8] for details.

## License

Source code in this repository is licensed under the MIT License. See
[LICENSE][4] for details.

[1]: https://github.com/sstallion/slang-format/actions/workflows/ci.yml
[2]: https://codecov.io/github/sstallion/slang-format
[3]: https://github.com/sstallion/slang-format/releases/latest
[4]: https://github.com/sstallion/slang-format?tab=MIT-1-ov-file
[5]: https://sv-lang.com/
[6]: https://clang.llvm.org/docs/ClangFormat.html
[7]: https://slang-format.io/usage.html
[8]: https://github.com/sstallion/slang-format/blob/main/CONTRIBUTING.md
