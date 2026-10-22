# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

include(FixtureTest)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/filelist.txt
     "# comment line\n"
     "\n"
     "${FIXTURES_DIR}/inplace.sv\n")

fixture_run(output @${CMAKE_CURRENT_BINARY_DIR}/filelist.txt)
fixture_expect_output("${output}" ${FIXTURES_DIR}/inplace.sv.expected)
