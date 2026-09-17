#!/bin/sh
#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
OSNAME=`uname -s`

BUILD_DIR=${BUILD_DIR:-build}

LLVM_PROF=llvm-profdata
LLVM_COV=llvm-cov
LIB_SUFFIX=so

if [ "${OSNAME}" = "Darwin" ]; then
    LLVM_PROF="xcrun ${LLVM_PROF}"
    LLVM_COV="xcrun ${LLVM_COV}"
    LIB_SUFFIX=dylib
fi

# Test sources and third-party dependencies are excluded from reports
IGNORE_REGEX='/test/|/_deps/'

_usage() {
    cat <<EOF
Usage: $0 <command> [component]

Commands:
    merge       Merge raw profiles into <component>.profdata
    show        Show line-by-line coverage
    report      Show coverage summary
    help        Print this help and exit

Components:
    libopendmi  Library, covered by library tests (default)
    opendmi     Command line tool, covered by its tests

Environment:
    BUILD_DIR   Build directory (default: build)
EOF
}

_missing_command() {
    echo "Missing command. Use -h or --help for help" 1>&2
    exit 1
}

_invalid_command() {
    echo "Invalid command: $1. Use -h or --help for help" 1>&2
    exit 1
}

_invalid_component() {
    echo "Invalid component: $1. Use -h or --help for help" 1>&2
    exit 1
}

# Print regular expression for files excluded from the component reports
_ignore_regex()
{
    case "$1" in
        libopendmi)
            echo "${IGNORE_REGEX}"
            ;;
        opendmi)
            # Library code (e.g. inline functions) is covered by its own tests
            echo "${IGNORE_REGEX}|/libopendmi/"
            ;;
    esac
}

# Print instrumented objects of the component in the form expected by llvm-cov
_objects()
{
    case "$1" in
        libopendmi)
            echo "${BUILD_DIR}/libopendmi/lib/libopendmi.${LIB_SUFFIX}"
            ;;
        opendmi)
            # Command line tool sources are linked into test executables
            OBJECTS=""
            for OBJECT in "${BUILD_DIR}"/opendmi/bin/*-test; do
                if [ -z "${OBJECTS}" ]; then
                    OBJECTS="${OBJECT}"
                else
                    OBJECTS="${OBJECTS} -object ${OBJECT}"
                fi
            done
            echo "${OBJECTS}"
            ;;
    esac
}

_merge()
{
    ${LLVM_PROF} merge --sparse "${BUILD_DIR}/$1"/bin/*.profraw -o "$1.profdata"
}

_show()
{
    ${LLVM_COV} show `_objects $1` -instr-profile="$1.profdata" -ignore-filename-regex="`_ignore_regex $1`"
}

_report()
{
    ${LLVM_COV} report `_objects $1` -instr-profile="$1.profdata" -ignore-filename-regex="`_ignore_regex $1`"
}

if [ $# -eq 0 ]; then
    _missing_command
fi

COMMAND=$1
COMPONENT=${2:-libopendmi}

case "${COMMAND}" in
    help|-h|--help)
        _usage
        exit 0
        ;;
esac

case "${COMPONENT}" in
    libopendmi|opendmi)
        ;;
    *)
        _invalid_component ${COMPONENT}
        ;;
esac

case "${COMMAND}" in
    merge)
        _merge ${COMPONENT}
        ;;
    show)
        _show ${COMPONENT}
        ;;
    report)
        _report ${COMPONENT}
        ;;
    *)
        _invalid_command ${COMMAND}
        ;;
esac
