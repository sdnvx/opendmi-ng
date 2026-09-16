#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
project(opendmi-opendmi)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

configure_file(
    ${PROJECT_SOURCE_DIR}/src/config.h.in
    ${PROJECT_SOURCE_DIR}/src/config.h
    USE_SOURCE_PERMISSIONS)

add_executable(opendmi
    src/main.c
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
    src/format/text.c
    src/format/text/handlers.c
    src/format/text/helpers.c
    src/utils/tty.c
)

if(WIN32)
    target_sources(opendmi PRIVATE src/pager/win32.c)
else()
    target_compile_definitions(opendmi PRIVATE _XOPEN_SOURCE _GNU_SOURCE)
    target_sources(opendmi PRIVATE src/pager/posix.c)
endif()

if(ENABLE_JSON)
    target_sources(opendmi PRIVATE src/format/json.c)
endif()

if(ENABLE_XML)
    target_include_directories(opendmi PRIVATE ${LIBXML2_INCLUDE_DIRS})
    target_sources(opendmi
        PRIVATE
            src/format/xml.c
            src/format/xml/handlers.c
            src/format/xml/helpers.c
    )
    target_link_libraries(opendmi PRIVATE ${LIBXML2_LIBRARIES})
endif()

if(ENABLE_YAML)
    target_include_directories(opendmi PRIVATE ${LIBYAML_INCLUDE_DIRS})
    target_sources(opendmi
        PRIVATE
            src/format/yaml.c
            src/format/yaml/handlers.c
            src/format/yaml/helpers.c
    )
    target_link_libraries(opendmi PRIVATE ${LIBYAML_LIBRARIES})
endif()

if(ENABLE_JSON)
    target_include_directories(opendmi PRIVATE ${LIBYAJL_INCLUDE_DIRS})
    target_sources(opendmi
        PRIVATE
            src/format/json.c
            src/format/json/handlers.c
            src/format/json/helpers.c
    )
    target_link_libraries(opendmi PRIVATE ${LIBYAJL_LIBRARIES})
endif()

target_include_directories(opendmi PRIVATE include)
target_link_libraries(opendmi PRIVATE opendmi-shared)

install(TARGETS opendmi RUNTIME DESTINATION bin)
