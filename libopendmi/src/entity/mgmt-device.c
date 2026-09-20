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

#include <opendmi/entity/mgmt-device.h>

static bool dmi_mgmt_device_decode(dmi_entity_t *entity);

static const dmi_name_set_t dmi_mgmt_device_type_names =
{
    .code  = "mgmt-device-type",
    .names = (dmi_name_t[]){
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
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_mgmt_device_addr_type_names =
{
    .code  = "mgmt-device-addr-type",
    .names = (dmi_name_t[]){
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
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_mgmt_device_spec =
{
    .code            = "mgmt-device",
    .name            = "Management device",
    .description     = (const char *[]){
        "The information in this structure defines the attributes of a "
        "Management Device. A Management Device might control one or more "
        "fans or voltage, current, or temperature probes as defined by one "
        "or more Management Device Component structures.",
        //
        nullptr
    },
    .type            = DMI_TYPE(MGMT_DEVICE),
    .minimum_version = DMI_VERSION(2, 3, 0),
    .minimum_length  = 0x0B,
    .decoded_length  = sizeof(dmi_mgmt_device_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_mgmt_device_t, description, STRING, {
            .code    = "description",
            .name    = "Description"
        }),
        DMI_ATTRIBUTE(dmi_mgmt_device_t, type, ENUM, {
            .code    = "type",
            .name    = "Type",
            .unspec  = dmi_value_ptr(DMI_MGMT_DEVICE_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_MGMT_DEVICE_TYPE_UNKNOWN),
            .values  = &dmi_mgmt_device_type_names
        }),
        DMI_ATTRIBUTE(dmi_mgmt_device_t, addr, ADDRESS, {
            .code    = "address",
            .name    = "Address",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_mgmt_device_t, addr_type, ENUM, {
            .code    = "address-type",
            .name    = "Address type",
            .unspec  = dmi_value_ptr(DMI_MGMT_DEVICE_ADDR_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_MGMT_DEVICE_ADDR_TYPE_UNKNOWN),
            .values  = &dmi_mgmt_device_addr_type_names
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_mgmt_device_decode
    }
};

const char *dmi_mgmt_device_type_name(dmi_mgmt_device_type_t value)
{
    return dmi_name_lookup(&dmi_mgmt_device_type_names, (int)value);
}

const char *dmi_mgmt_device_addr_type_name(dmi_mgmt_device_addr_type_t value)
{
    return dmi_name_lookup(&dmi_mgmt_device_addr_type_names, (int)value);
}

static bool dmi_mgmt_device_decode(dmi_entity_t *entity)
{
    dmi_mgmt_device_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MGMT_DEVICE));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = dmi_entity_stream(entity);

    return
        dmi_stream_decode_str(stream, &info->description) and
        dmi_stream_decode(stream, dmi_byte_t, &info->type) and
        dmi_stream_decode(stream, dmi_dword_t, &info->addr) and
        dmi_stream_decode(stream, dmi_byte_t, &info->addr_type);
}
