# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

include(FixtureTest)

fixture_run(output --list-ignored ${FIXTURES_DIR}/ignored.sv ${FIXTURES_DIR}/not_ignored.sv)

string(FIND "${output}" "ignored.sv" pos)
if(pos EQUAL -1)
  message(FATAL_ERROR "Fixture produced unexpected output: ${output}")
endif()

string(FIND "${output}" "not_ignored.sv" pos)
if(NOT pos EQUAL -1)
  message(FATAL_ERROR "Fixture produced unexpected output: ${output}")
endif()
