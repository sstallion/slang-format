# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

#[=======================================================================[.rst:
License
-------

Finds the project license file and extracts copyright information.  When
called from the top-level ``CMakeLists.txt``, the project license file,
copyright, and author are also stored in the :variable:`CMAKE_PROJECT_LICENSE_FILE`,
:variable:`CMAKE_PROJECT_COPYRIGHT`, :variable:`CMAKE_PROJECT_COPYRIGHT_YEAR`,
and :variable:`CMAKE_PROJECT_COPYRIGHT_AUTHOR` variables.

If :variable:`PROJECT_SPDX_LICENSE` is not defined (CMake 4.2 and above), it
will be determined by examining the parent list file for a license expression.

The following variables are defined by this module:

.. variable:: PROJECT_LICENSE_FILE

  Absolute path to the license file for the project.

.. variable:: PROJECT_COPYRIGHT

  Copyright notice for the project.

.. variable:: PROJECT_COPYRIGHT_YEAR

  Copyright year for the project.

.. variable:: PROJECT_COPYRIGHT_AUTHOR

  Copyright author for the project.

.. variable:: PROJECT_SPDX_LICENSE

  SPDX license identifier for the project.
#]=======================================================================]

find_file(PROJECT_LICENSE_FILE
          NAMES LICENSE LICENSE.txt
          PATHS ${CMAKE_CURRENT_SOURCE_DIR}
          NO_CACHE
          NO_DEFAULT_PATH)
set(CMAKE_PROJECT_LICENSE_FILE ${PROJECT_LICENSE_FILE}
    CACHE STATIC "Value Computed by License.cmake" FORCE)

if(PROJECT_LICENSE_FILE)
  block(SCOPE_FOR VARIABLES PROPAGATE PROJECT_COPYRIGHT
                                      PROJECT_COPYRIGHT_YEAR
                                      PROJECT_COPYRIGHT_AUTHOR)
    file(STRINGS ${PROJECT_LICENSE_FILE} lines)
    foreach(line IN LISTS lines)
      if(line MATCHES [[([Cc][Oo][Pp][Yy][Rr][Ii][Gg][Hh][Tt].*)$]])
        string(STRIP ${CMAKE_MATCH_1} PROJECT_COPYRIGHT)
        set(CMAKE_PROJECT_COPYRIGHT ${PROJECT_COPYRIGHT}
            CACHE STATIC "Value Computed by License.cmake" FORCE)

        if(PROJECT_COPYRIGHT MATCHES [[([12][0-9][0-9][0-9][ ,-]?)+(.*)$]])
          string(STRIP ${CMAKE_MATCH_1} PROJECT_COPYRIGHT_YEAR)
          set(CMAKE_PROJECT_COPYRIGHT_YEAR ${PROJECT_COPYRIGHT_YEAR}
              CACHE STATIC "Value Computed by License.cmake" FORCE)

          string(STRIP ${CMAKE_MATCH_2} PROJECT_COPYRIGHT_AUTHOR)
          set(CMAKE_PROJECT_COPYRIGHT_AUTHOR ${PROJECT_COPYRIGHT_AUTHOR}
              CACHE STATIC "Value Computed by License.cmake" FORCE)
        endif()
        break()
      endif()
    endforeach()
  endblock()
endif()

if(NOT PROJECT_SPDX_LICENSE)
  block(SCOPE_FOR VARIABLES PROPAGATE PROJECT_SPDX_LICENSE)
    file(STRINGS ${CMAKE_PARENT_LIST_FILE} _ REGEX [[#[ \t]*SPDX-License-Identifier:[ \t]*(.*)$]])
    set(PROJECT_SPDX_LICENSE ${CMAKE_MATCH_1})
    set(CMAKE_PROJECT_SPDX_LICENSE ${PROJECT_SPDX_LICENSE}
        CACHE STATIC "Value Computed by License.cmake" FORCE)
  endblock()
endif()
