# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

file(WRITE ${WORK_DIR}/.slang-format "IndentWidth: 4\n")
file(COPY ${FIXTURES_DIR}/inplace.sv DESTINATION ${WORK_DIR})

execute_process(COMMAND ${SLANG_FORMAT} --no-config ${WORK_DIR}/inplace.sv
                RESULT_VARIABLE result
                OUTPUT_VARIABLE output)

if(NOT result EQUAL 0)
  message(FATAL_ERROR "slang-format failed with exit code ${result}")
endif()

file(READ ${FIXTURES_DIR}/inplace.sv.expected expected)
if(NOT output STREQUAL expected)
  message(FATAL_ERROR "slang-format produced unexpected output: ${output}")
endif()
