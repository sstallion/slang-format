# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

include(FixtureTest)

fixture_run(output --no-ignored --list-ignored
            ${FIXTURE_SOURCE_DIR}/ignored.sv
            ${FIXTURE_SOURCE_DIR}/not_ignored.sv)

fixture_expect_empty("${output}")
