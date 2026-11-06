// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include "ConfigFile.h"
#include "IgnoreFile.h"
#include "slang/format/Format.h"
#include "slang/format/VersionInfo.h"
#include "slang/format/styles/Default.h"

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

[[noreturn]] void throwFileError(const std::filesystem::path& path) {
    const std::error_code ec{errno, std::system_category()};
    throw std::runtime_error{path.string() + ": " + ec.message()};
}

std::vector<std::string> readFileList(const std::filesystem::path& path) {
    std::ifstream stream{path};
    if (!stream) {
        throwFileError(path);
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

void listIgnoredFiles(const std::vector<std::string>& files, bool noIgnore) {
    for (const auto& file : files) {
        if (file == "-"sv) {
            continue;
        }

        const auto path = std::filesystem::absolute(file);
        if (!noIgnore && isIgnored(path)) {
            std::cout << file << "\n";
        }
    }
}

std::string readFile(const std::filesystem::path& path) {
    std::ifstream stream{path};
    if (!stream) {
        throwFileError(path);
    }
    return {std::istreambuf_iterator<char>{stream}, std::istreambuf_iterator<char>{}};
}

void writeFile(const std::filesystem::path& path, const std::string& content) {
    std::ofstream stream{path, std::ios::trunc};
    if (!stream) {
        throwFileError(path);
    }
    stream << content;
}

int formatFiles(std::string_view programName, const std::vector<std::string>& files, bool inplace,
                bool noConfig, bool noIgnore) {
    auto failed = 0;
    for (const auto& file : files) {
        try {
            if (file == "-"sv) {
                if (inplace) {
                    continue;
                }
                auto style = noConfig ? getDefaultStyle()
                                      : loadConfig(std::filesystem::current_path());
                std::cout << reformat(std::cin, style);
                continue;
            }

            const auto path = std::filesystem::absolute(file);
            if (!noIgnore && isIgnored(path)) {
                continue;
            }

            auto style = noConfig ? getDefaultStyle() : loadConfig(path.parent_path());

            if (inplace) {
                auto source = readFile(path);
                auto result = reformat(std::string_view{source}, style);
                if (result != source) {
                    writeFile(path, result);
                }
            }
            else {
                std::ifstream stream{path};
                if (!stream) {
                    throwFileError(path);
                }
                std::cout << reformat(stream, style);
            }
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
    std::optional<bool> inplace;
    std::optional<bool> listIgnored;
    std::optional<bool> noConfig;
    std::optional<bool> noIgnored;
    std::optional<bool> version;
    std::vector<std::string> fileListPaths;
    std::vector<std::string> positionalFiles;

    cmdLine.add("--dump-config", dumpConfig, "Dump configuration options to stdout and exit.");
    cmdLine.add("--files", fileListPaths,
                "A file containing a list of files to process.\n"
                "Blank lines and lines starting with '#' are ignored.",
                "<filename>");
    cmdLine.add("-i", inplace, "Inplace edit <file>s, if specified.");
    cmdLine.add("--list-ignored", listIgnored, "List ignored files.");
    cmdLine.add("--no-config", noConfig, "Do not search for .slang-format files.");
    cmdLine.add("--no-ignored", noIgnored, "Do not search for .slang-format-ignore files.");
    cmdLine.add("-h,--help", help, "Display available options.");
    cmdLine.add("--version", version, "Display version information and exit.");
    cmdLine.setPositional(positionalFiles, "[@<file>] [<file>]");

    if (!cmdLine.parse(argc, const_cast<const char* const*>(argv))) {
        for (const auto& err : cmdLine.getErrors()) {
            std::cerr << err << "\n";
        }
        return EXIT_FAILURE;
    }

    const auto programName = cmdLine.getProgramName();
    const auto overview = "SystemVerilog code formatter";

    if (help.value_or(false)) {
        std::cout << cmdLine.getHelpText(overview) << "\n";
        return EXIT_SUCCESS;
    }

    if (version.value_or(false)) {
        std::cout << programName << " version " << VersionInfo::getVersionString() << "\n";
        return EXIT_SUCCESS;
    }

    if (dumpConfig.value_or(false)) {
        auto style = noConfig.value_or(false) ? getDefaultStyle()
                                              : loadConfig(std::filesystem::current_path());
        std::cout << slang::format::dumpConfig(style) << "\n";
        return EXIT_SUCCESS;
    }

    try {
        auto files = buildFileList(positionalFiles, fileListPaths);
        if (files.empty()) {
            std::cerr << cmdLine.getHelpText(overview) << "\n";
            return EXIT_FAILURE;
        }

        if (listIgnored.value_or(false)) {
            listIgnoredFiles(files, noIgnored.value_or(false));
            return EXIT_SUCCESS;
        }

        auto failed = formatFiles(programName, files, inplace.value_or(false),
                                  noConfig.value_or(false), noIgnored.value_or(false));
        return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    }
    catch (const std::exception& e) {
        std::cerr << programName << ": " << e.what() << "\n";
        return EXIT_FAILURE;
    }
}
