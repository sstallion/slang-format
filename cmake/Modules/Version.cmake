# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

#[=======================================================================[.rst:
Version
-------

Determines the project version by examining source control.  Version numbers
are parsed according to Semantic Versioning 2.0.0.  When called from the
top-level ``CMakeLists.txt``, the project version number is also stored in the
:variable:`CMAKE_PROJECT_VERSION` series of variables.

In addition to the standard CMake version variables, the following variables
are defined by this module:

.. variable:: PROJECT_VERSION_PRERELEASE

  Pre-release version component of the :variable:`PROJECT_VERSION` variable.

.. variable:: PROJECT_VERSION_BUILD

  Build metadata component of the :variable:`PROJECT_VERSION` variable.

On Windows platforms, the following variables are defined:

.. variable:: PROJECT_VERSION_WIN32

  Version number expressed as ``<major>.<minor>.<patch>.<tweak>`` for Win32
  compatibility.

.. variable:: PROJECT_VERSION_WIN32_BIN

  Binary version number suitable for embedding in ``VERSIONINFO``resources.
#]=======================================================================]

# Sets PROJECT_VERSION to the output of the specified <command>, executed from
# the current source directory.
macro(_version_eval command)
  execute_process(COMMAND ${command} ${ARGN}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  OUTPUT_VARIABLE PROJECT_VERSION
                  ERROR_QUIET
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
endmacro()

# Attempt to determine version based on the latest annotated tag. If an
# annotated tag is not available, assume the version is 0.0.0 and mimic
# git-describe(1) output.
find_program(GIT_EXECUTABLE git)
mark_as_advanced(GIT_EXECUTABLE)

_version_eval(${GIT_EXECUTABLE} describe)
if(NOT PROJECT_VERSION)
  _version_eval(${GIT_EXECUTABLE} rev-parse --short HEAD)
  if(PROJECT_VERSION)
    string(PREPEND PROJECT_VERSION "-0-")
  endif()
  string(PREPEND PROJECT_VERSION "0.0.0")
endif()

string(REGEX REPLACE [[^v]] "" PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_PROJECT_VERSION ${PROJECT_VERSION}
    CACHE STATIC "Value Computed by Version.cmake")

# Parse PROJECT_VERSION according to Semantic Versioning. Two new variables
# are introduced, PROJECT_VERSION_PRERELEASE and PROJECT_VERSION_BUILD, which
# contain pre-release and build metadata, respectively. Due to limitations in
# CMake's regular expression specification, a more relaxed expression is used.
# See https://semver.org/spec/v2.0.0.html.
if(PROJECT_VERSION MATCHES [[^(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)-?([0-9A-Za-z.-]*)?\+?([0-9A-Za-z.-]*)?$]])
  set(PROJECT_VERSION_MAJOR ${CMAKE_MATCH_1})
  set(CMAKE_PROJECT_VERSION_MAJOR ${PROJECT_VERSION_MAJOR}
      CACHE STATIC "Value Computed by Version.cmake")

  set(PROJECT_VERSION_MINOR ${CMAKE_MATCH_2})
  set(CMAKE_PROJECT_VERSION_MINOR ${PROJECT_VERSION_MINOR}
      CACHE STATIC "Value Computed by Version.cmake")

  set(PROJECT_VERSION_PATCH ${CMAKE_MATCH_3})
  set(CMAKE_PROJECT_VERSION_PATCH ${PROJECT_VERSION_PATCH}
      CACHE STATIC "Value Computed by Version.cmake")

  set(PROJECT_VERSION_PRERELEASE ${CMAKE_MATCH_4})
  set(CMAKE_PROJECT_VERSION_PRERELEASE ${PROJECT_VERSION_PRERELEASE}
      CACHE STATIC "Value Computed by Version.cmake")

  set(PROJECT_VERSION_BUILD ${CMAKE_MATCH_5})
  set(CMAKE_PROJECT_VERSION_BUILD ${PROJECT_VERSION_BUILD}
      CACHE STATIC "Value Computed by Version.cmake")

  # Win32 version information is incompatible with Semantic Versioning.
  # Pre-release and build metadata information is lost, however the number
  # of additional commits can be encoded as the tweak version. See
  # https://learn.microsoft.com/en-us/windows/win32/menurc/version-information.
  if(WIN32)
    if(PROJECT_VERSION_PRERELEASE MATCHES [[^([0-9]+)-]])
      set(PROJECT_VERSION_TWEAK ${CMAKE_MATCH_1})
    else()
      set(PROJECT_VERSION_TWEAK 0)
    endif()
    set(CMAKE_PROJECT_VERSION_TWEAK ${PROJECT_VERSION_TWEAK}
        CACHE STATIC "Value Computed by Version.cmake")

    set(PROJECT_VERSION_WIN32 "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}")
    set(CMAKE_PROJECT_VERSION_WIN32 ${PROJECT_VERSION_WIN32}
        CACHE STATIC "Value Computed by Version.cmake")

    string(REPLACE "." "," PROJECT_VERSION_WIN32_BIN ${PROJECT_VERSION_WIN32})
    set(CMAKE_PROJECT_VERSION_WIN32_BIN ${PROJECT_VERSION_WIN32_BIN}
        CACHE STATIC "Value Computed by Version.cmake")
  endif()
endif()
