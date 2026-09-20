#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#

#
# Add ICU4C resource bundles built from resource sources.
#
#   add_resources(<target> <package> <source dir> <output variable>)
#
# Every `<locale>.txt` of <source dir> is compiled into `<locale>.res`, and
# the bundles are packaged into a static data object, which is stored in
# <output variable>. Link the object into the library or executable which
# opens the package.
#
# The package is registered at run time under <package>, which has to be
# passed to `udata_setAppData()`, and the data symbol is `<package>_dat`,
# because `pkgdata` appends "_dat" to the entry point name. Bundles are
# packaged flat, since ICU looks up `<locale>.res` in the root of the package.
# Locales with no bundle of their own fall back to `root.res`, so the source
# directory is expected to have `root.txt` with the default translations.
#
# Nothing is done if ICU is not found, and <output variable> is left unset.
#
function(add_resources TARGET_NAME PACKAGE_NAME SOURCE_DIR OUTPUT_VARIABLE)
    if(NOT ICU_FOUND)
        return()
    endif()

    set(BUNDLE_DIR ${CMAKE_CURRENT_BINARY_DIR}/locale)

    file(GLOB RESOURCE_SOURCES RELATIVE ${SOURCE_DIR} ${SOURCE_DIR}/*.txt)
    file(MAKE_DIRECTORY ${BUNDLE_DIR})

    set(BUNDLES)
    set(BUNDLE_PATHS)

    foreach(SOURCE ${RESOURCE_SOURCES})
        cmake_path(REPLACE_EXTENSION SOURCE ".res" OUTPUT_VARIABLE BUNDLE)

        message(STATUS "Compiling ${SOURCE} -> ${BUNDLE}")

        add_custom_command(
            OUTPUT ${BUNDLE_DIR}/${BUNDLE}
            COMMAND ${ICU_GENRB_EXECUTABLE}
                --sourcedir ${SOURCE_DIR}
                --destdir ${BUNDLE_DIR}
                --encoding UTF-8
                ${SOURCE}
            DEPENDS ${SOURCE_DIR}/${SOURCE}
            VERBATIM
        )

        list(APPEND BUNDLES ${BUNDLE})
        list(APPEND BUNDLE_PATHS ${BUNDLE_DIR}/${BUNDLE})
    endforeach()

    file(GENERATE
        OUTPUT ${BUNDLE_DIR}/${PACKAGE_NAME}-list.txt
        CONTENT "$<JOIN:${BUNDLES},\n>"
    )

    add_custom_target(${TARGET_NAME}-bundles DEPENDS ${BUNDLE_PATHS})

    set(PACKAGE_DATA ${BUNDLE_DIR}/${PACKAGE_NAME}_dat.o)

    add_custom_command(
        OUTPUT ${PACKAGE_DATA}
        COMMAND ${ICU_PKGDATA_EXECUTABLE}
            --name ${PACKAGE_NAME}
            --entrypoint ${PACKAGE_NAME}
            --libname ${PACKAGE_NAME}-resources
            --mode static
            --destdir ${BUNDLE_DIR}
            ${BUNDLE_DIR}/${PACKAGE_NAME}-list.txt
        DEPENDS ${BUNDLE_PATHS} ${BUNDLE_DIR}/${PACKAGE_NAME}-list.txt
        WORKING_DIRECTORY ${BUNDLE_DIR}
        VERBATIM
    )

    add_custom_target(${TARGET_NAME} DEPENDS ${PACKAGE_DATA})

    set(${OUTPUT_VARIABLE} ${PACKAGE_DATA} PARENT_SCOPE)
endfunction()
