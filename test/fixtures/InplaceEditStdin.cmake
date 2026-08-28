# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

execute_process(COMMAND ${SLANG_FORMAT} -i -
                RESULT_VARIABLE result
                OUTPUT_VARIABLE output)

if(NOT result EQUAL 0)
  message(FATAL_ERROR "slang-format -i - failed with exit code ${result}")
endif()

if(NOT output STREQUAL "")
  message(FATAL_ERROR "slang-format -i - produced unexpected output: ${output}")
endif()
