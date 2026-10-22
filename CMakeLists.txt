# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.31)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

project(slang-format
        DESCRIPTION "SystemVerilog code formatter"
        HOMEPAGE_URL "https://github.com/sstallion/slang-format"
        LANGUAGES CXX)

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake
                               ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

include(Options)
include(Dependencies)
include(CompileFlags)
include(License)
include(Version)

if(USE_CLANG_TIDY)
  find_package(ClangTidy)
endif()

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

configure_file(${SOURCE_DIR}/VersionInfo.cpp.in
               ${CMAKE_CURRENT_BINARY_DIR}/VersionInfo.cpp)

add_library(slang_format_lib
            ${SOURCE_DIR}/Align.cpp
            ${SOURCE_DIR}/AlignAssignment.cpp
            ${SOURCE_DIR}/AlignClassifier.cpp
            ${SOURCE_DIR}/AlignDeclaration.cpp
            ${SOURCE_DIR}/AlignDimension.cpp
            ${SOURCE_DIR}/AlignGroup.cpp
            ${SOURCE_DIR}/AlignInference.cpp
            ${SOURCE_DIR}/AlignComment.cpp
            ${SOURCE_DIR}/Format.cpp
            ${SOURCE_DIR}/FormatClass.cpp
            ${SOURCE_DIR}/FormatDeclaration.cpp
            ${SOURCE_DIR}/FormatExpression.cpp
            ${SOURCE_DIR}/FormatMember.cpp
            ${SOURCE_DIR}/FormatPortList.cpp
            ${SOURCE_DIR}/FormatPrinter.cpp
            ${SOURCE_DIR}/FormatStatement.cpp
            ${SOURCE_DIR}/Rewriter.cpp
            ${SOURCE_DIR}/TokenHelper.cpp
            ${CMAKE_CURRENT_BINARY_DIR}/VersionInfo.cpp)

add_library(slang::format ALIAS slang_format_lib)

include(GenerateExportHeader)
generate_export_header(slang_format_lib
                       BASE_NAME SLANG_FORMAT
                       EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/slang/format/Export.h)

target_include_directories(slang_format_lib
                           PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>")

target_link_libraries(slang_format_lib
                      PRIVATE slang::slang)

set_target_properties(slang_format_lib
                      PROPERTIES CXX_VISIBILITY_PRESET hidden
                                 VISIBILITY_INLINES_HIDDEN YES
                                 OUTPUT_NAME svformat
                                 VERSION ${PROJECT_VERSION}
                                 SOVERSION ${PROJECT_VERSION_MAJOR})

if(BUILD_TESTING)
  enable_testing()

  add_executable(slang_format_lib_tests
                 ${TESTS_DIR}/AlignAssignmentTest.cpp
                 ${TESTS_DIR}/AlignCommentTest.cpp
                 ${TESTS_DIR}/AlignDeclarationTest.cpp
                 ${TESTS_DIR}/AlignDimensionTest.cpp
                 ${TESTS_DIR}/AlignInferenceTest.cpp
                 ${TESTS_DIR}/FormatClassTest.cpp
                 ${TESTS_DIR}/FormatDeclarationTest.cpp
                 ${TESTS_DIR}/FormatExpressionTest.cpp
                 ${TESTS_DIR}/FormatMemberTest.cpp
                 ${TESTS_DIR}/FormatPortListTest.cpp
                 ${TESTS_DIR}/FormatPrinterTest.cpp
                 ${TESTS_DIR}/FormatStatementTest.cpp
                 ${TESTS_DIR}/FormatTest.cpp
                 ${TESTS_DIR}/RewriterTest.cpp)

  target_include_directories(slang_format_lib_tests
                             PRIVATE ${SOURCE_DIR})

  target_link_libraries(slang_format_lib_tests
                        slang_format_lib
                        GTest::gtest_main
                        GTest::gtest)

  include(GoogleTest)
  gtest_discover_tests(slang_format_lib_tests
                       DISCOVERY_TIMEOUT 30)
endif()

if(BUILD_TOOLS)
  add_subdirectory(tools)
endif()

if (BUILD_PACKAGING)
  set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
  set(CPACK_PACKAGE_NAME ${PROJECT_NAME})

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

  if(WIN32)
    set(CPACK_GENERATOR "ZIP")
  else()
    set(CPACK_GENERATOR "TGZ")
  endif()

  include(CPack)
endif()
