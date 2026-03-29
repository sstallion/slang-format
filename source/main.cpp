// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "Style.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

using namespace slang::format;
using namespace std::literals;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <file> ..." << "\n";
        return EXIT_FAILURE;
    }

    try {
        for (auto i = 1; i < argc; i++) {
            std::string result;

            if (argv[i] == "-"sv) {
                auto style = getStyle(std::filesystem::current_path());
                result = reformat(std::cin, style);
            }
            else {
                const std::filesystem::path path{argv[i]};
                std::ifstream file{path};
                if (!file) {
                    const std::error_code ec{errno, std::system_category()};
                    throw std::runtime_error{path.string() + ": " + ec.message()};
                }
                auto style = getStyle(path.parent_path());
                result = reformat(file, style);
            }

            std::cout << result;
        }
    }
    catch (const std::exception& e) {
        std::cerr << argv[0] << ": " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
