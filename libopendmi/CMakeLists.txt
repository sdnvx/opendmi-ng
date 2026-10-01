#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
project(opendmi-libopendmi)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

add_library(opendmi-object OBJECT)
add_library(opendmi-static STATIC)
add_library(opendmi-shared SHARED)

if(ICU_FOUND)
    add_resources(opendmi-resources libopendmi ${CMAKE_CURRENT_SOURCE_DIR}/locale LIBOPENDMI_RESOURCE_DATA)

    add_dependencies(opendmi-object opendmi-resources)

    target_include_directories(opendmi-object PRIVATE ${ICU_INCLUDE_DIRS})
    target_sources(opendmi-static PRIVATE ${LIBOPENDMI_RESOURCE_DATA})
    target_sources(opendmi-shared PRIVATE ${LIBOPENDMI_RESOURCE_DATA})

    # Plural rules of message formatting live in the internationalization
    # component, the rest of the resource handling in the common one
    list(APPEND LIBOPENDMI_LIBRARIES ICU::uc ICU::i18n)
    list(APPEND LIBOPENDMI_PC_REQUIRES icu-uc icu-i18n)
endif()

# Generated into the build directory, so that builds with different options
# do not overwrite the configuration of each other
configure_file(
    ${PROJECT_SOURCE_DIR}/src/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
    USE_SOURCE_PERMISSIONS
)

