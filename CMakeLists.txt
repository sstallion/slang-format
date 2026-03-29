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
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

include_directories(${SOURCE_DIR})

add_library(${PROJECT_NAME}-object OBJECT
            ${SOURCE_DIR}/Format.cpp
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

  add_executable(${PROJECT_NAME}-tests
                 ${TEST_DIR}/FormatTest.cpp
                 ${TEST_DIR}/RewriterTest.cpp
                 ${TEST_DIR}/StyleTest.cpp)

  target_link_libraries(${PROJECT_NAME}-tests
                        ${PROJECT_NAME}-object
                        GTest::gtest
                        GTest::gtest_main)

  include(GoogleTest)
  gtest_discover_tests(${PROJECT_NAME}-tests)
endif()
