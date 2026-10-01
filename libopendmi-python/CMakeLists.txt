#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
project(opendmi-libopendmi-python)

find_package(Python3 COMPONENTS Interpreter Development.Module REQUIRED)

# Extension module importable as `opendmi`, e.g. opendmi.cpython-314-darwin.so;
# Python3_add_library() links Python3::Module and drops the `lib` prefix
Python3_add_library(opendmi-python MODULE WITH_SOABI)
set_target_properties(opendmi-python PROPERTIES OUTPUT_NAME opendmi)

target_include_directories(opendmi-python PRIVATE include)
target_link_libraries(opendmi-python PRIVATE opendmi-shared)
target_sources(opendmi-python
    PRIVATE
        src/module.c
        src/context.c
)
