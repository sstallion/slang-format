# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

include(FixtureTest)

file(WRITE ${FIXTURE_BINARY_DIR}/filelist.txt
     "# comment line\n"
     "\n"
     "${FIXTURE_SOURCE_DIR}/inplace.sv\n")

fixture_run(output --files ${FIXTURE_BINARY_DIR}/filelist.txt)
fixture_expect_output("${output}" ${FIXTURE_SOURCE_DIR}/inplace.sv.expected)
