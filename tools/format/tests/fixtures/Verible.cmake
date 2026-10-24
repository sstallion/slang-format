# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

include(FixtureTest)

fixture_run(output ${FIXTURE_SOURCE_DIR}/verible.sv)
fixture_expect_output("${output}" ${FIXTURE_SOURCE_DIR}/verible.sv.expected)
