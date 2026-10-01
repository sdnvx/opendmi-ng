#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
project(opendmi-opendmi-sysfs)

execute_process(
    COMMAND uname -r
    OUTPUT_VARIABLE KERNEL_RELEASE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(KERNEL_BUILD_DIR "/lib/modules/${KERNEL_RELEASE}/build"
    CACHE PATH "Build directory of the kernel the module is built for")

set(KERNEL_ARCH ""
    CACHE STRING "Architecture the module is built for, e.g. x86_64, if it is not the host one")
set(KERNEL_CROSS_COMPILE ""
    CACHE STRING "Prefix of the toolchain the module is built with, e.g. x86_64-linux-gnu-")

if(NOT EXISTS "${KERNEL_BUILD_DIR}/Makefile")
    message(FATAL_ERROR "Kernel build directory not found: ${KERNEL_BUILD_DIR}")
endif()

find_program(MAKE_EXECUTABLE NAMES gmake make REQUIRED)
cmake_host_system_information(RESULT KBUILD_JOBS QUERY NUMBER_OF_LOGICAL_CORES)

# Module is built by Kbuild of the kernel rather than by CMake. Kbuild puts
# objects next to the sources, so it is given a tree of links to them in the
# build directory instead of the sources themselves.
set(KBUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/kbuild)

set(OPENDMI_SYSFS_COMPAT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/compat)
set(LIBOPENDMI_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libopendmi/include)

# Sources of the library are relative to its directory, unless they are
# added from elsewhere
get_target_property(LIBOPENDMI_SOURCE_DIR opendmi-object SOURCE_DIR)
get_target_property(LIBOPENDMI_SOURCES opendmi-object SOURCES)

# Backends of the library access files, which the kernel has no access to,
# and it has its own backend instead
list(FILTER LIBOPENDMI_SOURCES EXCLUDE REGEX "^(.*/)?src/(backend/linux|utils/file)\\.c$")
list(APPEND LIBOPENDMI_SOURCES
    src/backend/generic.c
    src/backend/linux-kernel.c
)

set(OPENDMI_SYSFS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/compat.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libgcc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
)

foreach(source IN LISTS LIBOPENDMI_SOURCES)
    cmake_path(ABSOLUTE_PATH source BASE_DIRECTORY ${LIBOPENDMI_SOURCE_DIR})
    list(APPEND OPENDMI_SYSFS_SOURCES ${source})
endforeach()

set(OPENDMI_SYSFS_OBJECTS "")
foreach(source IN LISTS OPENDMI_SYSFS_SOURCES)
    file(RELATIVE_PATH path ${CMAKE_SOURCE_DIR} ${source})
    cmake_path(GET path PARENT_PATH directory)

    file(MAKE_DIRECTORY ${KBUILD_DIR}/${directory})
    file(CREATE_LINK ${source} ${KBUILD_DIR}/${path} SYMBOLIC)

    string(REGEX REPLACE "\\.c$" ".o" object ${path})
    string(APPEND OPENDMI_SYSFS_OBJECTS " \\\n    ${object}")
endforeach()

configure_file(Kbuild.in ${KBUILD_DIR}/Kbuild @ONLY)

set(KBUILD_ARGS -C ${KERNEL_BUILD_DIR} M=${KBUILD_DIR})
if(KERNEL_ARCH)
    list(APPEND KBUILD_ARGS ARCH=${KERNEL_ARCH})
endif()
if(KERNEL_CROSS_COMPILE)
    list(APPEND KBUILD_ARGS CROSS_COMPILE=${KERNEL_CROSS_COMPILE})
endif()

# Kbuild tracks dependencies itself, so it is always run
add_custom_target(opendmi-sysfs ALL
    COMMAND ${MAKE_EXECUTABLE} ${KBUILD_ARGS} -j${KBUILD_JOBS} modules
    COMMENT "Building kernel module opendmi-sysfs.ko"
    USES_TERMINAL
    VERBATIM
)

add_custom_target(opendmi-sysfs-clean
    COMMAND ${MAKE_EXECUTABLE} ${KBUILD_ARGS} clean
    COMMENT "Cleaning kernel module opendmi-sysfs.ko"
    VERBATIM
)
