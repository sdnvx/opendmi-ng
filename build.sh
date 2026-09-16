#!/bin/sh
#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
set -e

SCRIPT_PATH=`readlink -f "$0"`
SCRIPT_ROOT=`dirname "$0"`
SCRIPT_NAME=`basename "$0"`

CONFIG_DIST="${SCRIPT_ROOT}/build.conf.dist"
CONFIG_LOCAL="${SCRIPT_ROOT}/build.conf"

. ${CONFIG_DIST}

if [ -f "${CONFIG_LOCAL}" ]; then
    . ${CONFIG_LOCAL}
fi

case "${BUILD_DIR}" in
    /*)
        ;;
    *)
        BUILD_DIR="${SCRIPT_ROOT}/${BUILD_DIR}"
        ;;
esac

if [ ! -d "${BUILD_DIR}" ]; then
    mkdir -p "${BUILD_DIR}"
fi

BUILD_DIR=`readlink -f "${BUILD_DIR}"`

OSNAME=`uname -s`
case "${OSNAME}" in
    Linux)
        NPROC=`nproc --all`
        ;;
    FreeBSD|OpenBSD|NetBSD|Darwin)
        NPROC=`sysctl -n hw.ncpu`
        ;;
    *)
        NPROC=1
esac

_find_command() {
    local NAME="$1"
    local COMMAND="$2"
    local PATH=`which ${COMMAND} 2>/dev/null`

    if [ -z "${PATH}" ]; then
        echo "${NAME} not found (${COMMAND})" 1>&2
        exit 1
    fi

    echo ${PATH}
}

CMAKE="`_find_command "CMake" cmake`"
CTEST="`_find_command "CTest" ctest`"
COMPILER="`_find_command "Compiler" cc`"

_missing_command() {
    echo "Missing command. Use -h or --help for help" 1>&2
    exit 1
}

_invalid_command() {
    echo "Invalid command: $1. Use -h or --help for help" 1>&2
    exit 1
}

_invalid_option() {
    echo "Invalid option: $1. Use -h or --help for help" 1>&2
    exit 1
}

_require_argument() {
    if [ $2 -eq 0 ]; then
        echo "Option requires an argument: $1" 1>&2
        exit 1
    fi
}

_usage() {
    echo "OpenDMI: Cross-platform DMI/SMBIOS framework"
    echo "Copyright (c) 2025-2026, The OpenDMI contributors"
    echo
    echo "Usage:"
    echo "    ${SCRIPT_NAME} [global options] <command> [command options]"
    echo
    echo "Global options:"
    echo "    -h, --help         Print this help and exit"
    echo "    -b, --build <DIR>  Set build directory"
    echo "    -j, --jobs <N>     Set number of parallel jobs"
    echo
    echo "Commands:"
    echo "    configure  Configure build settings"
    echo "    build      Build the entire project"
    echo "    install    Install the project to the configured prefix"
    echo "    package    Build distributable packages using CPack"
    echo "    test       Perform unit tests"
    echo "    clean      Delete all files that are created by building the program"
    echo "    distclean  Delete all files that are created by configuring or building the program"
    echo
    echo "Configure options:"
    echo "    General:"
    echo "        --prefix <PATH>    Set installation prefix (default=${PREFIX})"
    echo "        --compiler <PATH>  Set path to C compiler"
    echo "        --release          Release build"
    echo "        --debug            Debug build"
    echo "        --coverage         Coverage build"
    echo "    Components:"
    echo "        --enable-all       Build all components"
    echo "        --enable-golang    Build with Go support (libopendmi-go, default=${ENABLE_GOLANG})"
    echo "        --enable-python    Build with Python support (libopendmi-python, default=${ENABLE_PYTHON})"
    echo "        --enable-rust      Build with Rust support (libopendmi-rust, default=${ENABLE_RUST})"
    echo "        --enable-dbus      Build with D-bus support (opendmi-dbus, default=${ENABLE_DBUS})"
    echo "    Features:"
    echo "        --with-icu         Build with ICU4C support (default=${ENABLE_ICU})"
    echo "        --with-xml         Build with XML support (default=${ENABLE_XML})"
    echo "        --with-yaml        Build with YAML support (default=${ENABLE_YAML})"
    echo "        --with-json        Build with JSON support (default=${ENABLE_JSON})"
    echo "    Miscellaneous:"
    echo "        --verbose          Generate verbose Makefiles"
    echo
    echo "Defaults:"
    echo "    Compiler:        ${COMPILER}"
    echo "    Build directory: ${BUILD_DIR}"
    echo "    Build type:      ${BUILD_TYPE}"
    echo "    Number of jobs:  ${NPROC}"
}

_configure() {
    RELEASE_BRANCH=`git branch --show-current`
    if [ "${RELEASE_BRANCH}" = "main" ]; then
        RELEASE_DATE=`date +'%Y-%m-%d'`
    else
        RELEASE_TAG=`git describe --tags --abbrev=0`
        RELEASE_DATE=`git log -1 --format="%ai" ${RELEASE_TAG} | awk '{print $1}'`
    fi

    while [ $# -gt 0 ]; do
        OPTION=$1
        shift

        case "$OPTION" in
            --compiler)
                _require_argument "$OPTION" "$#"
                COMPILER="$1"
                shift
                ;;
            --prefix)
                _require_argument "$OPTION" "$#"
                PREFIX="$1"
                shift
                ;;
            --release)
                BUILD_TYPE=Release
                ;;
            --debug)
                BUILD_TYPE=Debug
                ;;
            --coverage)
                BUILD_TYPE=Coverage
                ;;
            --enable-all)
                ENABLE_GOLANG=ON
                ENABLE_PYTHON=ON
                ENABLE_RUST=ON
                ENABLE_DBUS=ON
                ;;
            --enable-golang)
                ENABLE_GOLANG=ON
                ;;
            --enable-python)
                ENABLE_PYTHON=ON
                ;;
            --enable-rust)
                ENABLE_RUST=ON
                ;;
            --enable-dbus)
                ENABLE_DBUS=ON
                ;;
            --with-icu)
                ENABLE_ICU=ON
                ;;
            --with-xml)
                ENABLE_XML=ON
                ;;
            --with-yaml)
                ENABLE_YAML=ON
                ;;
            --with-json)
                ENABLE_JSON=ON
                ;;
            --verbose)
                VERBOSE=ON
                ;;
            *)
                _invalid_option ${OPTION}
                ;;
        esac
    done

    FEATURES=""
    if [ "${ENABLE_ICU}" != "AUTO" ]; then
        FEATURES="${FEATURES} -DENABLE_ICU=${ENABLE_ICU}"
    fi
    if [ "${ENABLE_XML}" != "AUTO" ]; then
        FEATURES="${FEATURES} -DENABLE_XML=${ENABLE_XML}"
    fi
    if [ "${ENABLE_YAML}" != "AUTO" ]; then
        FEATURES="${FEATURES} -DENABLE_YAML=${ENABLE_YAML}"
    fi
    if [ "${ENABLE_JSON}" != "AUTO" ]; then
        FEATURES="${FEATURES} -DENABLE_JSON=${ENABLE_JSON}"
    fi
    if [ "${VERBOSE}" = "ON" ]; then
        FEATURES="${FEATURES} -DCMAKE_VERBOSE_MAKEFILE=TRUE"
    fi

    echo
    echo "Target:          ${OSNAME}"
    echo "Using CMake:     ${CMAKE}"
    echo "Using CTest:     ${CTEST}"
    echo "Using compiler:  ${COMPILER}"
    echo "Build directory: ${BUILD_DIR}"
    echo "Release branch:  ${RELEASE_BRANCH}"
    echo "Release date:    ${RELEASE_DATE}"
    echo "Build type:      ${BUILD_TYPE}"
    echo "Number of jobs:  ${NPROC}"
    echo

    ${CMAKE} -B ${BUILD_DIR} \
        -DCMAKE_C_COMPILER=${COMPILER} \
        -DCMAKE_INSTALL_PREFIX=${PREFIX} \
        -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
        -DOPENDMI_RELEASE_DATE=${RELEASE_DATE} \
        -DENABLE_GOLANG=${ENABLE_GOLANG} \
        -DENABLE_PYTHON=${ENABLE_PYTHON} \
        -DENABLE_RUST=${ENABLE_RUST} \
        -DENABLE_DBUS=${ENABLE_DBUS} \
        ${FEATURES}
}

_build() {
    ${CMAKE} --build ${BUILD_DIR} --parallel ${NPROC}
}

_test() {
    local CTEST_ARGS=""

    while [ $# -gt 0 ]; do
        OPTION=$1
        shift

        case "$OPTION" in
            --failed)
                CTEST_ARGS="${CTEST_ARGS} --rerun-failed"
                ;;
            *)
                _invalid_option ${OPTION}
                ;;
        esac
    done

    ${CTEST} --test-dir ${BUILD_DIR} --parallel ${NPROC} --output-on-failure ${CTEST_ARGS}
}

_install() {
    ${CMAKE} --build ${BUILD_DIR} --target install
}

_clean() {
    ${CMAKE} --build ${BUILD_DIR} --target clean
}

_package() {
    if [ ! -f "${BUILD_DIR}/CPackConfig.cmake" ]; then
        echo "Project is not configured. Run './${SCRIPT_NAME} configure' first." 1>&2
        exit 1
    fi
    (cd "${BUILD_DIR}" && cpack)
}

_distclean()
{
    rm -rf ${BUILD_DIR}
}

while [ $# -gt 0 ]; do
    OPTION=$1

    STARTCH=`echo ${OPTION} | cut -c1-1`
    if [ "${STARTCH}" != "-" ]; then
        break
    fi
    shift

    case "${OPTION}" in
        -h|--help)
            _usage
            exit 0
            ;;
        -b|--build)
            _require_argument ${OPTION} $#
            BUILD_DIR=$1
            shift
            ;;
        -j|--jobs)
            _require_argument ${OPTION} $#
            NPROC=$1
            shift
            ;;
        *)
            _invalid_option ${OPTION}
            ;;
    esac
done

if [ $# -eq 0 ]; then
    _missing_command
fi

COMMAND=$1
shift

cd "${SCRIPT_ROOT}"

case "${COMMAND}" in
    configure)
        _configure "$@"
        ;;
    build)
        _build "$@"
        ;;
    test)
        _test "$@"
        ;;
    install)
        _install "$@"
        ;;
    clean)
        _clean "$@"
        ;;
    package)
        _package "$@"
        ;;
    distclean)
        _distclean "$@"
        ;;
    *)
        _invalid_command ${COMMAND}
        ;;
esac
