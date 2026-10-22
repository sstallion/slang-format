# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

include(FixtureTest)

fixture_run(output -i -)
fixture_expect_empty("${output}")