target_include_directories(opendmi-object
    PUBLIC include
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

# Public headers are taken by directories, and the headers of a backend are
# added along with its sources. Headers the library uses itself but does not
# install, and C++ headers, are left out.
file(GLOB LIBOPENDMI_HEADERS CONFIGURE_DEPENDS
    include/opendmi/*.h
    include/opendmi/entity/*.h
    include/opendmi/entity/*/*.h
    include/opendmi/lint/*.h
    include/opendmi/module/*.h
    include/opendmi/utils/*.h
)
list(REMOVE_ITEM LIBOPENDMI_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/opendmi/internal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/opendmi/utils/win32.h
)
list(APPEND LIBOPENDMI_HEADERS include/opendmi/backend/dump.h)

# Sources are taken by directories, and the ones of the backends, which depend
# on the platform, are added by name
file(GLOB LIBOPENDMI_SOURCES CONFIGURE_DEPENDS
    src/*.c
    src/entity/*.c
    src/entity/*/*.c
    src/lint/*.c
    src/module/*.c
    src/utils/*.c
)
list(REMOVE_ITEM LIBOPENDMI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/win32.c)

target_sources(opendmi-object
    PRIVATE
        ${LIBOPENDMI_SOURCES}
        src/backend/dump.c
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(opendmi-object
        PRIVATE
            src/backend/linux.c
    )

    list(APPEND LIBOPENDMI_HEADERS include/opendmi/backend/linux.h)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    find_library(COREFOUNDATION_LIB CoreFoundation REQUIRED)
    find_library(IOKIT_LIB IOKit REQUIRED)

    # Frameworks are located in SDK, which path may differ on target systems
    list(APPEND LIBOPENDMI_LIBRARIES
        $<BUILD_INTERFACE:${COREFOUNDATION_LIB}>
        "$<INSTALL_INTERFACE:-framework CoreFoundation>"
        $<BUILD_INTERFACE:${IOKIT_LIB}>
        "$<INSTALL_INTERFACE:-framework IOKit>"
    )
    list(APPEND LIBOPENDMI_PC_LIBS "-framework CoreFoundation" "-framework IOKit")

    target_sources(opendmi-object
        PRIVATE
            src/backend/darwin.c
    )

    list(APPEND LIBOPENDMI_HEADERS include/opendmi/backend/darwin.h)
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    target_sources(opendmi-object
        PRIVATE
            src/backend/generic.c
            src/backend/freebsd.c
    )

    list(APPEND LIBOPENDMI_HEADERS
        include/opendmi/backend/generic.h
        include/opendmi/backend/freebsd.h
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
    target_sources(opendmi-object
        PRIVATE
            src/backend/generic.c
            src/backend/netbsd.c
    )

    list(APPEND LIBOPENDMI_HEADERS
        include/opendmi/backend/generic.h
        include/opendmi/backend/netbsd.h
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND LIBOPENDMI_LIBRARIES ntdll)
    list(APPEND LIBOPENDMI_PC_CFLAGS -DDMI_STATIC)

    target_sources(opendmi-object
        PRIVATE
            src/backend/windows.c
            src/utils/win32.c
    )

    list(APPEND LIBOPENDMI_HEADERS include/opendmi/backend/windows.h)
endif()

if(NOT MSVC)
    target_compile_options(opendmi-object PRIVATE -fPIC)
endif()
if(ENABLE_COVERAGE)
    target_compile_options(opendmi-object PUBLIC ${COVERAGE_OPTIONS})
endif()

# Only public API symbols, marked with __dmi_api, are exported
target_compile_definitions(opendmi-object PRIVATE DMI_BUILD)
set_target_properties(opendmi-object PROPERTIES C_VISIBILITY_PRESET hidden)

target_sources(opendmi-static
    PUBLIC FILE_SET HEADERS
        BASE_DIRS include
        FILES ${LIBOPENDMI_HEADERS}
)
target_compile_definitions(opendmi-static INTERFACE DMI_STATIC)
target_sources(opendmi-static PRIVATE $<TARGET_OBJECTS:opendmi-object>)
target_link_libraries(opendmi-static PUBLIC ${LIBOPENDMI_LIBRARIES})
set_target_properties(opendmi-static PROPERTIES
    OUTPUT_NAME opendmi
    EXPORT_NAME static
)

# MSVC names both static and DLL import libraries `opendmi.lib`
if(MSVC)
    set_target_properties(opendmi-static PROPERTIES OUTPUT_NAME opendmi-static)
endif()

target_sources(opendmi-shared
    PUBLIC FILE_SET HEADERS
        BASE_DIRS include
        FILES ${LIBOPENDMI_HEADERS}
)
target_sources(opendmi-shared PRIVATE $<TARGET_OBJECTS:opendmi-object>)
target_link_libraries(opendmi-shared PUBLIC ${LIBOPENDMI_LIBRARIES})
if(ENABLE_COVERAGE)
    target_link_options(opendmi-shared PUBLIC $<BUILD_INTERFACE:${COVERAGE_OPTIONS}>)
endif()
set_target_properties(opendmi-shared PROPERTIES
    OUTPUT_NAME opendmi
    EXPORT_NAME shared
    VERSION ${OPENDMI_VERSION}
    SOVERSION ${LIBOPENDMI_SOVERSION}
)

# The same target names are available when the project is used as a
# subdirectory and when it is installed
add_library(OpenDMI::static ALIAS opendmi-static)
add_library(OpenDMI::shared ALIAS opendmi-shared)

install(TARGETS opendmi-static opendmi-shared
    EXPORT OpenDMITargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

#
# CMake package configuration: find_package(OpenDMI)
#
include(CMakePackageConfigHelpers)

set(LIBOPENDMI_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/OpenDMI)

install(EXPORT OpenDMITargets
    NAMESPACE OpenDMI::
    DESTINATION ${LIBOPENDMI_CMAKE_DIR}
)

if(ICU_FOUND)
    set(LIBOPENDMI_WITH_ICU ON)
else()
    set(LIBOPENDMI_WITH_ICU OFF)
endif()

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/../cmake/OpenDMIConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/OpenDMIConfig.cmake
    INSTALL_DESTINATION ${LIBOPENDMI_CMAKE_DIR}
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/OpenDMIConfigVersion.cmake
    VERSION ${OPENDMI_VERSION}
    COMPATIBILITY ${LIBOPENDMI_COMPATIBILITY}
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/OpenDMIConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/OpenDMIConfigVersion.cmake
    DESTINATION ${LIBOPENDMI_CMAKE_DIR}
)

#
# pkg-config file: pkg-config opendmi
#
# Installation prefix is determined relative to the file location, so the
# prefix can be changed at install time
if(IS_ABSOLUTE "${CMAKE_INSTALL_LIBDIR}")
    set(LIBOPENDMI_PC_PREFIX "${CMAKE_INSTALL_PREFIX}")
    set(LIBOPENDMI_PC_LIBDIR "${CMAKE_INSTALL_LIBDIR}")
else()
    # One step up for every component of the directory holding the file
    string(REPLACE "/" ";" LIBOPENDMI_PC_COMPONENTS "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

    set(LIBOPENDMI_PC_RELATIVE "")
    foreach(COMPONENT ${LIBOPENDMI_PC_COMPONENTS})
        if((COMPONENT STREQUAL "") OR (COMPONENT STREQUAL "."))
            continue()
        endif()

        if(LIBOPENDMI_PC_RELATIVE STREQUAL "")
            set(LIBOPENDMI_PC_RELATIVE "..")
        else()
            string(APPEND LIBOPENDMI_PC_RELATIVE "/..")
        endif()
    endforeach()

    set(LIBOPENDMI_PC_PREFIX "\${pcfiledir}/${LIBOPENDMI_PC_RELATIVE}")
    set(LIBOPENDMI_PC_LIBDIR "\${prefix}/${CMAKE_INSTALL_LIBDIR}")
endif()

if(IS_ABSOLUTE "${CMAKE_INSTALL_INCLUDEDIR}")
    set(LIBOPENDMI_PC_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}")
else()
    set(LIBOPENDMI_PC_INCLUDEDIR "\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}")
endif()

# Imported targets are provided by packages listed in LIBOPENDMI_PC_REQUIRES,
# and libraries specified with generator expressions are added explicitly
foreach(LIBRARY ${LIBOPENDMI_LIBRARIES})
    if((LIBRARY MATCHES "::") OR (LIBRARY MATCHES "^\\$<"))
        continue()
    elseif(LIBRARY MATCHES "^-")
        list(APPEND LIBOPENDMI_PC_LIBS "${LIBRARY}")
    else()
        list(APPEND LIBOPENDMI_PC_LIBS "-l${LIBRARY}")
    endif()
endforeach()

list(JOIN LIBOPENDMI_PC_REQUIRES ", " LIBOPENDMI_PC_REQUIRES)
list(JOIN LIBOPENDMI_PC_CFLAGS " " LIBOPENDMI_PC_CFLAGS)
list(JOIN LIBOPENDMI_PC_LIBS " " LIBOPENDMI_PC_LIBS)

configure_file(
    ${PROJECT_SOURCE_DIR}/../cmake/opendmi.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/opendmi.pc
    @ONLY
)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/opendmi.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

#
# Add library test.
#
#   opendmi_test(<name> SOURCES <sources>... [LIBRARY <target>])
#
# The test is registered as `<name>`, and its executable target is named
# `<name>-test` with slashes replaced by dashes. The test is linked with the
# shared library by default, or with the library target given in LIBRARY
# (e.g. `opendmi-static`), and is run with the arguments given in ARGS.
#
function(opendmi_test TEST_NAME)
    cmake_parse_arguments(PARSE_ARGV 1 TEST "" "LIBRARY" "SOURCES;ARGS")

    string(REPLACE "/" "-" TEST_TARGET "${TEST_NAME}")
    string(APPEND TEST_TARGET "-test")

    if(NOT TEST_LIBRARY)
        set(TEST_LIBRARY opendmi-shared)
    endif()

    add_executable(${TEST_TARGET} ${TEST_SOURCES} test/logger.c)
    target_compile_definitions(${TEST_TARGET} PRIVATE OPENDMI_TEST_DATA="${PROJECT_SOURCE_DIR}/../data")
    target_link_libraries(${TEST_TARGET} ${TEST_LIBRARY} cmocka-static)

    if(ENABLE_COVERAGE)
        target_compile_options(${TEST_TARGET} PUBLIC ${COVERAGE_OPTIONS})
        target_link_options(${TEST_TARGET} PUBLIC ${COVERAGE_OPTIONS})
    endif()

    add_test(
        NAME ${TEST_NAME}
        COMMAND ${TEST_TARGET} ${TEST_ARGS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    set_tests_properties(${TEST_NAME} PROPERTIES
        SKIP_RETURN_CODE 69
        ENVIRONMENT "LANG=en_US.UTF-8;LC_ALL=C"
    )

    if(ENABLE_COVERAGE)
        set_tests_properties(${TEST_NAME} PROPERTIES
            ENVIRONMENT_MODIFICATION "LLVM_PROFILE_FILE=set:${CMAKE_CURRENT_BINARY_DIR}/bin/${TEST_TARGET}.profraw"
        )
    endif()
endfunction()

opendmi_test(attribute SOURCES test/attribute.c)
opendmi_test(error SOURCES test/error.c)
opendmi_test(anonymize SOURCES test/anonymize.c)
opendmi_test(context SOURCES test/context.c)
opendmi_test(entity SOURCES test/entity.c)
opendmi_test(entry SOURCES test/entry.c)
opendmi_test(module SOURCES test/module.c)
opendmi_test(module/static SOURCES test/module.c LIBRARY opendmi-static)
opendmi_test(platform SOURCES test/platform.c)
opendmi_test(registry SOURCES test/registry.c)
opendmi_test(dump SOURCES test/dump.c)
opendmi_test(locale SOURCES test/locale.c)
opendmi_test(lint SOURCES test/lint.c)
opendmi_test(buffer SOURCES test/buffer.c)
opendmi_test(reader SOURCES test/reader.c)
opendmi_test(writer SOURCES test/writer.c)
opendmi_test(decoder SOURCES test/decoder.c)
opendmi_test(field SOURCES test/field.c)

# Dumps of the corpus, which the encoder test writes back structure by
# structure
file(GLOB_RECURSE OPENDMI_TEST_DUMPS "${PROJECT_SOURCE_DIR}/../data/*.bin")
list(SORT OPENDMI_TEST_DUMPS)

opendmi_test(encoder SOURCES test/encoder.c ARGS ${OPENDMI_TEST_DUMPS})

opendmi_test(utils/endian SOURCES test/utils/endian.c)
opendmi_test(utils/codec SOURCES test/utils/codec.c)
opendmi_test(utils/string SOURCES test/utils/string.c)
opendmi_test(utils/vector SOURCES test/utils/vector.c)
opendmi_test(utils/name SOURCES test/utils/name.c)
opendmi_test(utils/version SOURCES test/utils/version.c)
opendmi_test(utils/version-cxx SOURCES test/utils/version.cc)
opendmi_test(utils/datetime SOURCES test/utils/datetime.c)
opendmi_test(utils/datetime-cxx SOURCES test/utils/datetime.cc)
opendmi_test(utils/uuid SOURCES test/utils/uuid.c)
opendmi_test(utils/uuid-cxx SOURCES test/utils/uuid.cc)
opendmi_test(utils/misc SOURCES test/utils/misc.c)
opendmi_test(utils/file SOURCES test/utils/file.c)

opendmi_test(entity/common SOURCES test/entity/common.c)
opendmi_test(entity/baseboard SOURCES test/entity/baseboard.c)
opendmi_test(entity/chassis SOURCES test/entity/chassis.c)
opendmi_test(entity/memory-controller SOURCES test/entity/memory-controller.c)
opendmi_test(entity/firmware SOURCES test/entity/firmware.c)
opendmi_test(entity/cache SOURCES test/entity/cache.c)
opendmi_test(entity/processor SOURCES test/entity/processor.c)
opendmi_test(entity/slot SOURCES test/entity/slot.c)
opendmi_test(entity/onboard-device SOURCES test/entity/onboard-device.c)
opendmi_test(entity/memory-array SOURCES test/entity/memory-array.c)
opendmi_test(entity/memory-device SOURCES test/entity/memory-device.c)
opendmi_test(entity/memory-error SOURCES test/entity/memory-error.c)
opendmi_test(entity/memory-channel SOURCES test/entity/memory-channel.c)
opendmi_test(entity/ipmi-device SOURCES test/entity/ipmi-device.c)
opendmi_test(entity/firmware-inventory SOURCES test/entity/firmware-inventory.c)
opendmi_test(entity/names SOURCES test/entity/names.c)
opendmi_test(entity/mgmt-controller SOURCES test/entity/mgmt-controller.c)
opendmi_test(entity/hpe/modern SOURCES test/entity/hpe/modern.c)
opendmi_test(entity/hpe/proliant SOURCES test/entity/hpe/proliant.c)
opendmi_test(entity/intel/amt SOURCES test/entity/intel/amt.c)
opendmi_test(entity/intel/asf SOURCES test/entity/intel/asf.c)
opendmi_test(entity/intel/platform SOURCES test/entity/intel/platform.c)
opendmi_test(entity/intel/fvi SOURCES test/entity/intel/fvi.c)
opendmi_test(entity/intel/mei SOURCES test/entity/intel/mei.c)
opendmi_test(entity/intel/svt SOURCES test/entity/intel/svt.c)
opendmi_test(entity/intel/vpro SOURCES test/entity/intel/vpro.c)
opendmi_test(entity/lenovo/oem SOURCES test/entity/lenovo/oem.c)
opendmi_test(entity/lenovo/tvt SOURCES test/entity/lenovo/tvt.c)
opendmi_test(entity/intel-rsd/cabled-pcie SOURCES test/entity/intel-rsd/cabled-pcie.c)
opendmi_test(entity/intel-rsd/fpga SOURCES test/entity/intel-rsd/fpga.c)
opendmi_test(entity/intel-rsd/memory-device SOURCES test/entity/intel-rsd/memory-device.c)
opendmi_test(entity/intel-rsd/network-card SOURCES test/entity/intel-rsd/network-card.c)
opendmi_test(entity/intel-rsd/pcie SOURCES test/entity/intel-rsd/pcie.c)
opendmi_test(entity/intel-rsd/phys-device-mapping SOURCES test/entity/intel-rsd/phys-device-mapping.c)
opendmi_test(entity/intel-rsd/processor-cpuid SOURCES test/entity/intel-rsd/processor-cpuid.c)
opendmi_test(entity/intel-rsd/rules SOURCES test/entity/intel-rsd/rules.c)
opendmi_test(entity/intel-rsd/storage-device SOURCES test/entity/intel-rsd/storage-device.c)
opendmi_test(entity/intel-rsd/tpm SOURCES test/entity/intel-rsd/tpm.c)
opendmi_test(entity/intel-rsd/txt SOURCES test/entity/intel-rsd/txt.c)
opendmi_test(entity/acer/hotkeys SOURCES test/entity/acer/hotkeys.c)
opendmi_test(entity/ami/firewire-guid SOURCES test/entity/ami/firewire-guid.c)
opendmi_test(entity/apple/decode SOURCES test/entity/apple/decode.c)
opendmi_test(entity/cisco/ucs SOURCES test/entity/cisco/ucs.c)
opendmi_test(entity/dell/calling-iface SOURCES test/entity/dell/calling-iface.c)
opendmi_test(entity/dell/hotkeys SOURCES test/entity/dell/hotkeys.c)
opendmi_test(entity/dell/indexed-io SOURCES test/entity/dell/indexed-io.c)
opendmi_test(entity/dell/protected-area SOURCES test/entity/dell/protected-area.c)
opendmi_test(entity/dell/records SOURCES test/entity/dell/records.c)
opendmi_test(entity/sun/extended SOURCES test/entity/sun/extended.c)
opendmi_test(entity/battery SOURCES test/entity/battery.c)
opendmi_test(entity/bis-entry-point SOURCES test/entity/bis-entry-point.c)
opendmi_test(entity/pointing-device SOURCES test/entity/pointing-device.c)
opendmi_test(entity/additional-info SOURCES test/entity/additional-info.c)
opendmi_test(entity/mgmt-device-threshold SOURCES test/entity/mgmt-device-threshold.c)
opendmi_test(entity/probe SOURCES test/entity/probe.c)
opendmi_test(entity/processor-ex SOURCES test/entity/processor-ex.c)
opendmi_test(entity/system-boot SOURCES test/entity/system-boot.c)
opendmi_test(entity/system-event-log SOURCES test/entity/system-event-log.c)
opendmi_test(entity/tpm-device SOURCES test/entity/tpm-device.c)
opendmi_test(entity/string-property SOURCES test/entity/string-property.c)

add_manpages(opendmi-manuals ${CMAKE_CURRENT_SOURCE_DIR}/man)
