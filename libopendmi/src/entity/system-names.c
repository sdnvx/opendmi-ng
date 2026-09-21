//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/stream.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/system-internal.h>

const dmi_name_set_t dmi_system_wakeup_type_names =
{
    .code  = "system-wakeup-type",
    .names = DMI_NAMES({
        DMI_NAME_RESERVED(DMI_SYSTEM_WAKEUP_TYPE_RESERVED),
        DMI_NAME_OTHER(DMI_SYSTEM_WAKEUP_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_SYSTEM_WAKEUP_TYPE_UNKNOWN),
        {
            .id   = DMI_SYSTEM_WAKEUP_TYPE_APM_TIMER,
            .code = "apm-timer",
            .name = "APM Timer"
        },
        {
            .id   = DMI_SYSTEM_WAKEUP_TYPE_MODEM_RING,
            .code = "modem-ring",
            .name = "Modem Ring"
        },
        {
            .id   = DMI_SYSTEM_WAKEUP_TYPE_LAN_REMOTE,
            .code = "lan-remote",
            .name = "LAN Remote"
        },
        {
            .id = DMI_SYSTEM_WAKEUP_TYPE_POWER_SWITCH,
            .code = "power-switch",
            .name = "Power Switch"
        },
        {
            .id   = DMI_SYSTEM_WAKEUP_TYPE_PCI_PME,
            .code = "pci-pme",
            .name = "PCI PME#"
        },
        {
            .id   = DMI_SYSTEM_WAKEUP_TYPE_POWER_RESTORE,
            .code = "power-restore",
            .name = "AC Power Restored"
        },
        {}
    })
};

const char *dmi_system_wakeup_type_name(dmi_system_wakeup_type_t value)
{
    return dmi_name_lookup(&dmi_system_wakeup_type_names, (int)value);
}
