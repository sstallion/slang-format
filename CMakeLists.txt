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

find_package(ClangTidy)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

include_directories(${SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/Config.h)

add_library(${PROJECT_NAME}-object OBJECT
            ${SOURCE_DIR}/Format.cpp
            ${SOURCE_DIR}/Ignore.cpp
            ${SOURCE_DIR}/Rewriter.cpp
            ${SOURCE_DIR}/Style.cpp)

target_link_libraries(${PROJECT_NAME}-object
                      slang::slang
                      yaml-cpp::yaml-cpp)

add_executable(${PROJECT_NAME}
               ${SOURCE_DIR}/main.cpp)

target_link_libraries(${PROJECT_NAME}
                      ${PROJECT_NAME}-object)

if(BUILD_TESTING)
  enable_testing()

  set(FIXTURES_DIR ${TESTS_DIR}/fixtures)
  add_subdirectory(${FIXTURES_DIR})

  add_executable(${PROJECT_NAME}-tests
                 ${TESTS_DIR}/FormatTest.cpp
                 ${TESTS_DIR}/IgnoreTest.cpp
                 ${TESTS_DIR}/RewriterTest.cpp
                 ${TESTS_DIR}/StyleTest.cpp)

  target_link_libraries(${PROJECT_NAME}-tests
                        ${PROJECT_NAME}-object
                        GTest::gtest
                        GTest::gtest_main)

  include(GoogleTest)
  gtest_discover_tests(${PROJECT_NAME}-tests)
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION .)

if (BUILD_PACKAGING)
  set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
  set(CPACK_PACKAGE_NAME ${PROJECT_NAME})

  # The slang project packages executables using GitHub actions rather than
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
