# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

include(FixtureTest)

file(COPY ${FIXTURE_SOURCE_DIR}/inplace.sv DESTINATION ${FIXTURE_BINARY_DIR})

fixture_run(output -i ${FIXTURE_BINARY_DIR}/inplace.sv)

fixture_expect_file(${FIXTURE_BINARY_DIR}/inplace.sv
                    ${FIXTURE_SOURCE_DIR}/inplace.sv.expected)
