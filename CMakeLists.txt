# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.31)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake
                               ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

project(slang-format
        DESCRIPTION "SystemVerilog code formatter"
        HOMEPAGE_URL "https://github.com/sstallion/slang-format"
        LANGUAGES CXX)

include(License)
include(Version)
include(Options)
include(Dependencies)
include(CompileFlags)

if(USE_CLANG_TIDY)
  find_package(ClangTidy)
endif()

configure_file(source/VersionInfo.cpp.in source/VersionInfo.cpp)

add_library(slang_format_lib
            source/Align.cpp
            source/AlignAssignment.cpp
            source/AlignClassifier.cpp
            source/AlignDeclaration.cpp
            source/AlignDimension.cpp
            source/AlignGroup.cpp
            source/AlignInference.cpp
            source/AlignComment.cpp
            source/Format.cpp
            source/FormatClass.cpp
            source/FormatDeclaration.cpp
            source/FormatExpression.cpp
            source/FormatMember.cpp
            source/FormatPortList.cpp
            source/FormatPrinter.cpp
            source/FormatStatement.cpp
            source/Rewriter.cpp
            source/TokenHelper.cpp
            ${CMAKE_CURRENT_BINARY_DIR}/source/VersionInfo.cpp)

add_library(slang::format ALIAS slang_format_lib)

set_target_properties(slang_format_lib
                      PROPERTIES OUTPUT_NAME svformat
                                 VERSION ${PROJECT_VERSION}
                                 SOVERSION ${PROJECT_VERSION_MAJOR}
                                 CXX_VISIBILITY_PRESET hidden
                                 VISIBILITY_INLINES_HIDDEN ON)

include(GenerateExportHeader)
generate_export_header(slang_format_lib
                       BASE_NAME SLANG_FORMAT
                       EXPORT_FILE_NAME include/slang/format/Export.h)

target_include_directories(slang_format_lib
                           PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

target_link_libraries(slang_format_lib
                      PRIVATE slang::slang)

if(BUILD_TESTING)
  enable_testing()

  add_executable(slang_format_lib_tests
                 tests/AlignAssignmentTest.cpp
                 tests/AlignCommentTest.cpp
                 tests/AlignDeclarationTest.cpp
                 tests/AlignDimensionTest.cpp
                 tests/AlignInferenceTest.cpp
                 tests/FormatClassTest.cpp
                 tests/FormatDeclarationTest.cpp
                 tests/FormatExpressionTest.cpp
                 tests/FormatMemberTest.cpp
                 tests/FormatPortListTest.cpp
                 tests/FormatPrinterTest.cpp
                 tests/FormatStatementTest.cpp
                 tests/FormatTest.cpp
                 tests/RewriterTest.cpp)

  target_include_directories(slang_format_lib_tests
                             PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source)

  target_link_libraries(slang_format_lib_tests
                        slang_format_lib
                        GTest::gtest_main
                        GTest::gtest)

  include(GoogleTest)
  gtest_discover_tests(slang_format_lib_tests
                       DISCOVERY_TIMEOUT 10)
endif()

if(BUILD_TOOLS)
  add_subdirectory(tools)
endif()

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(BUILD_DOCS)
  set(PROJECT_DOCS_SITE "slang-format.io")
  set(PROJECT_DOCS_URL "https://${PROJECT_DOCS_SITE}")

  configure_file(docs/Doxyfile.in docs/Doxyfile)
  configure_file(docs/conf.py.in docs/conf.py)

  find_package(Python)
  add_custom_target(docs ALL
                    COMMAND ${Python_EXECUTABLE} ${MCSS_DOXYGEN_SCRIPT} docs/conf.py
                    COMMENT "Building documentation")
endif()

if (BUILD_PACKAGING)
  set(CPACK_PACKAGE_NAME ${PROJECT_NAME})

  if(WIN32)
    set(CPACK_GENERATOR "ZIP")
  else()
    set(CPACK_GENERATOR "TGZ")
  endif()

  # The slang project packages executables using GitHub Actions rather than
  # CPack; naming conventions established upstream are followed below.
  if(APPLE)
    set(CPACK_SYSTEM_NAME macos)
  elseif(WIN32)
    set(CPACK_SYSTEM_NAME windows)
  else()
    string(TOLOWER ${CMAKE_SYSTEM_NAME} CPACK_SYSTEM_NAME)
  endif()

  string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} CPACK_SYSTEM_PROCESSOR)
  if(CPACK_SYSTEM_PROCESSOR STREQUAL aarch64)
    set(CPACK_SYSTEM_PROCESSOR arm64)
  elseif(CPACK_SYSTEM_PROCESSOR STREQUAL amd64)
    set(CPACK_SYSTEM_PROCESSOR x86_64)
  endif()

  string(JOIN - CPACK_PACKAGE_FILE_NAME
                ${CPACK_PACKAGE_NAME}
                ${CPACK_SYSTEM_NAME}
                ${CPACK_SYSTEM_PROCESSOR})

  set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)

  include(CPack)
endif()
