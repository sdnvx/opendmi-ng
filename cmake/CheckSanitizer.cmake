#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
# Check whether the AddressSanitizer of the toolchain runs on this platform.
#
# The sanitizer is a runtime library of the compiler rather than a part of the
# program, and a runtime the operating system has outgrown hangs every program
# before it reaches main(). Compiling and linking say nothing about it, so the
# check builds a program with the sanitizer and runs it with a time limit:
# without it, a toolchain like that leaves every test of the build timing out
# with nothing to point at.
#
# FLAG is the name of the option holding the answer, which is left alone
# unless it is AUTO: ON requires the sanitizer, and OFF skips the check.
#
macro(check_sanitizer FLAG)
    if(NOT DEFINED ${FLAG} OR "${${FLAG}}" STREQUAL "AUTO")
        message(STATUS "Looking for a working AddressSanitizer (optional)...")

        __check_sanitizer_runs(__sanitizer_runs)

        set(${FLAG} ${__sanitizer_runs})

        if(NOT ${FLAG})
            message(WARNING "AddressSanitizer does not run with this toolchain, "
                            "building without it")
        endif()
    elseif(${FLAG})
        message(STATUS "Looking for a working AddressSanitizer (required)...")

        __check_sanitizer_runs(__sanitizer_runs)

        if(NOT __sanitizer_runs)
            message(FATAL_ERROR "AddressSanitizer does not run with this toolchain")
        endif()
    endif()
endmacro()

#
# Build a program with the sanitizer and run it, which is the only way to tell
# a runtime which hangs from one which works.
#
function(__check_sanitizer_runs RESULT)
    set(${RESULT} OFF PARENT_SCOPE)

    # The runtime belongs to the compiler, so the answer is kept for the
    # compiler which gave it and asked again once another one is chosen. The
    # resource directory is what names the runtime, and it is asked for every
    # time, since a toolchain chosen with xcode-select changes it while the
    # name and the version of the compiler stay as they are
    execute_process(COMMAND "${CMAKE_C_COMPILER}" -print-resource-dir
                    RESULT_VARIABLE known
                    OUTPUT_VARIABLE resources
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET)

    if(NOT known EQUAL 0)
        set(resources "${CMAKE_C_COMPILER_VERSION}")
    endif()

    set(toolchain "${CMAKE_C_COMPILER}:${resources}")

    if(DEFINED CACHE{OPENDMI_SANITIZER_RUNS} AND
       "${OPENDMI_SANITIZER_TOOLCHAIN}" STREQUAL "${toolchain}")
        set(${RESULT} ${OPENDMI_SANITIZER_RUNS} PARENT_SCOPE)
        return()
    endif()

    set(OPENDMI_SANITIZER_TOOLCHAIN "${toolchain}" CACHE INTERNAL "Toolchain the answer is of")

    # Programs built here are of another platform and cannot be run, so the
    # runtime of the target is taken on trust
    if(CMAKE_CROSSCOMPILING)
        set(OPENDMI_SANITIZER_RUNS ON CACHE INTERNAL "AddressSanitizer runs")
        set(${RESULT} ON PARENT_SCOPE)
        return()
    endif()

    set(directory "${CMAKE_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/CheckSanitizer")
    set(source "${directory}/main.c")
    set(binary "${directory}/check-sanitizer")

    file(WRITE "${source}" "int main(void) { return 0; }\n")

    try_compile(built
                SOURCES "${source}"
                COMPILE_DEFINITIONS -fsanitize=address
                LINK_OPTIONS -fsanitize=address
                COPY_FILE "${binary}")

    if(NOT built)
        message(STATUS "AddressSanitizer is not supported by the toolchain")
        set(OPENDMI_SANITIZER_RUNS OFF CACHE INTERNAL "AddressSanitizer runs")
        return()
    endif()

    # A runtime which hangs does so before the program starts, and burns a
    # core until it is killed, so the check waits for it rather than for the
    # program, which has nothing to do
    execute_process(COMMAND "${binary}"
                    TIMEOUT 10
                    RESULT_VARIABLE status
                    OUTPUT_QUIET
                    ERROR_QUIET)

    if(status EQUAL 0)
        message(STATUS "AddressSanitizer runs")
        set(OPENDMI_SANITIZER_RUNS ON CACHE INTERNAL "AddressSanitizer runs")
        set(${RESULT} ON PARENT_SCOPE)
    else()
        message(STATUS "AddressSanitizer fails to run: ${status}")
        set(OPENDMI_SANITIZER_RUNS OFF CACHE INTERNAL "AddressSanitizer runs")
    endif()
endfunction()
