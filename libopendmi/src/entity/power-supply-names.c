//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/power-supply-internal.h>

const dmi_name_set_t dmi_power_supply_type_names =
{
    .code  = "power-supply-type",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_POWER_SUPPLY_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_POWER_SUPPLY_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_POWER_SUPPLY_TYPE_UNKNOWN),
        {
            .id   = DMI_POWER_SUPPLY_TYPE_LINEAR,
            .code = "linear",
            .name = "Linear"
        },
        {
            .id   = DMI_POWER_SUPPLY_TYPE_SWITCHING,
            .code = "switching",
            .name = "Switching"
        },
        {
            .id   = DMI_POWER_SUPPLY_TYPE_BATTERY,
            .code = "battery",
            .name = "Battery"
        },
        {
            .id   = DMI_POWER_SUPPLY_TYPE_UPS,
            .code = "ups",
            .name = "UPS"
        },
        {
            .id   = DMI_POWER_SUPPLY_TYPE_CONVERTER,
            .code = "converter",
            .name = "Converter"
        },
        {
            .id   = DMI_POWER_SUPPLY_TYPE_REGULATOR,
            .code = "regulator",
            .name = "Regulator"
        },
        {}
    })
};

const dmi_name_set_t dmi_range_switching_type_names =
{
    .code  = "range-switching-type",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_RANGE_SWITCHING_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_RANGE_SWITCHING_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_RANGE_SWITCHING_TYPE_UNKNOWN),
        {
            .id   = DMI_RANGE_SWITCHING_TYPE_MANUAL,
            .code = "manual",
            .name = "Manual"
        },
        {
            .id   = DMI_RANGE_SWITCHING_TYPE_AUTO,
            .code = "auto",
            .name = "Auto-switch"
        },
        {
            .id   = DMI_RANGE_SWITCHING_TYPE_WIDE,
            .code = "wide-range",
            .name = "Wide range"
        },
        {
            .id   = DMI_RANGE_SWITCHING_TYPE_NOT_APPLICABLE,
            .code = "not-applicable",
            .name = "Not applicable"
        },
        {}
    })
};

const char *dmi_power_supply_type_name(dmi_power_supply_type_t value)
{
    return dmi_name_lookup(&dmi_power_supply_type_names, (int)value);
}

const char *dmi_range_switching_type_name(dmi_range_switching_type_t value)
{
    return dmi_name_lookup(&dmi_range_switching_type_names, (int)value);
}
