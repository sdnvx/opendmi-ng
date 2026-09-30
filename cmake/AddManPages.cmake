#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#

#
# Add manual pages built from AsciiDoc sources.
#
#   add_manpages(<target> <source dir>)
#
# Sources are looked up recursively in <source dir>, and each manual page is
# installed to the subdirectory of its source, e.g. `man1` or `man3`. Manual
# pages are compressed if gzip is available. Nothing is done if AsciiDoctor
# is not found.
#
# A manual page describing several functions or types is installed under the
# name of each: its other names are symbolic links to its source, which are
# installed as symbolic links to the page. Links are used rather than `.so`
# requests, which some readers do not look up in compressed pages.
#
function(add_manpages TARGET_NAME SOURCE_DIR)
    if(NOT ASCIIDOCTOR_FOUND)
        return()
    endif()

    file(GLOB_RECURSE MANPAGE_SOURCES RELATIVE ${SOURCE_DIR} ${SOURCE_DIR}/*.adoc)

    set(MANPAGES)

    foreach(SOURCE ${MANPAGE_SOURCES})
        cmake_path(REMOVE_EXTENSION SOURCE LAST_ONLY OUTPUT_VARIABLE MANPAGE)
        cmake_path(GET SOURCE PARENT_PATH SECTION_DIR)

        set(SOURCE_PATH "${SOURCE_DIR}/${SOURCE}")
        set(MANPAGE_PATH "${CMAKE_CURRENT_BINARY_DIR}/man/${MANPAGE}")
        set(OUTPUT_PATH "${MANPAGE_PATH}")
        set(OUTPUT_EXTENSION "")

        # Compressed page replaces the generated one
        if(GZIP_EXECUTABLE)
            set(OUTPUT_EXTENSION ".gz")
            string(APPEND OUTPUT_PATH "${OUTPUT_EXTENSION}")
        endif()

        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/man/${SECTION_DIR}")

        # Other name of a page links to the page it names, which is built from
        # the source the link leads to, in the same section
        if(IS_SYMLINK "${SOURCE_PATH}")
            file(REAL_PATH "${SOURCE_PATH}" TARGET_PATH)
            file(REAL_PATH "${SOURCE_DIR}/${SECTION_DIR}" SECTION_PATH)
            cmake_path(GET TARGET_PATH PARENT_PATH TARGET_DIR)
            cmake_path(GET TARGET_PATH STEM LAST_ONLY TARGET_PAGE)

            if(NOT TARGET_DIR STREQUAL SECTION_PATH)
                message(FATAL_ERROR "Manual page ${SOURCE} links out of its section")
            endif()

            file(CREATE_LINK "${TARGET_PAGE}${OUTPUT_EXTENSION}" "${OUTPUT_PATH}" SYMBOLIC)
            install(FILES ${OUTPUT_PATH} DESTINATION ${CMAKE_INSTALL_MANDIR}/${SECTION_DIR})

            continue()
        endif()

        # AsciiDoctor writes a `.so` page next to the page it builds for every
        # other name the page gives, which would replace the pages built under
        # these names, so each page is built in a directory of its own, under
        # a name no manual page has, and copied from there
        set(BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/man-build/${MANPAGE}")
        set(BUILD_PATH "${BUILD_DIR}/manpage")

        file(MAKE_DIRECTORY "${BUILD_DIR}")

        set(COMMANDS
            COMMAND ${ASCIIDOCTOR_EXECUTABLE} --backend manpage
                --attribute release-version=${OPENDMI_VERSION}
                --out-file ${BUILD_PATH} ${SOURCE_PATH}
            COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_PATH} ${MANPAGE_PATH}
        )

        if(GZIP_EXECUTABLE)
            list(APPEND COMMANDS COMMAND ${GZIP_EXECUTABLE} --force ${MANPAGE_PATH})
        endif()

        add_custom_command(
            OUTPUT ${OUTPUT_PATH}
            ${COMMANDS}
            DEPENDS ${SOURCE_PATH}
            VERBATIM
        )

        install(FILES ${OUTPUT_PATH} DESTINATION ${CMAKE_INSTALL_MANDIR}/${SECTION_DIR})

        list(APPEND MANPAGES ${OUTPUT_PATH})
    endforeach()

    add_custom_target(${TARGET_NAME} ALL DEPENDS ${MANPAGES})
endfunction()
