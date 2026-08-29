# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

add_test(NAME FilesFlag
         COMMAND ${CMAKE_COMMAND}
                 -DSLANG_FORMAT=$<TARGET_FILE:${PROJECT_NAME}>
                 -DFIXTURES_DIR=${FIXTURES_DIR}
                 -DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}
                 -P ${FIXTURES_DIR}/FilesFlag.cmake)

add_test(NAME FilesPositional
         COMMAND ${CMAKE_COMMAND}
                 -DSLANG_FORMAT=$<TARGET_FILE:${PROJECT_NAME}>
                 -DFIXTURES_DIR=${FIXTURES_DIR}
                 -DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}
                 -P ${FIXTURES_DIR}/FilesPositional.cmake)

add_test(NAME InplaceEdit
         COMMAND ${CMAKE_COMMAND}
                 -DSLANG_FORMAT=$<TARGET_FILE:${PROJECT_NAME}>
                 -DFIXTURES_DIR=${FIXTURES_DIR}
                 -DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}
                 -P ${FIXTURES_DIR}/InplaceEdit.cmake)

add_test(NAME InplaceEditStdin
         COMMAND ${CMAKE_COMMAND}
                 -DSLANG_FORMAT=$<TARGET_FILE:${PROJECT_NAME}>
                 -DFIXTURES_DIR=${FIXTURES_DIR}
                 -DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}
                 -P ${FIXTURES_DIR}/InplaceEditStdin.cmake)

add_test(NAME ListIgnored
         COMMAND ${CMAKE_COMMAND}
                 -DSLANG_FORMAT=$<TARGET_FILE:${PROJECT_NAME}>
                 -DFIXTURES_DIR=${FIXTURES_DIR}
                 -DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}
                 -P ${FIXTURES_DIR}/ListIgnored.cmake)

add_test(NAME Stdin
         COMMAND ${CMAKE_COMMAND}
                 -DSLANG_FORMAT=$<TARGET_FILE:${PROJECT_NAME}>
                 -DFIXTURES_DIR=${FIXTURES_DIR}
                 -DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}
                 -P ${FIXTURES_DIR}/Stdin.cmake)
