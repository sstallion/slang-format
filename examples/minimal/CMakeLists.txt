# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

add_executable(slang_format_minimal
               source/main.cpp)

target_link_libraries(slang_format_minimal
                      PRIVATE slang::format)
