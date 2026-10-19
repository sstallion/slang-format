# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

include(FixtureTest)

file(COPY ${FIXTURES_DIR}/inplace.sv DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

fixture_run(output -i ${CMAKE_CURRENT_BINARY_DIR}/inplace.sv)
fixture_expect_file(${CMAKE_CURRENT_BINARY_DIR}/inplace.sv ${FIXTURES_DIR}/inplace.sv.expected)
