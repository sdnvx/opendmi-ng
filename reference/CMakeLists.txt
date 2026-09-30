#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
project(opendmi-reference)

if(ASCIIDOCTOR_PDF_FOUND)
    set(REFERENCE_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/reference.pdf)
    # Book includes the manual pages of the tool and the library
    file(GLOB_RECURSE REFERENCE_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.adoc
        ${CMAKE_SOURCE_DIR}/opendmi/man/*.adoc
        ${CMAKE_SOURCE_DIR}/libopendmi/man/*.adoc
    )

    add_custom_command(
        OUTPUT ${REFERENCE_OUTPUT}
        COMMAND ${ASCIIDOCTOR_PDF_EXECUTABLE}
            --require asciidoctor-pdf
            --require ${CMAKE_CURRENT_SOURCE_DIR}/extensions/opendmi-converter.rb
            --require ${CMAKE_CURRENT_SOURCE_DIR}/extensions/opendmi-manpages.rb
            --backend pdf
            --attribute pdf-themesdir=${CMAKE_CURRENT_SOURCE_DIR}/themes
            --attribute pdf-fontsdir=${CMAKE_CURRENT_SOURCE_DIR}/fonts
            --theme opendmi
            --attribute release-version=${OPENDMI_VERSION}
            --attribute soversion=${LIBOPENDMI_SOVERSION}
            --out-file ${REFERENCE_OUTPUT}
            ${CMAKE_CURRENT_SOURCE_DIR}/src/index.adoc
        DEPENDS
            themes/opendmi-theme.yml
            extensions/opendmi-converter.rb
            extensions/opendmi-manpages.rb
            ${CMAKE_SOURCE_DIR}/LICENSE.md
            ${CMAKE_SOURCE_DIR}/common/opendmi-logo-square-compact.svg
            ${REFERENCE_SOURCES}
    )

    add_custom_target(opendmi-reference ALL DEPENDS ${REFERENCE_OUTPUT})
endif()
