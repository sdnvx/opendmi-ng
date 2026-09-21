//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/firmware-language-internal.h>

const dmi_name_set_t dmi_firmware_language_flag_names =
{
    .code = "firmware-language-flag",
    .names = DMI_NAMES({
        {
            .id   = 0,
            .code = "is-abbreviated",
            .name = "Abbreviated"
        },
        {}
    })
};
