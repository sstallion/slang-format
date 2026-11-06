// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#include <iostream>

#include <slang/format/Format.h>
#include <slang/format/styles/Default.h>

using namespace slang::format;

int main() {
    std::cout << reformat(std::cin, getDefaultStyle());
    return 0;
}
