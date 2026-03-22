# Copyright 2026 Steven Stallion
# SPDX-License-Identifier: MIT

include(FetchContent)

# Preserve normal variables in third-party projects:
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

block()
  FetchContent_Declare(
    slang
    GIT_REPOSITORY https://github.com/MikePopoloski/slang.git
    GIT_TAG ace09c5d7c9f4e28eed654d2f353c6dc792ebf67 # v10.0
    SYSTEM
  )
  FetchContent_MakeAvailable(slang)
endblock()

block()
  set(YAML_CPP_BUILD_CONTRIB OFF)
  set(YAML_CPP_BUILD_TOOLS OFF)

  FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG 56e3bb550c91fd7005566f19c079cb7a503223cf # yaml-cpp-0.9.0
    SYSTEM
  )
  FetchContent_MakeAvailable(yaml-cpp)
endblock()

block()
  set(BUILD_GMOCK OFF)
  set(INSTALL_GTEST OFF)

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG 52eb8108c5bdec04579160ae17225d66034bd723 # v1.17.0
    EXCLUDE_FROM_ALL
    SYSTEM
  )
  FetchContent_MakeAvailable(googletest)
endblock()
