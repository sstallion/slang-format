# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

execute_process(COMMAND ${SLANG_FORMAT} --no-config --no-ignored --list-ignored
                        ${FIXTURES_DIR}/ignored.sv
                        ${FIXTURES_DIR}/not_ignored.sv
                RESULT_VARIABLE result
                OUTPUT_VARIABLE output)

if(NOT result EQUAL 0)
  message(FATAL_ERROR "slang-format failed with exit code ${result}")
endif()

if(NOT output STREQUAL "")
  message(FATAL_ERROR "slang-format produced unexpected output: ${output}")
endif()
