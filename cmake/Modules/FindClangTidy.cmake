# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

#[=======================================================================[.rst:
ClangTidy
---------

Finds the ``clang-tidy`` executable and configures the project for its use.
If the default executable cannot be found, the module will search for the
latest version in the system path.  If the :variable:`CMAKE_EXPORT_COMPILE_COMMANDS`
variable is set, ``clang-tidy`` will be configured to use the project's
compilation database by default.  The :variable:`CLANG_TIDY_FLAGS` variable
may also be defined to specify additional flags.
#]=======================================================================]

if(NOT CMAKE_TIDY_FLAGS AND CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_TIDY_FLAGS "-p;${CMAKE_CURRENT_BINARY_DIR}")
endif()

set(CLANG_TIDY_FLAGS ${CMAKE_TIDY_FLAGS}
    CACHE STRING "Flags used by clang-tidy during the build")

set(CLANG_TIDY_LANGUAGES "C;CXX;OBJC;OBJCXX")
mark_as_advanced(CLANG_TIDY_LANGUAGES)

find_program(CLANG_TIDY_EXECUTABLE clang-tidy)
mark_as_advanced(CLANG_TIDY_EXECUTABLE)

if(NOT CLANG_TIDY_EXECUTABLE)
  cmake_path(CONVERT "$ENV{PATH}" TO_CMAKE_PATH_LIST CLANG_TIDY_PATH NORMALIZE)
  foreach(path IN LISTS CLANG_TIDY_PATH)
    file(GLOB names LIST_DIRECTORIES false RELATIVE ${path} "${path}/clang-tidy-*")
    list(APPEND CLANG_TIDY_NAMES ${names})
  endforeach()

  list(REMOVE_DUPLICATES CLANG_TIDY_NAMES)
  list(FILTER CLANG_TIDY_NAMES EXCLUDE REGEX [[-diff-|\.py$]])
  list(SORT CLANG_TIDY_NAMES COMPARE NATURAL ORDER DESCENDING)

  find_program(CLANG_TIDY_EXECUTABLE
               NAMES ${CLANG_TIDY_NAMES})
endif()

if(CLANG_TIDY_EXECUTABLE)
  foreach(lang IN LISTS CLANG_TIDY_LANGUAGES)
    set(CMAKE_${lang}_CLANG_TIDY ${CLANG_TIDY_EXECUTABLE})
    if(CLANG_TIDY_FLAGS)
      list(APPEND CMAKE_${lang}_CLANG_TIDY ${CLANG_TIDY_FLAGS})
    endif()
  endforeach()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ClangTidy
                                  REQUIRED_VARS CLANG_TIDY_EXECUTABLE)
