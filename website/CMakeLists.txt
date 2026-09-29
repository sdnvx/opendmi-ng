#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
project(opendmi-website)

find_package(Antora REQUIRED)

# Site offers the reference manual as a PDF too, which is built along with it
if(NOT ASCIIDOCTOR_PDF_FOUND)
    message(WARNING "AsciiDoctor-PDF not found, website generation will be disabled")
    return()
endif()

if(NOT ANTORA_FOUND)
    return()
endif()

set(WEBSITE_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/site)
set(WEBSITE_STAMP ${CMAKE_CURRENT_BINARY_DIR}/site.stamp)

# Site is made of its own pages and of the reference manual, which includes the
# manual pages, and offers the PDF of the manual and the latest release the
# changelog records, all of which are gathered while the site is built
file(GLOB_RECURSE WEBSITE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/*.adoc
    ${CMAKE_CURRENT_SOURCE_DIR}/supplemental-ui/*
    ${CMAKE_SOURCE_DIR}/opendmi/man/*.adoc
    ${CMAKE_SOURCE_DIR}/libopendmi/man/*.adoc
    ${CMAKE_SOURCE_DIR}/reference/src/*.adoc
    ${CMAKE_SOURCE_DIR}/reference/themes/*
    ${CMAKE_SOURCE_DIR}/reference/extensions/*
    ${CMAKE_SOURCE_DIR}/reference/fonts/*
    ${CMAKE_SOURCE_DIR}/common/opendmi-*.svg
    ${CMAKE_SOURCE_DIR}/common/favicon.ico
)

add_custom_command(
    OUTPUT ${WEBSITE_STAMP}
    COMMAND ${CMAKE_COMMAND} -E env ASCIIDOCTOR_PDF=${ASCIIDOCTOR_PDF_EXECUTABLE}
        ${ANTORA_EXECUTABLE} --to-dir ${WEBSITE_OUTPUT} antora-playbook.yml
    COMMAND ${CMAKE_COMMAND} -E touch ${WEBSITE_STAMP}
    DEPENDS
        antora-playbook.yml
        antora.yml
        collect.js
        ${CMAKE_SOURCE_DIR}/CHANGELOG.md
        ${CMAKE_SOURCE_DIR}/CONTRIBUTORS.md
        ${CMAKE_SOURCE_DIR}/LICENSE.md
        ${WEBSITE_SOURCES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
)

add_custom_target(opendmi-website ALL DEPENDS ${WEBSITE_STAMP})
