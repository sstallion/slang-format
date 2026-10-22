# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_library(slang_format_obj_lib OBJECT
            ${SOURCE_DIR}/ConfigFile.cpp
            ${SOURCE_DIR}/FileLoader.cpp
            ${SOURCE_DIR}/IgnoreFile.cpp)

target_include_directories(slang_format_obj_lib
                           PUBLIC ${SOURCE_DIR})

target_link_libraries(slang_format_obj_lib
                      PUBLIC slang::format
                             slang::slang
                             yaml-cpp::yaml-cpp)

add_executable(slang_format
               ${SOURCE_DIR}/main.cpp)

add_executable(slang::tools::format ALIAS slang_format)

set_target_properties(slang_format
                      PROPERTIES OUTPUT_NAME slang-format)

target_link_libraries(slang_format
                      PRIVATE slang_format_obj_lib)

if(BUILD_TESTING)
  set(FIXTURES_DIR ${TESTS_DIR}/fixtures)
  add_subdirectory(${FIXTURES_DIR})

  add_executable(slang_format_tests
                 ${TESTS_DIR}/ConfigFileTest.cpp
                 ${TESTS_DIR}/IgnoreFileTest.cpp)

  target_link_libraries(slang_format_tests
                        slang_format_obj_lib
                        GTest::gtest_main
                        GTest::gtest)

  include(GoogleTest)
  gtest_discover_tests(slang_format_tests
                       DISCOVERY_TIMEOUT 30)
endif()

install(TARGETS slang_format DESTINATION .)
