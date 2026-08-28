// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "Format.h"
#include "Ignore.h"
#include "Style.h"

#include <cerrno>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

#include <slang/util/CommandLine.h>

using namespace slang::format;
using namespace std::literals;

namespace {

std::vector<std::string> readFileList(const std::filesystem::path& path) {
    std::ifstream stream{path};
    if (!stream) {
        const std::error_code ec{errno, std::system_category()};
        throw std::runtime_error{path.string() + ": " + ec.message()};
    }

    std::vector<std::string> result;
    std::string line;
    while (std::getline(stream, line)) {
        if (line.empty() || line.starts_with('#')) {
            continue;
        }
        result.push_back(std::move(line));
    }
    return result;
}

std::vector<std::string> buildFileList(std::vector<std::string>& positionalFiles,
                                       const std::vector<std::string>& fileListPaths) {
    std::vector<std::string> files;
    for (auto& arg : positionalFiles) {
        if (arg.starts_with('@')) {
            auto entries = readFileList(arg.substr(1));
            files.insert(files.end(), std::make_move_iterator(entries.begin()),
                         std::make_move_iterator(entries.end()));
        }
        else {
            files.push_back(std::move(arg));
        }
    }

    for (const auto& path : fileListPaths) {
        auto entries = readFileList(path);
        files.insert(files.end(), std::make_move_iterator(entries.begin()),
                     std::make_move_iterator(entries.end()));
    }
    return files;
}

void listIgnoredFiles(const std::vector<std::string>& files) {
    for (const auto& file : files) {
        if (file == "-"sv) {
            continue;
        }

        const auto path = std::filesystem::absolute(file);
        if (isIgnored(path)) {
            std::cout << file << "\n";
        }
    }
}

int formatFiles(std::string_view programName, const std::vector<std::string>& files) {
    auto failed = 0;
    for (const auto& file : files) {
        try {
            std::string result;

            if (file == "-"sv) {
                auto style = getStyle(std::filesystem::current_path());
                result = reformat(std::cin, style);
            }
            else {
                const auto path = std::filesystem::absolute(file);
                if (isIgnored(path)) {
                    continue;
                }

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
        catch (const std::exception& e) {
            std::cerr << programName << ": " << e.what() << "\n";
            failed++;
        }
    }
    return failed;
}

} // namespace

int main(int argc, char* argv[]) {
    slang::CommandLine cmdLine;

    std::optional<bool> dumpConfig;
    std::optional<bool> help;
    std::optional<bool> listIgnored;
    std::optional<bool> version;
    std::vector<std::string> fileListPaths;
    std::vector<std::string> positionalFiles;

    cmdLine.add("--dump-config", dumpConfig, "Dump configuration options to stdout and exit");
    cmdLine.add("-h,--help", help, "Display available options");
    cmdLine.add("--list-ignored", listIgnored, "List ignored files");
    cmdLine.add("--version", version, "Display version information and exit");
    cmdLine.add("--files", fileListPaths,
                "A file containing a list of files to process, one per line.\n"
                "Blank lines and lines starting with '#' are ignored",
                "<filename>");
    cmdLine.setPositional(positionalFiles, "files");

    if (!cmdLine.parse(argc, const_cast<const char* const*>(argv))) {
        for (const auto& err : cmdLine.getErrors()) {
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

    if (dumpConfig.value_or(false)) {
        auto style = getStyle(std::filesystem::current_path());
        std::cout << dumpConfiguration(style) << "\n";
        return EXIT_SUCCESS;
    }

    try {
        auto files = buildFileList(positionalFiles, fileListPaths);
        if (files.empty()) {
            std::cerr << cmdLine.getHelpText("SystemVerilog code formatter") << "\n";
            return EXIT_FAILURE;
        }

        if (listIgnored.value_or(false)) {
            listIgnoredFiles(files);
            return EXIT_SUCCESS;
        }

        auto failed = formatFiles(cmdLine.getProgramName(), files);
        return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    }
    catch (const std::exception& e) {
        std::cerr << cmdLine.getProgramName() << ": " << e.what() << "\n";
        return EXIT_FAILURE;
    }
}
