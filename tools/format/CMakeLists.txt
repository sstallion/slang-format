# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

add_library(slang_format_obj_lib OBJECT
            source/ConfigFile.cpp
            source/FileLoader.cpp
            source/IgnoreFile.cpp)

target_link_libraries(slang_format_obj_lib
                      PUBLIC slang::format
                             slang::slang
                             yaml-cpp::yaml-cpp)

add_executable(slang_format
               source/main.cpp)

add_executable(slang::tools::format ALIAS slang_format)

set_target_properties(slang_format
                      PROPERTIES OUTPUT_NAME slang-format)

target_link_libraries(slang_format
                      PRIVATE slang_format_obj_lib)

if(BUILD_TESTING)
  add_executable(slang_format_tests
                 tests/ConfigFileTest.cpp
                 tests/IgnoreFileTest.cpp)

  target_include_directories(slang_format_tests
                             PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source)

  target_link_libraries(slang_format_tests
                        slang_format_obj_lib
                        GTest::gtest_main
                        GTest::gtest)

  include(GoogleTest)
  gtest_discover_tests(slang_format_tests
                       DISCOVERY_TIMEOUT 10)

  # Fixture tests are used to verify user-facing behavior that cannot be
  # easily covered by unit tests.
  file(GLOB fixtures "tests/fixtures/*.cmake")
  foreach(fixture ${fixtures})
    get_filename_component(name ${fixture} NAME_WE)
    add_test(NAME "Format.${name}"
             COMMAND ${CMAKE_COMMAND}
                     "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}"
                     "-DFIXTURE_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures"
                     "-DFIXTURE_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}/tests/fixtures"
                     "-DFIXTURE_EXECUTABLE=$<TARGET_FILE:slang_format>"
                     "-DFIXTURE_FLAGS=--no-config" # ignore config files
                     -P ${fixture})
  endforeach()
endif()

install(TARGETS slang_format DESTINATION .)
