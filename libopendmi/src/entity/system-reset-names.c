//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/stream.h>
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <limits.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/system-reset-internal.h>

const dmi_name_set_t dmi_boot_option_names =
{
    .code  = "boot-option",
    .names = DMI_NAMES({
        DMI_NAME_RESERVED(DMI_BOOT_OPTION_RESERVED),
        {
            .id   = DMI_BOOT_OPTION_OPERATING_SYSTEM,
            .code = "operating-system",
            .name = "Operating system"
        },
        {
            .id   = DMI_BOOT_OPTION_SYSTEM_UTILS,
            .code = "system-utils",
            .name = "System utilities"
        },
        {
            .id   = DMI_BOOT_OPTION_AVOID_REBOOT,
            .code = "avoid-reboot",
            .name = "Do not reboot"
        },
        {}
    })
};

const char *dmi_boot_option_name(dmi_boot_option_t value)
{
    return dmi_name_lookup(&dmi_boot_option_names, (int)value);
}
