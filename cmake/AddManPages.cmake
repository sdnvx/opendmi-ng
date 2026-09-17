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

        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/man/${SECTION_DIR}")

        set(COMMANDS
            COMMAND ${ASCIIDOCTOR_EXECUTABLE} --backend manpage --out-file ${MANPAGE_PATH} ${SOURCE_PATH}
        )

        # Compressed page replaces the generated one
        if(GZIP_EXECUTABLE)
            list(APPEND COMMANDS COMMAND ${GZIP_EXECUTABLE} --force ${MANPAGE_PATH})
            string(APPEND OUTPUT_PATH ".gz")
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
