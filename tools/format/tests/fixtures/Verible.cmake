# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

include(FixtureTest)

fixture_run(output ${FIXTURES_DIR}/verible.sv)
fixture_expect_output("${output}" ${FIXTURES_DIR}/verible.sv.expected)
