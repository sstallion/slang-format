# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

include(FixtureTest)

fixture_run(output - INPUT_FILE ${FIXTURE_SOURCE_DIR}/stdin.sv)
fixture_expect_output("${output}" ${FIXTURE_SOURCE_DIR}/stdin.sv.expected)
