# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

#[=======================================================================[.rst:
FixtureTest
-----------

# Runs :variable:`FIXTURE_EXECUTABLE` test fixture with :variable:`FIXTURE_FLAGS`,
# failing on a non-zero exit code. ``INPUT_FILE <path>`` feeds standard input.
# The captured standard output is stored in ``output_var``.
#]=======================================================================]

function(fixture_run output_var)
  cmake_parse_arguments(ARG "" "INPUT_FILE" "" ${ARGN})
  set(input)
  if(ARG_INPUT_FILE)
    set(input INPUT_FILE ${ARG_INPUT_FILE})
  endif()

  execute_process(COMMAND ${FIXTURE_EXECUTABLE} ${FIXTURE_FLAGS} ${ARG_UNPARSED_ARGUMENTS}
                  ${input}
                  RESULT_VARIABLE result
                  OUTPUT_VARIABLE output)
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "Fixture failed with exit code ${result}")
  endif()

  set(${output_var} "${output}" PARENT_SCOPE)
endfunction()

# Fails unless <output> equals the contents of <expected_file>.
function(fixture_expect_output output expected_file)
  file(READ ${expected_file} expected)
  if(NOT output STREQUAL expected)
    message(FATAL_ERROR "Fixture produced unexpected output: ${output}")
  endif()
endfunction()

# Fails unless <output> is empty.
function(fixture_expect_empty output)
  if(NOT output STREQUAL "")
    message(FATAL_ERROR "Fixture produced unexpected output: ${output}")
  endif()
endfunction()

# Fails unless the files <actual> and <expected> are identical.
function(fixture_expect_file actual expected)
  execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${actual} ${expected}
                  RESULT_VARIABLE result)
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "Fixture produced unexpected output in ${actual}")
  endif()
endfunction()
