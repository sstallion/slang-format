# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

file(WRITE ${WORK_DIR}/filelist.txt
     "# comment line\n"
     "\n"
     "${FIXTURES_DIR}/inplace.sv\n")

execute_process(COMMAND ${SLANG_FORMAT} --files ${WORK_DIR}/filelist.txt
                RESULT_VARIABLE result
                OUTPUT_VARIABLE output)

if(NOT result EQUAL 0)
  message(FATAL_ERROR "slang-format --files failed with exit code ${result}")
endif()

file(READ ${FIXTURES_DIR}/inplace.sv.expected expected)
if(NOT output STREQUAL expected)
  message(FATAL_ERROR "--files output does not match expected")
endif()
