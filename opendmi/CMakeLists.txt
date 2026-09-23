#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
project(opendmi-opendmi)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

if(Curses_FOUND AND NOT TARGET Curses::Curses)
    add_library(Curses::Curses INTERFACE IMPORTED)
    set_target_properties(
        Curses::Curses
        PROPERTIES
            INTERFACE_LINK_LIBRARIES "${CURSES_LIBRARIES}"
            INTERFACE_INCLUDE_DIRECTORIES "${CURSES_INCLUDE_DIRS}"
    )
endif()

# Generated into the build directory, so that builds with different options
# do not overwrite the configuration of each other
configure_file(
    ${PROJECT_SOURCE_DIR}/src/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
    USE_SOURCE_PERMISSIONS
)

#
# Command line tool code is built as an object library, so that it can be
# linked into both the tool itself and its tests.
#
add_library(opendmi-cli OBJECT
    src/option.c
    src/command.c
    src/command/common.c
    src/command/dump.c
    src/command/entry.c
    src/command/list.c
    src/command/modules.c
    src/command/show.c
    src/command/explain.c
    src/command/lint.c
    src/command/export.c
    src/command/import.c
    src/command/types.c
    src/format.c
    src/format/iter.c
    src/format/text.c
    src/format/text/handlers.c
    src/format/text/helpers.c
    src/utils/locale.c
    src/utils/tty.c
    src/utils/utf8.c
)

if(WIN32)
    target_sources(opendmi-cli PRIVATE src/pager/win32.c)
else()
    target_compile_definitions(opendmi-cli PUBLIC _XOPEN_SOURCE _GNU_SOURCE)
    target_sources(opendmi-cli PRIVATE src/pager/posix.c)
endif()

if(ENABLE_XML)
    target_include_directories(opendmi-cli PUBLIC ${LIBXML2_INCLUDE_DIRS})
    target_sources(opendmi-cli
        PRIVATE
            src/format/xml.c
            src/format/xml/handlers.c
            src/format/xml/helpers.c
    )
    target_link_libraries(opendmi-cli PUBLIC ${LIBXML2_LIBRARIES})
endif()

if(ENABLE_YAML)
    target_include_directories(opendmi-cli PUBLIC ${LIBYAML_INCLUDE_DIRS})
    target_sources(opendmi-cli
        PRIVATE
            src/format/yaml.c
            src/format/yaml/handlers.c
            src/format/yaml/helpers.c
    )
    target_link_libraries(opendmi-cli PUBLIC ${LIBYAML_LIBRARIES})
endif()

if(ENABLE_JSON)
    target_include_directories(opendmi-cli PUBLIC ${LIBYAJL_INCLUDE_DIRS})
    target_sources(opendmi-cli
        PRIVATE
            src/format/json.c
            src/format/json/handlers.c
            src/format/json/helpers.c
    )
    target_link_libraries(opendmi-cli PUBLIC ${LIBYAJL_LIBRARIES})
endif()

target_include_directories(opendmi-cli PUBLIC include ${CMAKE_CURRENT_BINARY_DIR})
if(ICU_FOUND)
    add_resources(opendmi-cli-resources opendmi ${CMAKE_CURRENT_SOURCE_DIR}/locale OPENDMI_RESOURCE_DATA)

    add_dependencies(opendmi-cli opendmi-cli-resources)
endif()

target_link_libraries(opendmi-cli PUBLIC opendmi-shared)

if(ENABLE_CURSES)
    target_link_libraries(opendmi-cli PUBLIC Curses::Curses)
endif()

if(ENABLE_COVERAGE)
    target_compile_options(opendmi-cli PRIVATE ${COVERAGE_OPTIONS})
    target_link_options(opendmi-cli PUBLIC ${COVERAGE_OPTIONS})
endif()

add_executable(opendmi src/main.c)
target_link_libraries(opendmi PRIVATE opendmi-cli)

# Resource data is a plain object file, and `opendmi-cli` is an object library,
# so it is passed to the linker of every program which uses the tool sources
if(ICU_FOUND)
    target_link_libraries(opendmi PRIVATE ${OPENDMI_RESOURCE_DATA})
