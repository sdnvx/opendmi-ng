//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>

#include <opendmi/entity/memory-error.h>

const dmi_name_set_t dmi_memory_error_type_names =
{
    .code  = "memory-error-type",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_MEMORY_ERROR_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_MEMORY_ERROR_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_MEMORY_ERROR_TYPE_UNKNOWN),
        {
            .id   = DMI_MEMORY_ERROR_TYPE_OK,
            .code = "ok",
            .name = "OK"
        },
        {
            .id   = DMI_MEMORY_ERROR_TYPE_BAD_READ,
            .code = "bad-read",
            .name = "Bad read"
        },
        {
            .id   = DMI_MEMORY_ERROR_TYPE_PARITY,
            .code = "parity",
            .name = "Parity error"
        },
        {
            .id   = DMI_MEMORY_ERROR_TYPE_SINGLE_BIT,
            .code = "single-bit",
            .name = "Single-bit error"
        },
        {
            .id   = DMI_MEMORY_ERROR_TYPE_DOUBLE_BIT,
            .code = "double-bit",
            .name = "Double-bit error"
        },
        {
            .id   = DMI_MEMORY_ERROR_TYPE_MULTI_BIT,
            .code = "multi-bit",
            .name = "Multi-bit error"
        },
        {
            .id   = DMI_MEMORY_ERROR_TYPE_NIBBLE,
            .code = "nibble",
            .name = "Nibble error"
        },
        {
            .id   = DMI_MEMORY_ERROR_TYPE_CHECKSUM,
            .code = "checksum",
            .name = "Checksum error"
        },
        {
            .id   = DMI_MEMORY_ERROR_TYPE_CRC,
            .code = "crc",
            .name = "CRC error"
        },
        {
            .id   = DMI_MEMORY_ERROR_TYPE_CORRECTED_SINGLE_BIT,
            .code = "corrected-single-bit",
            .name = "Corrected single-bit error"
        },
        {
            .id   = DMI_MEMORY_ERROR_TYPE_CORRECTED,
            .code = "corrected",
            .name = "Corrected error"
        },
        {
            .id   = DMI_MEMORY_ERROR_TYPE_UNCORRECTABLE,
            .code = "uncorrectable",
            .name = "Uncorrectable error"
        },
        {}
    })
};

const dmi_name_set_t dmi_memory_error_granularity_names =
{
    .code  = "memory-error-granularity",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_MEMORY_ERROR_GRANULARITY_UNSPEC),
        DMI_NAME_OTHER(DMI_MEMORY_ERROR_GRANULARITY_OTHER),
        DMI_NAME_UNKNOWN(DMI_MEMORY_ERROR_GRANULARITY_UNKNOWN),
        {
            .id   = DMI_MEMORY_ERROR_GRANULARITY_DEVICE,
            .code = "device",
            .name = "Device level"
        },
        {
            .id   = DMI_MEMORY_ERROR_GRANULARITY_PARTITION,
            .code = "partition",
            .name = "Memory partition level"
        },
        {}
    })
};

const dmi_name_set_t dmi_memory_error_operation_names =
{
    .code  = "memory-error-operation",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_MEMORY_ERROR_OPERATION_UNSPEC),
        DMI_NAME_OTHER(DMI_MEMORY_ERROR_OPERATION_OTHER),
        DMI_NAME_UNKNOWN(DMI_MEMORY_ERROR_OPERATION_UNKNOWN),
        {
            .id   = DMI_MEMORY_ERROR_OPERATION_READ,
            .code = "read",
            .name = "Read"
        },
        {
            .id   = DMI_MEMORY_ERROR_OPERATION_WRITE,
            .code = "write",
            .name = "Write"
        },
        {
            .id   = DMI_MEMORY_ERROR_OPERATION_PARTIAL_WRITE,
            .code = "partial-write",
            .name = "Partial write"
        },
        {}
    })
};

const char *dmi_memory_error_type_name(dmi_memory_error_type_t value)
{
    return dmi_name_lookup(&dmi_memory_error_type_names, (int)value);
}

const char *dmi_memory_error_granularity_name(dmi_memory_error_granularity_t value)
{
    return dmi_name_lookup(&dmi_memory_error_granularity_names, (int)value);
}

const char *dmi_memory_error_operation_name(dmi_memory_error_operation_t value)
{
    return dmi_name_lookup(&dmi_memory_error_operation_names, (int)value);
}
