# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

execute_process(COMMAND ${SLANG_FORMAT} -
                INPUT_FILE ${FIXTURES_DIR}/stdin.sv
                RESULT_VARIABLE result
                OUTPUT_VARIABLE output)

if(NOT result EQUAL 0)
  message(FATAL_ERROR "slang-format - failed with exit code ${result}")
endif()

file(READ ${FIXTURES_DIR}/stdin.sv.expected expected)
if(NOT output STREQUAL expected)
  message(FATAL_ERROR "stdin output does not match expected")
endif()
