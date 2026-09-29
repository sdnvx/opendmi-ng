#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
message(STATUS "Looking for Antora (optional)...")

# Antora is installed along with the website sources by `npm ci`, which pins
# its version and the versions of the extensions the site is built with, and
# a global installation is used otherwise
find_program(ANTORA_EXECUTABLE antora
    HINTS ${CMAKE_SOURCE_DIR}/website/node_modules/.bin
    OPTIONAL)
mark_as_advanced(ANTORA_EXECUTABLE)

if(NOT ANTORA_EXECUTABLE)
    set(ANTORA_FOUND "NO")
else()
    set(ANTORA_FOUND "YES")
    message(STATUS "Found Antora: ${ANTORA_EXECUTABLE}")
endif()

if(NOT ANTORA_FOUND AND Antora_FIND_REQUIRED)
    message(WARNING "Antora not found, website generation will be disabled")
endif()
