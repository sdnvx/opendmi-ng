#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
cmake_minimum_required(VERSION 3.25)

project(opendmi
        VERSION 0.4.0
        DESCRIPTION "Cross-platform DMI/SMBIOS framework"
        HOMEPAGE_URL https://github.com/sdnvx/opendmi
        LANGUAGES C)

set(OPENDMI_VERSION "${CMAKE_PROJECT_VERSION}")

set(OPENDMI_VERSION_MAJOR "${CMAKE_PROJECT_VERSION_MAJOR}")
set(OPENDMI_VERSION_MINOR "${CMAKE_PROJECT_VERSION_MINOR}")
set(OPENDMI_VERSION_PATCH "${CMAKE_PROJECT_VERSION_PATCH}")

if(NOT OPENDMI_RELEASE_DATE)
    string(TIMESTAMP OPENDMI_RELEASE_DATE "%Y-%m-%d")
endif()

set(ENABLE_GOLANG OFF CACHE BOOL "Enable Go support (libopendmi-go)")
set(ENABLE_PYTHON OFF CACHE BOOL "Enable Python support (libopendmi-python)")
set(ENABLE_RUST OFF CACHE BOOL "Enable Rust support (libopendmi-rust)")
set(ENABLE_DBUS OFF CACHE BOOL "Enable D-bus support (opendmi-dbus)")

set(ENABLE_ICU OFF CACHE BOOL "Build ICU4C support")
set(ENABLE_XML AUTO CACHE BOOL "Build with XML support (libxml)")
set(ENABLE_YAML AUTO CACHE BOOL "Build with YAML support (libyaml)")
set(ENABLE_JSON AUTO CACHE BOOL "Build with JSON support (libyajl)")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

if(MSVC)
    add_compile_options(/std:c11 /W4)
    add_compile_definitions(_CRT_NONSTDC_NO_DEPRECATE)
else()
    add_compile_options(-Wall -Wextra)

    if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" OR
       "${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang")
        add_compile_options(
            -Wno-unused-command-line-argument
            -Wno-gnu-zero-variadic-macro-arguments
            -Wno-keyword-macro
        )
    elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options(
            -Wno-error=clobbered
        )
    endif()
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL "Coverage")
    add_compile_options(-g -fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
else()
    add_compile_options(-O2)
    add_compile_definitions(NDEBUG)
endif()

if(UNIX)
    add_compile_definitions(_REENTRANT _GNU_SOURCE)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
    add_compile_definitions(_NETBSD_SOURCE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
    add_compile_definitions(__BSD_VISIBLE __XPG_VISIBLE=420)
endif()

include(FindPackageEx)

if(APPLE)
    list(PREPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/icu4c")
    LIST(PREPEND CMAKE_PROGRAM_PATH "/opt/homebrew/opt/icu4c/bin/")
endif()

find_program(GZIP_EXECUTABLE NAMES gzip)

find_package(CppCheck)
find_package(AsciiDoctor)
find_package(AsciiDoctorPdf)

find_package_ex(ICU ENABLE_ICU COMPONENTS data uc)
find_package_ex(LibXml2 ENABLE_XML)
find_package_ex(LibYaml ENABLE_YAML)
find_package_ex(LibYajl ENABLE_JSON)

include(FetchCMocka)
enable_testing()

add_subdirectory(libopendmi)
add_subdirectory(opendmi)
add_subdirectory(reference)

if(ENABLE_GOLANG)
    message(STATUS "GoLang support is enabled")
    add_subdirectory(libopendmi-go)
else()
    message(STATUS "GoLang suport is disabled")
endif()

if(ENABLE_PYTHON)
    message(STATUS "Python support is enabled")
    add_subdirectory(libopendmi-python)
else()
    message(STATUS "Python support is disabled")
endif()

if(ENABLE_RUST)
    message(STATUS "Rust support is enabled")
    add_subdirectory(libopendmi-rust)
else()
    message(STATUS "Rust support is disabled")
endif()

if(UNIX AND ENABLE_DBUS)
    message(STATUS "DBus support is enabled")
    add_subdirectory(opendmi-dbus)
else()
    message(STATUS "DBus support is disabled")
endif()

include(CPack)
include(GNUInstallDirs)
