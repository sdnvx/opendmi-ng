//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include <opendmi/entity/hpe/power-supply-internal.h>

const dmi_entity_spec_t dmi_hpe_power_supply_spec =
{
    .type        = DMI_TYPE(HPE_POWER_SUPPLY),
    .code        = "hpe-power-supply",
    .name        = "HP/HPE power supply information",
    .description = (const char *[]){
        "Supplements the system power supply information (type 39) with the "
        "actual manufacturer of the power supply and the way its FRU is "
        "accessed.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x0B,
        .decoded_length = sizeof(dmi_hpe_power_supply_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_hpe_power_supply_t, power_supply_handle, dmi_word_t),
        DMI_FIELD_STRING(dmi_hpe_power_supply_t, manufacturer),
        DMI_FIELD_STRING(dmi_hpe_power_supply_t, revision),
        DMI_FIELD(dmi_hpe_power_supply_t, fru_access,  dmi_byte_t),
        DMI_FIELD(dmi_hpe_power_supply_t, i2c_bus,     dmi_byte_t),
        DMI_FIELD(dmi_hpe_power_supply_t, i2c_address, dmi_byte_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_power_supply_t, power_supply_handle, HANDLE, {
            .code    = "power-supply-handle",
            .name    = "Power supply handle",
            .targets = dmi_types(DMI_TYPE_POWER_SUPPLY)
        }),
        DMI_ATTRIBUTE(dmi_hpe_power_supply_t, manufacturer, STRING, {
            .code = "manufacturer",
            .name = "Manufacturer"
        }),
        DMI_ATTRIBUTE(dmi_hpe_power_supply_t, revision, STRING, {
            .code = "revision",
            .name = "Revision"
        }),
        DMI_ATTRIBUTE(dmi_hpe_power_supply_t, fru_access, ENUM, {
            .code   = "fru-access",
            .name   = "FRU access method",
            .values = &dmi_hpe_fru_access_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_power_supply_t, i2c_bus, INTEGER, {
            .code   = "i2c-bus",
            .name   = "I2C bus or segment",
            .unspec = dmi_value_ptr((uint8_t)UINT8_MAX)
        }),
        DMI_ATTRIBUTE(dmi_hpe_power_supply_t, i2c_address, INTEGER, {
            .code   = "i2c-address",
            .name   = "I2C address",
            .unspec = dmi_value_ptr((uint8_t)UINT8_MAX),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        {}
    })
};

const dmi_name_set_t dmi_hpe_fru_access_names =
{
    .code  = "hpe-fru-access",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_FRU_ACCESS_NONE,
            .code = "none",
            .name = "Not available"
        },
        {
            .id   = DMI_HPE_FRU_ACCESS_IPMI,
            .code = "ipmi",
            .name = "IPMI I2C"
        },
        {
            .id   = DMI_HPE_FRU_ACCESS_ILO,
            .code = "ilo",
            .name = "iLO"
        },
        {
            .id   = DMI_HPE_FRU_ACCESS_CHASSIS,
            .code = "chassis",
            .name = "Chassis manager"
        },
        {}
    })
};

const char *dmi_hpe_fru_access_name(dmi_hpe_fru_access_t value)
{
    return dmi_name_lookup(&dmi_hpe_fru_access_names, (int)value);
}
