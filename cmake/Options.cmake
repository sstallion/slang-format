# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

include(CMakeDependentOption)

option(BUILD_SHARED_LIBS "Build shared libraries." OFF)
option(BUILD_TESTING "Build tests." ${PROJECT_IS_TOP_LEVEL})
option(BUILD_TOOLS "Build tools." ${PROJECT_IS_TOP_LEVEL})
option(BUILD_EXAMPLES "Build examples." ${PROJECT_IS_TOP_LEVEL})
option(BUILD_DOCS "Build documentation." OFF)

cmake_dependent_option(BUILD_PACKAGING "Build packages." ON
                       "PROJECT_IS_TOP_LEVEL AND BUILD_TOOLS" OFF)

option(USE_CLANG_TIDY "Run clang-tidy with the compiler." ${PROJECT_IS_TOP_LEVEL})
