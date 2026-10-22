# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

include(FixtureTest)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/.slang-format "IndentWidth: 4\n")
file(COPY ${FIXTURES_DIR}/inplace.sv DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

fixture_run(output ${CMAKE_CURRENT_BINARY_DIR}/inplace.sv)
fixture_expect_output("${output}" ${FIXTURES_DIR}/inplace.sv.expected)
