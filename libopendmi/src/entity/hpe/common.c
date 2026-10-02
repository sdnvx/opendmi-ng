//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>

#include "common-internal.h"

const dmi_name_set_t dmi_hpe_flag_names =
{
    .code  = "hpe-flag",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_FLAG_NO,
            .code = "no",
            .name = "No"
        },
        {
            .id   = DMI_HPE_FLAG_YES,
            .code = "yes",
            .name = "Yes"
        },
        {}
    })
};

const dmi_name_set_t dmi_hpe_encryption_names =
{
    .code  = "hpe-encryption",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_ENCRYPTION_NONE,
            .code = "none",
            .name = "Not encrypted"
        },
        {
            .id   = DMI_HPE_ENCRYPTION_ENCRYPTED,
            .code = "encrypted",
            .name = "Encrypted"
        },
        DMI_NAME_UNKNOWN(DMI_HPE_ENCRYPTION_UNKNOWN),
        {
            .id   = DMI_HPE_ENCRYPTION_UNSUPPORTED,
            .code = "unsupported",
            .name = "Not supported"
        },
        {}
    })
};

DMI_NAME_FUNCTION(dmi_hpe_flag)
DMI_NAME_FUNCTION(dmi_hpe_encryption)
