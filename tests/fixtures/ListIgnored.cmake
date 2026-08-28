# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

execute_process(COMMAND ${SLANG_FORMAT} --list-ignored
                        ${FIXTURES_DIR}/ignored.sv
                        ${FIXTURES_DIR}/not_ignored.sv
                RESULT_VARIABLE result
                OUTPUT_VARIABLE output)

if(NOT result EQUAL 0)
  message(FATAL_ERROR "slang-format --list-ignored failed with exit code ${result}")
endif()

string(FIND "${output}" "ignored.sv" pos)
if(pos EQUAL -1)
  message(FATAL_ERROR "list-ignored output missing 'ignored.sv'")
endif()

string(FIND "${output}" "not_ignored.sv" pos)
if(NOT pos EQUAL -1)
  message(FATAL_ERROR "list-ignored output unexpectedly contains 'not_ignored.sv'")
endif()
