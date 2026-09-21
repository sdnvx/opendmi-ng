//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/value.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/registry.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/cooling-device-internal.h>

const dmi_name_set_t dmi_cooling_device_type_names =
{
    .code  = "cooling-device-type",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_COOLING_DEVICE_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_COOLING_DEVICE_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_COOLING_DEVICE_TYPE_UNKNOWN),
        {
            .id   = DMI_COOLING_DEVICE_TYPE_FAN,
            .code = "fan",
            .name = "Fan"
        },
        {
            .id   = DMI_COOLING_DEVICE_TYPE_CENTRIFUGAL_BLOWER,
            .code = "centrifugal-blower",
            .name = "Centrifugal blower"
        },
        {
            .id   = DMI_COOLING_DEVICE_TYPE_CHIP_FAN,
            .code = "chip-fan",
            .name = "Chip fan"
        },
        {
            .id   = DMI_COOLING_DEVICE_TYPE_CABINET_FAN,
            .code = "cabinet-fan",
            .name = "Cabinet fan"
        },
        {
            .id   = DMI_COOLING_DEVICE_TYPE_POWER_SUPPLY_FAN,
            .code = "power-supply-fan",
            .name = "Power supply fan"
        },
        {
            .id   = DMI_COOLING_DEVICE_TYPE_HEAT_PIPE,
            .code = "heat-pipe",
            .name = "Heat pipe"
        },
        {
            .id   = DMI_COOLING_DEVICE_TYPE_INTEGRATED_REFRIGERATION,
            .code = "integrated-refrigeration",
            .name = "Integrated refrigeration"
        },
        {
            .id   = DMI_COOLING_DEVICE_TYPE_ACTIVE_COOLING,
            .code = "active-cooling",
            .name = "Active cooling"
        },
        {
            .id   = DMI_COOLING_DEVICE_TYPE_PASSIVE_COOLING,
            .code = "passive-cooling",
            .name = "Passive cooling"
        },
        {}
    })
};

const char *dmi_cooling_device_type_name(dmi_cooling_device_type_t value)
{
    return dmi_name_lookup(&dmi_cooling_device_type_names, (int)value);
}
