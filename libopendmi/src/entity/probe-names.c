//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/reader.h>
#include <opendmi/internal.h>

#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/probe.h>

const dmi_name_set_t dmi_probe_location_names =
{
    .code  = "probe-location",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_PROBE_LOCATION_UNSPEC),
        DMI_NAME_OTHER(DMI_PROBE_LOCATION_OTHER),
        DMI_NAME_UNKNOWN(DMI_PROBE_LOCATION_UNKNOWN),
        {
            .id   = DMI_PROBE_LOCATION_PROCESSOR,
            .code = "processor",
            .name = "Processor"
        },
        {
            .id   = DMI_PROBE_LOCATION_DISK,
            .code = "disk",
            .name = "Disk"
        },
        {
            .id   = DMI_PROBE_LOCATION_PERIPHERAL_BAY,
            .code = "peripheral-bay",
            .name = "Peripheral bay"
        },
        {
            .id   = DMI_PROBE_LOCATION_SYSTEM_MGMT_MODULE,
            .code = "system-mgmt-module",
            .name = "System management module"
        },
        {
            .id   = DMI_PROBE_LOCATION_MOTHERBOARD,
            .code = "motherboard",
            .name = "Motherboard"
        },
        {
            .id   = DMI_PROBE_LOCATION_MEMORY_MODULE,
            .code = "memory-module",
            .name = "Memory module"
        },
        {
            .id   = DMI_PROBE_LOCATION_PROCESSOR_MODULE,
            .code = "processor-module",
            .name = "Processor module"
        },
        {
            .id   = DMI_PROBE_LOCATION_POWER_UNIT,
            .code = "power-unit",
            .name = "Power unit"
        },
        {
            .id   = DMI_PROBE_LOCATION_ADDIN_CARD,
            .code = "addin-card",
            .name = "Add-in card"
        },
        {
            .id   = DMI_PROBE_LOCATION_FRONT_PANEL_BOARD,
            .code = "front-panel-board",
            .name = "Front panel board"
        },
        {
            .id   = DMI_PROBE_LOCATION_BACK_PANEL_BOARD,
            .code = "back-panel-board",
            .name = "Back panel board"
        },
        {
            .id   = DMI_PROBE_LOCATION_POWER_SYSTEM_BOARD,
            .code = "power-system-board",
            .name = "Power system board"
        },
        {
            .id   = DMI_PROBE_LOCATION_DRIVE_BACK_PLANE,
            .code = "drive-back-plane",
            .name = "Drive back plane"
        },
        {}
    })
};

const char *dmi_probe_location_name(dmi_probe_location_t value)
{
    return dmi_name_lookup(&dmi_probe_location_names, (int)value);
}
