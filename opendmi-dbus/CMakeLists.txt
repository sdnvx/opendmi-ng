#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
project(opendmi-opendmi-dbus)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

# Generated into the build directory, so that builds with different options
# do not overwrite the configuration of each other
configure_file(
    ${PROJECT_SOURCE_DIR}/src/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
    USE_SOURCE_PERMISSIONS
)

add_executable(opendmi-dbus
    src/main.c
    src/pidfile.c
)

target_include_directories(opendmi-dbus PRIVATE include ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(opendmi-dbus PRIVATE opendmi-shared)

install(TARGETS opendmi-dbus RUNTIME DESTINATION bin)
