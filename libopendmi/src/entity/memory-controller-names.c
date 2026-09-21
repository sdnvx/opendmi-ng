//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/memory-controller-internal.h>

const dmi_name_set_t dmi_error_detect_method_names =
{
    .code  = "error-detect-method",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_ERROR_DETECT_METHOD_UNSPEC),
        DMI_NAME_OTHER(DMI_ERROR_DETECT_METHOD_OTHER),
        DMI_NAME_UNKNOWN(DMI_ERROR_DETECT_METHOD_UNKNOWN),
        DMI_NAME_NONE(DMI_ERROR_DETECT_METHOD_NONE),
        {
            .id   = DMI_ERROR_DETECT_METHOD_PARITY,
            .code = "parity",
            .name = "8-bit Parity"
        },
        {
            .id   = DMI_ERROR_DETECT_METHOD_ECC_32,
            .code = "ecc-32",
            .name = "32-bit ECC"
        },
        {
            .id   = DMI_ERROR_DETECT_METHOD_ECC_64,
            .code = "ecc-64",
            .name = "64-bit ECC"
        },
        {
            .id   = DMI_ERROR_DETECT_METHOD_ECC_128,
            .code = "ecc-128",
            .name = "128-bit ECC"
        },
        {
            .id   = DMI_ERROR_DETECT_METHOD_CRC,
            .code = "crc",
            .name = "CRC"
        },
        {}
    })
};

const dmi_name_set_t dmi_error_correct_caps_names =
{
    .code  = "error-correct-cap",
    .names = DMI_NAMES({
        DMI_NAME_OTHER(0),
        DMI_NAME_UNKNOWN(1),
        DMI_NAME_NONE(2),
        {
            .id   = 3,
            .code = "single-bit",
            .name = "Single-bit error correcting"
        },
        {
            .id   = 4,
            .code = "double-bit",
            .name = "Double-bit error correcting"
        },
        {
            .id   = 5,
            .code = "scrubbing",
            .name = "Error scrubbing"
        },
        {}
    })
};

const dmi_name_set_t dmi_memory_module_speed_names =
{
    .code  = "memory-module-speed",
    .names = DMI_NAMES({
        DMI_NAME_OTHER(0),
        DMI_NAME_UNKNOWN(1),
        {
            .id   = 2,
            .code = "70ns",
            .name = "70 ns"
        },
        {
            .id   = 3,
            .code = "60ns",
            .name = "60 ns"
        },
        {
            .id   = 4,
            .code = "50ns",
            .name = "50 ns"
        },
        {}
    })
};

const dmi_name_set_t dmi_memory_interleave_names =
{
    .code  = "memory-interleave",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_MEMORY_INTERLEAVE_UNSPEC),
        DMI_NAME_OTHER(DMI_MEMORY_INTERLEAVE_OTHER),
        DMI_NAME_UNKNOWN(DMI_MEMORY_INTERLEAVE_UNKNOWN),
        {
            .id   = DMI_MEMORY_INTERLEAVE_1WAY,
            .code = "1-way",
            .name = "One-Way Interleave"
        },
        {
            .id   = DMI_MEMORY_INTERLEAVE_2WAY,
            .code = "2-way",
            .name = "Two-Way Interleave"
        },
        {
            .id   = DMI_MEMORY_INTERLEAVE_4WAY,
            .code = "4-way",
            .name = "Four-Way Interleave"
        },
        {
            .id   = DMI_MEMORY_INTERLEAVE_8WAY,
            .code = "8-way",
            .name = "Eight-Way Interleave"
        },
        {
            .id   = DMI_MEMORY_INTERLEAVE_16WAY,
            .code = "16-way",
            .name = "Sixteen-Way Interleave"
        },
        {}
    })
};

const dmi_name_set_t dmi_memory_module_voltage_names =
{
    .code  = "memory-module-voltage",
    .names = DMI_NAMES({
        {
            .id   = 0,
            .code = "5v",
            .name = "5V"
        },
        {
            .id   = 1,
            .code = "3v3",
            .name = "3.3V"
        },
        {
            .id   = 2,
            .code = "2v9",
            .name = "2.9V"
        },
        {}
    })
};

const char *dmi_error_detect_method_name(dmi_error_detect_method_t value)
{
    return dmi_name_lookup(&dmi_error_detect_method_names, (int)value);
}

const char *dmi_memory_interleave_name(dmi_memory_interleave_t value)
{
    return dmi_name_lookup(&dmi_memory_interleave_names, (int)value);
}
