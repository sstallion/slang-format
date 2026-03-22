# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_definitions(
  "$<$<CONFIG:Debug>:DEBUG>"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(
    -Wall
    -Wextra
    -Werror
  )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options(
    /W4
    /WX
  )
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
