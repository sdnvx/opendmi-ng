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
#include <opendmi/utils/codec.h>

#include <opendmi/entity/mgmt-device-internal.h>

const dmi_name_set_t dmi_mgmt_device_type_names =
{
    .code  = "mgmt-device-type",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_MGMT_DEVICE_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_MGMT_DEVICE_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_MGMT_DEVICE_TYPE_UNKNOWN),
        {
            .id   = DMI_MGMT_DEVICE_TYPE_NATIONAL_LM75,
            .code = "national-lm75",
            .name = "National Semiconductor LM75"
        },
        {
            .id   = DMI_MGMT_DEVICE_TYPE_NATIONAL_LM78,
            .code = "national-lm78",
            .name = "National Semiconductor LM78"
        },
        {
            .id   = DMI_MGMT_DEVICE_TYPE_NATIONAL_LM79,
            .code = "national-lm79",
            .name = "National Semiconductor LM79"
        },
        {
            .id   = DMI_MGMT_DEVICE_TYPE_NATIONAL_LM80,
            .code = "national-lm80",
            .name = "National Semiconductor LM80"
        },
        {
            .id   = DMI_MGMT_DEVICE_TYPE_NATIONAL_LM81,
            .code = "national-lm81",
            .name = "National Semiconductor LM81"
        },
        {
            .id   = DMI_MGMT_DEVICE_TYPE_ANALOG_ADM9240,
            .code = "analog-adm9240",
            .name = "Analog Devices ADM9240"
        },
        {
            .id   = DMI_MGMT_DEVICE_TYPE_DALLAS_DS1780,
            .code = "dallas-ds1780",
            .name = "Dallas Semiconductor DS1780"
        },
        {
            .id   = DMI_MGMT_DEVICE_TYPE_MAXIM_1617,
            .code = "maxim-1617",
            .name = "Maxim 1617"
        },
        {
            .id   = DMI_MGMT_DEVICE_TYPE_GENESYS_GL518SM,
            .code = "genesys-gl518sm",
            .name = "Genesys GL518SM"
        },
        {
            .id   = DMI_MGMT_DEVICE_TYPE_WINBOND_W83781D,
            .code = "winbond-w83781d",
            .name = "Winbond W83781D"
        },
        {
            .id   = DMI_MGMT_DEVICE_TYPE_HOLTEK_HT82H791,
            .code = "holtek-ht82h791",
            .name = "Holtek HT82H791"
        },
        {}
    })
};

const dmi_name_set_t dmi_mgmt_device_addr_type_names =
{
    .code  = "mgmt-device-addr-type",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_MGMT_DEVICE_ADDR_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_MGMT_DEVICE_ADDR_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_MGMT_DEVICE_ADDR_TYPE_UNKNOWN),
        {
            .id   = DMI_MGMT_DEVICE_ADDR_TYPE_PORT,
            .code = "port",
            .name = "I/O port"
        },
        {
            .id   = DMI_MGMT_DEVICE_ADDR_TYPE_MEMORY,
            .code = "memory",
            .name = "Memory"
        },
        {
            .id   = DMI_MGMT_DEVICE_ADDR_TYPE_SMBUS,
            .code = "smbus",
            .name = "SMBus"
        },
        {}
    })
};

const char *dmi_mgmt_device_type_name(dmi_mgmt_device_type_t value)
{
    return dmi_name_lookup(&dmi_mgmt_device_type_names, (int)value);
}

const char *dmi_mgmt_device_addr_type_name(dmi_mgmt_device_addr_type_t value)
{
    return dmi_name_lookup(&dmi_mgmt_device_addr_type_names, (int)value);
}
