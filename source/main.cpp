// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "Style.h"

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <slang/util/CommandLine.h>

using namespace slang::format;
using namespace std::literals;

int main(int argc, char* argv[]) {
    slang::CommandLine cmdLine;

    std::optional<bool> help;
    std::optional<bool> version;
    std::vector<std::string> files;

    cmdLine.add("-h,--help", help, "Display available options");
    cmdLine.add("--version", version, "Display version information and exit");
    cmdLine.setPositional(files, "files");

    if (!cmdLine.parse(argc, const_cast<const char* const*>(argv))) {
        for (auto& err : cmdLine.getErrors()) {
            std::cerr << err << "\n";
        }
        return EXIT_FAILURE;
    }

    if (help.value_or(false)) {
        std::cout << cmdLine.getHelpText("SystemVerilog code formatter") << "\n";
        return EXIT_SUCCESS;
    }

    if (version.value_or(false)) {
        std::cout << "slang-format version " << SLANG_FORMAT_VERSION << "\n";
        return EXIT_SUCCESS;
    }

    if (files.empty()) {
        std::cerr << cmdLine.getHelpText("SystemVerilog code formatter") << "\n";
        return EXIT_FAILURE;
    }

    try {
        for (const auto& file : files) {
            std::string result;

            if (file == "-"sv) {
                auto style = getStyle(std::filesystem::current_path());
                result = reformat(std::cin, style);
            }
            else {
                const std::filesystem::path path{file};
                std::ifstream stream{path};
                if (!stream) {
                    const std::error_code ec{errno, std::system_category()};
                    throw std::runtime_error{path.string() + ": " + ec.message()};
                }
                auto style = getStyle(path.parent_path());
                result = reformat(stream, style);
            }

            std::cout << result;
        }
    }
    catch (const std::exception& e) {
        std::cerr << cmdLine.getProgramName() << ": " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
