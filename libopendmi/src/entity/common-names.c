//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>
#include <opendmi/entity/common.h>

const dmi_name_set_t dmi_status_names =
{
    .code  = "status",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_STATUS_UNSPEC),
        DMI_NAME_OTHER(DMI_STATUS_OTHER),
        DMI_NAME_UNKNOWN(DMI_STATUS_UNKNOWN),
        {
            .id   = DMI_STATUS_OK,
            .code = "ok",
            .name = "OK"
        },
        {
            .id   = DMI_STATUS_NON_CRITICAL,
            .code = "non-critical",
            .name = "Non-critical"
        },
        {
            .id   = DMI_STATUS_CRITICAL,
            .code = "critical",
            .name = "Critical"
        },
        {
            .id   = DMI_STATUS_NON_RECOVERABLE,
            .code = "non-recoverable",
            .name = "Non-recoverable"
        },
        {}
    })
};

const dmi_name_set_t dmi_error_correct_type_names =
{
    .code  = "error-correction-type",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_ERROR_CORRECT_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_ERROR_CORRECT_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_ERROR_CORRECT_TYPE_UNKNOWN),
        DMI_NAME_NONE(DMI_ERROR_CORRECT_TYPE_NONE),
        {
            .id   = DMI_ERROR_CORRECT_TYPE_PARITY,
            .code = "parity",
            .name = "Parity"
        },
        {
            .id   = DMI_ERROR_CORRECT_TYPE_SINGLE_BIT,
            .code = "single-bit",
            .name = "Single-bit ECC"
        },
        {
            .id   = DMI_ERROR_CORRECT_TYPE_MULTI_BIT,
            .code = "multi-bit",
            .name = "Multi-bit ECC"
        },
        {
            .id   = DMI_ERROR_CORRECT_TYPE_CRC,
            .code = "crc",
            .name = "CRC"
        },
        {}
    })
};

const char *dmi_status_name(dmi_status_t value)
{
    return dmi_name_lookup(&dmi_status_names, (int)value);
}

const char *dmi_error_correct_type_name(dmi_error_correct_type_t value)
{
    return dmi_name_lookup(&dmi_error_correct_type_names, (int)value);
}