endif()

# Windows has no rpath, so the library DLL is copied next to the executable to
# make it runnable from the build directory
if(WIN32)
    add_custom_command(TARGET opendmi POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:opendmi> $<TARGET_FILE_DIR:opendmi>
        COMMAND_EXPAND_LISTS
    )
endif()

install(TARGETS opendmi RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_manpages(opendmi-cli-manuals ${CMAKE_CURRENT_SOURCE_DIR}/man)

#
# Add test for command line tool.
#
#   opendmi_cli_test(<name> SOURCES <sources>... [LINK_CLI])
#
# The test is registered as `opendmi/<name>`, and its executable target is
# named `opendmi-<name>-test` with slashes replaced by dashes. SOURCES are test
# sources, optionally including tested command line tool sources. LINK_CLI
# links the whole command line tool code (except `main()`) into the test.
#
function(opendmi_cli_test TEST_NAME)
    cmake_parse_arguments(PARSE_ARGV 1 TEST "LINK_CLI" "" "SOURCES")

    set(TEST_FULL_NAME "opendmi/${TEST_NAME}")

    string(REPLACE "/" "-" TEST_TARGET "${TEST_FULL_NAME}")
    string(APPEND TEST_TARGET "-test")

    add_executable(${TEST_TARGET} ${TEST_SOURCES} test/helpers.c)
    target_compile_definitions(${TEST_TARGET} PRIVATE OPENDMI_TEST_DATA="${PROJECT_SOURCE_DIR}/../data")
    target_link_libraries(${TEST_TARGET} PRIVATE cmocka-static)

    # Tests which build the tool sources themselves need its resources as well
    if(ICU_FOUND)
        add_dependencies(${TEST_TARGET} opendmi-cli-resources)
        target_link_libraries(${TEST_TARGET} PRIVATE ${OPENDMI_RESOURCE_DATA})
    endif()

    if(TEST_LINK_CLI)
        target_link_libraries(${TEST_TARGET} PRIVATE opendmi-cli)
    else()
        target_include_directories(${TEST_TARGET} PRIVATE include ${CMAKE_CURRENT_BINARY_DIR})
        target_link_libraries(${TEST_TARGET} PRIVATE opendmi-shared)

        if(NOT WIN32)
            target_compile_definitions(${TEST_TARGET} PRIVATE _XOPEN_SOURCE _GNU_SOURCE)
        endif()

        if(ENABLE_CURSES)
            target_link_libraries(${TEST_TARGET} PRIVATE Curses::Curses)
        endif()
    endif()

    if(ENABLE_COVERAGE)
        target_compile_options(${TEST_TARGET} PUBLIC ${COVERAGE_OPTIONS})
        target_link_options(${TEST_TARGET} PUBLIC ${COVERAGE_OPTIONS})
    endif()

    add_test(
        NAME ${TEST_FULL_NAME}
        COMMAND ${TEST_TARGET}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    set_tests_properties(${TEST_FULL_NAME} PROPERTIES
        SKIP_RETURN_CODE 69
        ENVIRONMENT "LANG=en_US.UTF-8;LC_ALL=C"
    )

    # Windows has no rpath, so the library DLL is looked up via PATH
    if(WIN32)
        set_tests_properties(${TEST_FULL_NAME} PROPERTIES
            ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:opendmi-shared>"
        )
    endif()

    if(ENABLE_COVERAGE)
        set_tests_properties(${TEST_FULL_NAME} PROPERTIES
            ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_CURRENT_BINARY_DIR}/bin/${TEST_TARGET}.profraw"
        )
    endif()
endfunction()

# Option parser test uses its own stub of `dmi_command_message()`
opendmi_cli_test(option SOURCES test/option.c src/option.c src/utils/tty.c src/utils/locale.c)
opendmi_cli_test(utf8 SOURCES test/utf8.c src/utils/utf8.c)
opendmi_cli_test(command SOURCES test/command.c LINK_CLI)
opendmi_cli_test(format SOURCES test/format.c LINK_CLI)
opendmi_cli_test(iter SOURCES test/iter.c src/format/iter.c)
