//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>

#include "hardware-security-internal.h"

const dmi_name_set_t dmi_hardware_security_status_names =
{
    .code  = "hardware-security-status",
    .names = DMI_NAMES({
        {
            .id   = DMI_HARDWARE_SECURITY_STATUS_DISABLED,
            .code = "disabled",
            .name = "Disabled"
        },
        {
            .id   = DMI_HARDWARE_SECURITY_STATUS_ENABLED,
            .code = "enabled",
            .name = "Enabled"
        },
        {
            .id   = DMI_HARDWARE_SECURITY_STATUS_NOT_IMPLEMENTED,
            .code = "not-implemented",
            .name = "Not implemented"
        },
        DMI_NAME_UNKNOWN(DMI_HARDWARE_SECURITY_STATUS_UNKNOWN),
        {}
    })
};

DMI_NAME_FUNCTION(dmi_hardware_security_status)
