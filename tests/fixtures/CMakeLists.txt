# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

foreach(fixture IN ITEMS FilesFlag FilesPositional InplaceEdit InplaceEditStdin
                         ListIgnored NoConfig NoIgnore Stdin Verible)
  add_test(NAME ${fixture}
           COMMAND ${CMAKE_COMMAND}
                   "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}"
                   "-DFIXTURE_EXECUTABLE=$<TARGET_FILE:${PROJECT_NAME}>"
                   "-DFIXTURE_FLAGS=--no-config"
                   "-DFIXTURES_DIR=${FIXTURES_DIR}"
                   -P "${FIXTURES_DIR}/${fixture}.cmake")
endforeach()
