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

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Coverage")
    set(ENABLE_COVERAGE ON)

    if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" OR
       "${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang")
        set(COVERAGE_OPTIONS -fprofile-instr-generate -fcoverage-mapping)
    elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
        set(COVERAGE_OPTIONS --coverage)
    endif()
endif()

add_library(opendmi-object OBJECT)
add_library(opendmi-static STATIC)
add_library(opendmi-shared SHARED)

if(ICU_FOUND)
    set(LIBOPENDMI_RESOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/locale)
    set(LIBOPENDMI_BUNDLE_ROOT ${CMAKE_CURRENT_BINARY_DIR}/locale)

    file(GLOB_RECURSE LIBOPENDMI_RESOURCES
        RELATIVE ${LIBOPENDMI_RESOURCE_ROOT}
        ${LIBOPENDMI_RESOURCE_ROOT}/*.txt)
    make_directory(${LIBOPENDMI_BUNDLE_ROOT})

    foreach(RESOURCE_PATH ${LIBOPENDMI_RESOURCES})
        cmake_path(GET RESOURCE_PATH FILENAME RESOURCE_NAME)

        cmake_path(REPLACE_EXTENSION RESOURCE_PATH ".res" OUTPUT_VARIABLE BUNDLE_PATH)
        cmake_path(REMOVE_FILENAME BUNDLE_PATH OUTPUT_VARIABLE BUNDLE_DIR)

        make_directory(${LIBOPENDMI_BUNDLE_ROOT}/${BUNDLE_DIR})

        message(STATUS "Compiling ${RESOURCE_PATH} -> ${BUNDLE_PATH}")

        add_custom_command(
            OUTPUT ${LIBOPENDMI_BUNDLE_ROOT}/${BUNDLE_PATH}
            COMMAND ${ICU_GENRB_EXECUTABLE}
                --sourcedir ${LIBOPENDMI_RESOURCE_ROOT}/${BUNDLE_DIR}
                --destdir ${LIBOPENDMI_BUNDLE_ROOT}/${BUNDLE_DIR}
                --encoding UTF-8
                ${RESOURCE_NAME}
            DEPENDS ${LIBOPENDMI_RESOURCE_ROOT}/${RESOURCE_PATH}
        )

        list(APPEND LIBOPENDMI_BUNDLES ${BUNDLE_PATH})
        list(APPEND LIBOPENDMI_BUNDLES_DEPS ${LIBOPENDMI_BUNDLE_ROOT}/${BUNDLE_PATH})
    endforeach()

    file(GENERATE
        OUTPUT ${LIBOPENDMI_BUNDLE_ROOT}/package-list.txt
        CONTENT "$<JOIN:${LIBOPENDMI_BUNDLES},\n>"
    )

    add_custom_target(opendmi-bundles
        DEPENDS ${LIBOPENDMI_BUNDLES_DEPS}
    )

    set(LIBOPENDMI_BUNDLE_DATA ${LIBOPENDMI_BUNDLE_ROOT}/resources_dat.o)
    add_custom_command(
        OUTPUT ${LIBOPENDMI_BUNDLE_DATA}
        COMMAND ${ICU_PKGDATA_EXECUTABLE}
            --name resources
            --entrypoint dmi_resources
            --libname opendmi-resources
            --mode static
            --destdir ${LIBOPENDMI_BUNDLE_ROOT}
            ${LIBOPENDMI_BUNDLE_ROOT}/package-list.txt
        DEPENDS opendmi-bundles
        WORKING_DIRECTORY ${LIBOPENDMI_BUNDLE_ROOT}
    )

    add_custom_target(opendmi-resources
        DEPENDS ${LIBOPENDMI_BUNDLE_DATA}
    )

    add_dependencies(opendmi-object opendmi-resources)

    target_include_directories(opendmi-object PRIVATE ${ICU_INCLUDE_DIRS})
    target_sources(opendmi-static PUBLIC ${LIBOPENDMI_BUNDLE_DATA})
    target_sources(opendmi-shared PUBLIC ${LIBOPENDMI_BUNDLE_DATA})

    list(APPEND LIBOPENDMI_LIBRARIES ICU::uc)
endif()

configure_file(
    ${PROJECT_SOURCE_DIR}/src/config.h.in
    ${PROJECT_SOURCE_DIR}/src/config.h
    USE_SOURCE_PERMISSIONS)

target_include_directories(opendmi-object PUBLIC include)
target_sources(opendmi-object
    PRIVATE
        src/attribute.c
        src/backend.c
        src/context.c
        src/entity.c
        src/entry.c
        src/error.c
        src/filter.c
        src/log.c
        src/module.c
        src/registry.c
        src/stream.c
        src/utils.c
        src/vendor.c

        src/utils/base64.c
        src/utils/codec.c
        src/utils/datetime.c
        src/utils/endian.c
        src/utils/file.c
        src/utils/name.c
        src/utils/string.c
        src/utils/uuid.c
        src/utils/vector.c
        src/utils/version.c

        src/entity/additional-info.c
        src/entity/baseboard.c
        src/entity/battery.c
        src/entity/bis-entry-point.c
        src/entity/cache.c
        src/entity/chassis.c
        src/entity/common.c
        src/entity/cooling-device.c
        src/entity/current-probe.c
        src/entity/firmware.c
        src/entity/firmware-inventory.c
        src/entity/firmware-language.c
        src/entity/group-assoc.c
        src/entity/hardware-security.c
        src/entity/ipmi-device.c
        src/entity/memory-array.c
        src/entity/memory-array-addr.c
        src/entity/memory-channel.c
        src/entity/memory-controller.c
        src/entity/memory-device.c
        src/entity/memory-device-addr.c
        src/entity/memory-error.c
        src/entity/memory-error-32.c
        src/entity/memory-error-64.c
        src/entity/memory-module.c
        src/entity/mgmt-controller.c
        src/entity/mgmt-device.c
        src/entity/mgmt-device-component.c
        src/entity/mgmt-device-threshold.c
        src/entity/oem-strings.c
        src/entity/onboard-device.c
        src/entity/onboard-device-ex.c
        src/entity/oob-remote-access.c
        src/entity/pointing-device.c
        src/entity/port-connector.c
        src/entity/power-controls.c
        src/entity/power-supply.c
        src/entity/probe.c
        src/entity/processor.c
        src/entity/processor-ex.c
        src/entity/slot.c
        src/entity/string-property.c
        src/entity/system.c
        src/entity/system-boot.c
        src/entity/system-config.c
        src/entity/system-event-log.c
        src/entity/system-reset.c
        src/entity/temperature-probe.c
        src/entity/tpm-device.c
        src/entity/voltage-probe.c

        src/module/acer.c

        src/module/ami.c
        src/entity/ami/type-221.c

        src/module/apple.c

        src/module/dell.c
        src/entity/dell/common.c
        src/entity/dell/bios-flags.c
        src/entity/dell/calling-iface.c
        src/entity/dell/indexed-io.c
        src/entity/dell/infrared-port.c
        src/entity/dell/parallel-port.c
        src/entity/dell/protected-area-1.c
        src/entity/dell/protected-area-2.c
        src/entity/dell/revisions.c
        src/entity/dell/serial-port.c

        src/module/hpe.c

        src/module/intel.c
        src/entity/intel/rsd-cabled-pcie.c
        src/entity/intel/rsd-fpga.c
        src/entity/intel/rsd-memory-device.c
        src/entity/intel/rsd-network-card.c
        src/entity/intel/rsd-pcie.c
        src/entity/intel/rsd-phys-device-mapping.c
        src/entity/intel/rsd-processor-cpuid.c
        src/entity/intel/rsd-storage-device.c
        src/entity/intel/rsd-tpm.c
        src/entity/intel/rsd-txt.c

        src/module/lenovo.c

        src/module/sun.c
        src/entity/sun/memory-array-ex.c
        src/entity/sun/memory-device-ex.c
        src/entity/sun/pcie-root-complex.c
        src/entity/sun/port-ex.c
        src/entity/sun/processor-ex.c

        src/backend/dump.c
    PUBLIC FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/opendmi/attribute.h
            include/opendmi/backend.h
            include/opendmi/context.h
            include/opendmi/defs.h
            include/opendmi/entry.h
            include/opendmi/error.h
            include/opendmi/filter.h
            include/opendmi/keyword.h
            include/opendmi/log.h
            include/opendmi/module.h
            include/opendmi/registry.h
            include/opendmi/entity.h
            include/opendmi/types.h
            include/opendmi/stream.h
            include/opendmi/utils.h
            include/opendmi/value.h
            include/opendmi/vendor.h

            include/opendmi/utils/base64.h
            include/opendmi/utils/codec.h
            include/opendmi/utils/datetime.h
            include/opendmi/utils/endian.h
            include/opendmi/utils/file.h
            include/opendmi/utils/name.h
            include/opendmi/utils/string.h
            include/opendmi/utils/uuid.h
            include/opendmi/utils/vector.h
            include/opendmi/utils/version.h

            include/opendmi/entity/additional-info.h
            include/opendmi/entity/baseboard.h
            include/opendmi/entity/battery.h
            include/opendmi/entity/bis-entry-point.h
            include/opendmi/entity/cache.h
            include/opendmi/entity/chassis.h
            include/opendmi/entity/common.h
            include/opendmi/entity/cooling-device.h
            include/opendmi/entity/current-probe.h
            include/opendmi/entity/firmware-inventory.h
            include/opendmi/entity/firmware-language.h
            include/opendmi/entity/firmware.h
            include/opendmi/entity/group-assoc.h
            include/opendmi/entity/hardware-security.h
            include/opendmi/entity/ipmi-device.h
            include/opendmi/entity/memory-array-addr.h
            include/opendmi/entity/memory-array.h
            include/opendmi/entity/memory-channel.h
            include/opendmi/entity/memory-controller.h
            include/opendmi/entity/memory-device-addr.h
            include/opendmi/entity/memory-device.h
            include/opendmi/entity/memory-error.h
            include/opendmi/entity/memory-error-32.h
            include/opendmi/entity/memory-error-64.h
            include/opendmi/entity/memory-module.h
            include/opendmi/entity/mgmt-controller.h
            include/opendmi/entity/mgmt-device.h
            include/opendmi/entity/mgmt-device-component.h
            include/opendmi/entity/mgmt-device-threshold.h
            include/opendmi/entity/oem-strings.h
            include/opendmi/entity/onboard-device.h
            include/opendmi/entity/onboard-device-ex.h
            include/opendmi/entity/oob-remote-access.h
            include/opendmi/entity/pointing-device.h
            include/opendmi/entity/port-connector.h
            include/opendmi/entity/power-controls.h
            include/opendmi/entity/power-supply.h
            include/opendmi/entity/probe.h
            include/opendmi/entity/processor.h
            include/opendmi/entity/processor-ex.h
            include/opendmi/entity/slot.h
            include/opendmi/entity/string-property.h
            include/opendmi/entity/system.h
            include/opendmi/entity/system-boot.h
            include/opendmi/entity/system-config.h
            include/opendmi/entity/system-event-log.h
            include/opendmi/entity/system-reset.h
            include/opendmi/entity/temperature-probe.h
            include/opendmi/entity/tpm-device.h
            include/opendmi/entity/voltage-probe.h

            include/opendmi/module/acer.h

            include/opendmi/module/ami.h
            include/opendmi/entity/ami/type-221.h

            include/opendmi/module/apple.h

            include/opendmi/module/dell.h
            include/opendmi/entity/dell/common.h
            include/opendmi/entity/dell/bios-flags.h
            include/opendmi/entity/dell/calling-iface.h
            include/opendmi/entity/dell/indexed-io.h
            include/opendmi/entity/dell/infrared-port.h
            include/opendmi/entity/dell/parallel-port.h
            include/opendmi/entity/dell/protected-area-1.h
            include/opendmi/entity/dell/protected-area-2.h
            include/opendmi/entity/dell/revisions.h
            include/opendmi/entity/dell/serial-port.h

            include/opendmi/module/hpe.h

            include/opendmi/module/intel.h
            include/opendmi/entity/intel/rsd-cabled-pcie.h
            include/opendmi/entity/intel/rsd-fpga.h
            include/opendmi/entity/intel/rsd-memory-device.h
            include/opendmi/entity/intel/rsd-network-card.h
            include/opendmi/entity/intel/rsd-pcie.h
            include/opendmi/entity/intel/rsd-phys-device-mapping.h
            include/opendmi/entity/intel/rsd-processor-cpuid.h
            include/opendmi/entity/intel/rsd-storage-device.h
            include/opendmi/entity/intel/rsd-tpm.h
            include/opendmi/entity/intel/rsd-txt.h

            include/opendmi/module/lenovo.h

            include/opendmi/module/sun.h
            include/opendmi/entity/sun/memory-array-ex.h
            include/opendmi/entity/sun/memory-device-ex.h
            include/opendmi/entity/sun/pcie-root-complex.h
            include/opendmi/entity/sun/port-ex.h
            include/opendmi/entity/sun/processor-ex.h

            include/opendmi/backend/dump.h
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(opendmi-object
        PRIVATE
            src/backend/linux.c
        PUBLIC FILE_SET HEADERS
            BASE_DIRS include
            FILES include/opendmi/backend/linux.h
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    find_library(COREFOUNDATION_LIB CoreFoundation)
    find_library(IOKIT_LIB IOKit)
    list(APPEND LIBOPENDMI_LIBRARIES ${COREFOUNDATION_LIB} ${IOKIT_LIB})

    target_sources(opendmi-object
        PRIVATE
            src/backend/darwin.c
        PUBLIC FILE_SET HEADERS
            BASE_DIRS include
            FILES include/opendmi/backend/darwin.h
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    target_sources(opendmi-object
        PRIVATE
            src/backend/generic.c
            src/backend/freebsd.c
        PUBLIC FILE_SET HEADERS
            BASE_DIRS include
            FILES
                include/opendmi/backend/generic.h
                include/opendmi/backend/freebsd.h
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
    target_sources(opendmi-object
        PRIVATE
            src/backend/generic.c
            src/backend/netbsd.c
        PUBLIC FILE_SET HEADERS
            BASE_DIRS include
            FILES
                include/opendmi/backend/generic.h
                include/opendmi/backend/netbsd.h
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND LIBOPENDMI_LIBRARIES ntdll)

    target_sources(opendmi-object
        PRIVATE
            src/backend/windows.c
            src/utils/win32.c
        PUBLIC FILE_SET HEADERS
            BASE_DIRS include
            FILES include/opendmi/backend/windows.h
    )
endif()

if(NOT MSVC)
    target_compile_options(opendmi-object PRIVATE -fPIC)
endif()
if(ENABLE_COVERAGE)
    target_compile_options(opendmi-object PUBLIC ${COVERAGE_OPTIONS})
endif()

target_include_directories(opendmi-static INTERFACE include)
target_sources(opendmi-static PRIVATE $<TARGET_OBJECTS:opendmi-object>)
target_link_libraries(opendmi-static PUBLIC ${LIBOPENDMI_LIBRARIES})
set_target_properties(opendmi-static PROPERTIES OUTPUT_NAME opendmi)

target_include_directories(opendmi-shared INTERFACE include)
target_sources(opendmi-shared PRIVATE $<TARGET_OBJECTS:opendmi-object>)
target_link_libraries(opendmi-shared PUBLIC ${LIBOPENDMI_LIBRARIES})
if(ENABLE_COVERAGE)
    target_link_options(opendmi-shared PUBLIC ${COVERAGE_OPTIONS})
endif()
set_target_properties(opendmi-shared PROPERTIES OUTPUT_NAME opendmi)

install(TARGETS opendmi-object PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS opendmi-static ARCHIVE DESTINATION lib)
install(TARGETS opendmi-shared LIBRARY DESTINATION lib)

macro(opendmi_test TEST_NAME TEST_SOURCES)
    string(REPLACE "/" "-" TEST_TARGET "${TEST_NAME}")
    string(APPEND TEST_TARGET "-test")

    add_executable(${TEST_TARGET} ${TEST_SOURCES} test/logger.c)
    target_compile_definitions(${TEST_TARGET} PRIVATE OPENDMI_TEST_DATA="${PROJECT_SOURCE_DIR}/../data")
    target_link_libraries(${TEST_TARGET} opendmi-shared cmocka-static)

    if(ENABLE_COVERAGE)
        target_compile_options(${TEST_TARGET} PUBLIC ${COVERAGE_OPTIONS})
        target_link_options(${TEST_TARGET} PUBLIC ${COVERAGE_OPTIONS})
    endif()

    add_test(
        NAME ${TEST_NAME}
        COMMAND ${TEST_TARGET}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    set_tests_properties(${TEST_NAME} PROPERTIES SKIP_RETURN_CODE 69)

    if(ENABLE_COVERAGE)
        set_tests_properties(${TEST_NAME} PROPERTIES
            ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_CURRENT_BINARY_DIR}/bin/${TEST_TARGET}.profraw"
        )
    endif()
endmacro()

opendmi_test(attribute test/attribute.c)
opendmi_test(error test/error.c)
opendmi_test(context test/context.c)
opendmi_test(entity test/entity.c)
opendmi_test(entry test/entry.c)
opendmi_test(module test/module.c)
opendmi_test(stream test/stream.c)

opendmi_test(utils/endian test/utils/endian.c)
opendmi_test(utils/codec test/utils/codec.c)
opendmi_test(utils/string test/utils/string.c)
opendmi_test(utils/vector test/utils/vector.c)
opendmi_test(utils/version test/utils/version.c)
opendmi_test(utils/datetime test/utils/datetime.c)
opendmi_test(utils/uuid test/utils/uuid.c)
opendmi_test(utils/misc test/utils/misc.c)

opendmi_test(entity/common test/entity/common.c)
opendmi_test(entity/baseboard test/entity/baseboard.c)
opendmi_test(entity/chassis test/entity/chassis.c)
opendmi_test(entity/memory-controller test/entity/memory-controller.c)
opendmi_test(entity/cache test/entity/cache.c)
opendmi_test(entity/slot test/entity/slot.c)
opendmi_test(entity/onboard-device test/entity/onboard-device.c)
opendmi_test(entity/memory-array test/entity/memory-array.c)
opendmi_test(entity/memory-device test/entity/memory-device.c)
opendmi_test(entity/memory-error test/entity/memory-error.c)
opendmi_test(entity/memory-channel test/entity/memory-channel.c)
opendmi_test(entity/ipmi-device test/entity/ipmi-device.c)
opendmi_test(entity/battery test/entity/battery.c)
opendmi_test(entity/pointing-device test/entity/pointing-device.c)
opendmi_test(entity/probe test/entity/probe.c)

if(ASCIIDOCTOR_FOUND)
    file(GLOB_RECURSE LIBOPENDMI_MANPAGE_SOURCES
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/man
        *.adoc)

    foreach(src ${LIBOPENDMI_MANPAGE_SOURCES})
        cmake_path(REMOVE_EXTENSION src LAST_ONLY OUTPUT_VARIABLE dst)

        set(src_path "${CMAKE_CURRENT_SOURCE_DIR}/man/${src}")
        set(dst_path "${CMAKE_CURRENT_BINARY_DIR}/man/${dst}")

        cmake_path(REMOVE_FILENAME dst_path OUTPUT_VARIABLE dst_dir)
        make_directory(${dst_dir})

        add_custom_command(
            OUTPUT ${dst_path}
            COMMAND ${ASCIIDOCTOR_EXECUTABLE} --backend manpage --out-file ${dst_path} ${src_path}
            DEPENDS ${src_path}
        )

        if (GZIP_EXECUTABLE)
            add_custom_command(
                OUTPUT ${dst_path}.gz
                COMMAND ${GZIP_EXECUTABLE} --force ${dst_path}
                DEPENDS ${dst_path}
            )

            list(APPEND LIBOPENDMI_MANPAGES ${dst_path}.gz)
        else()
            list(APPEND LIBOPENDMI_MANPAGES ${dst_path})
        endif()
    endforeach()

    add_custom_target(opendmi-manuals DEPENDS ${LIBOPENDMI_MANPAGES})
    add_dependencies(opendmi-object opendmi-manuals)

    install(FILES ${LIBOPENDMI_MANPAGES} TYPE MAN)
endif()
