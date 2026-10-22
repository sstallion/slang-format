# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

include(FixtureTest)

fixture_run(output --no-ignored --list-ignored ${FIXTURES_DIR}/ignored.sv ${FIXTURES_DIR}/not_ignored.sv)
fixture_expect_empty("${output}")
