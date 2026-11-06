# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

include(FixtureTest)

file(WRITE ${FIXTURE_BINARY_DIR}/.slang-format "IndentWidth: 4\n")
file(COPY ${FIXTURE_SOURCE_DIR}/inplace.sv DESTINATION ${FIXTURE_BINARY_DIR})

fixture_run(output ${FIXTURE_BINARY_DIR}/inplace.sv)
fixture_expect_output("${output}" ${FIXTURE_SOURCE_DIR}/inplace.sv.expected)
