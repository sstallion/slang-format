# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

file(COPY ${FIXTURES_DIR}/inplace.sv DESTINATION ${WORK_DIR})

execute_process(COMMAND ${SLANG_FORMAT} -i ${WORK_DIR}/inplace.sv
                RESULT_VARIABLE result)

if(NOT result EQUAL 0)
  message(FATAL_ERROR "slang-format -i failed with exit code ${result}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files
                        ${WORK_DIR}/inplace.sv
                        ${FIXTURES_DIR}/inplace.sv.expected
                RESULT_VARIABLE result)

if(NOT result EQUAL 0)
  message(FATAL_ERROR "inplace output does not match expected")
endif()
