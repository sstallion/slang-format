# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

include(FixtureTest)

fixture_run(output - INPUT_FILE ${FIXTURES_DIR}/stdin.sv)
fixture_expect_output("${output}" ${FIXTURES_DIR}/stdin.sv.expected)
