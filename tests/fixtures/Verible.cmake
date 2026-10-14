# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

execute_process(COMMAND ${SLANG_FORMAT} ${FIXTURES_DIR}/verible.sv
                RESULT_VARIABLE result
                OUTPUT_VARIABLE output)

if(NOT result EQUAL 0)
  message(FATAL_ERROR "slang-format failed with exit code ${result}")
endif()

file(READ ${FIXTURES_DIR}/verible.sv.expected expected)
if(NOT output STREQUAL expected)
  message(FATAL_ERROR "slang-format produced unexpected output: ${output}")
endif()
