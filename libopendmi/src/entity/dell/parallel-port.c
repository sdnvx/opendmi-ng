//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/dell.h>
#include <opendmi/entity/dell/parallel-port.h>

static bool dmi_dell_parallel_port_decode(dmi_entity_t *entity);

static const dmi_name_set_t dmi_dell_parallel_port_connector_type_names =
{
    .code  = "dell-parallel-port-connector-types",
    .names = (const dmi_name_t[]) {
        DMI_NAME_UNSPEC(DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_UNKNOWN),
        {
            .id   = DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_DB_25_F,
            .code = "db-25-f",
            .name = "DB-25 female IEEE 1284-A receptacle"
        },
        {
            .id   = DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_DB_25_M,
            .code = "db-25-m",
            .name = "DB-25 male IEEE 1284-A plug"
        },
        {
            .id   = DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_CN_36,
            .code = "cn-36",
            .name = "Centronics IEEE 1284-B receptacle"
        },
        {
            .id   = DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_HPCN_36,
            .code = "hpcn-36",
            .name = "Mini-Centronics IEEE 1284-C receptacle"
        },
        {
            .id   = DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_PROPRIETARY,
            .code = "proprietary",
            .name = "Proprietary"
        },
        {
            .id   = DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_CN_14,
            .code = "cn-14",
            .name = "Centronics-14"
        },
        {
            .id   = DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_DB_36_F,
            .code = "db-36-f",
            .name = "DB-36 female"
        },
        {
            .id   = DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_HPCN_20,
            .code = "hpcn-20",
            .name = "Mini-Centronics-20"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_dell_parallel_port_connector_pinout_names =
{
    .code  = "dell-parallel-port-connector-pinouts",
    .names = (const dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_UNSPEC),
        DMI_NAME_OTHER(DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_OTHER),
        DMI_NAME_UNKNOWN(DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_UNKNOWN),

        {
            .id   = DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_XT_AT,
            .code = "xt-at",
            .name = "XT/AT"
        },
        {
            .id   = DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_PS2,
            .code = "ps2",
            .name = "PS/2"
        },
        {
            .id   = DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_IEEE_1284,
            .code = "ieee-1284",
            .name = "IEEE 1284"
        },
        {
            .id   = DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_PROPRIETARY,
            .code = "proprietary",
            .name = "Proprietary"
        },
        {
            .id   = DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_PC_98,
            .code = "pc-98",
            .name = "PC-98"
        },
        {
            .id   = DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_PC_98_HIRESO,
            .code = "pc-98-hireso",
            .name = "PC-98 Hireso"
        },
        {
            .id   = DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_PC_H98,
            .code = "pc-h98",
            .name = "PC-H98"
        },
        {
            .id   = DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_PC_98_NOTE,
            .code = "pc-98-note",
            .name = "PC-98 Note"
        },
        {
            .id   = DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_PC_98_FULL,
            .code = "pc-98-full",
            .name = "PC-98 Full"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_dell_parallel_port_caps_names =
{
    .code  = "dell-parallel-port-caps",
    .names = (const dmi_name_t[]){
        {
            .id   = 0,
            .code = "xt-at",
            .name = "XT/AT"
        },
        {
            .id   = 1,
            .code = "ps2",
            .name = "PS/2"
        },
        {
            .id   = 2,
            .code = "ecp",
            .name = "ECP"
        },
        {
            .id   = 3,
            .code = "epp",
            .name = "EPP"
        },
        {
            .id   = 4,
            .code = "pc-98",
            .name = "PC-98"
        },
        {
            .id   = 5,
            .code = "pc-98-hireso",
            .name = "PC-98 Hireso"
        },
        {
            .id   = 6,
            .code = "pc-h98",
            .name = "PC-H98"
        },
        {
            .id   = 15,
            .code = "dma-support",
            .name = "DMA support"
        },
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_dell_parallel_port_spec =
{
    .type            = DMI_TYPE(DELL_PARALLEL_PORT),
    .code            = "dell-parallel-port",
    .name            = "Dell parallel port",
    .minimum_version = DMI_VERSION(2, 2, 0),
    .minimum_length  = 0x0C,
    .decoded_length  = sizeof(dmi_dell_parallel_port_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_dell_parallel_port_t, base_addr, ADDRESS, {
            .code    = "base-address",
            .name    = "Base address",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_parallel_port_t, irq_number, INTEGER, {
            .code    = "irq-number",
            .name    = "IRQ number"
        }),
        DMI_ATTRIBUTE(dmi_dell_parallel_port_t, connector, ENUM, {
            .code    = "connector",
            .name    = "Connector type",
            .unspec  = dmi_value_ptr(DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_UNKNOWN),
            .values  = &dmi_dell_parallel_port_connector_type_names
        }),
        DMI_ATTRIBUTE(dmi_dell_parallel_port_t, pinout, ENUM, {
            .code    = "pinout",
            .name    = "Connector pinout",
            .unspec  = dmi_value_ptr(DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_UNSPEC),
            .unknown = dmi_value_ptr(DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_UNKNOWN),
            .values  = &dmi_dell_parallel_port_connector_pinout_names
        }),
        DMI_ATTRIBUTE(dmi_dell_parallel_port_t, capabilities, SET, {
            .code    = "capabilities",
            .name    = "Capabilities",
            .values  = &dmi_dell_parallel_port_caps_names
        }),
        DMI_ATTRIBUTE(dmi_dell_parallel_port_t, security, ENUM, {
            .code    = "security",
            .name    = "Security",
            .unspec  = dmi_value_ptr(DMI_DELL_PORT_SECURITY_UNSPEC),
            .unknown = dmi_value_ptr(DMI_DELL_PORT_SECURITY_UNKNOWN),
            .values  = &dmi_dell_port_security_names
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_dell_parallel_port_decode
    }
};

const char *dmi_dell_parallel_port_connector_type_name(
        dmi_dell_parallel_port_connector_type_t value)
{
    return dmi_name_lookup(&dmi_dell_parallel_port_connector_type_names, (int)value);
}

const char *dmi_dell_parallel_port_connector_pinout_name(
        dmi_dell_parallel_port_connector_pinout_t value)
{
    return dmi_name_lookup(&dmi_dell_parallel_port_connector_pinout_names, (int)value);
}

static bool dmi_dell_parallel_port_decode(dmi_entity_t *entity)
{
    dmi_dell_parallel_port_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(DELL_PARALLEL_PORT));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    bool status =
        dmi_stream_decode(stream, dmi_word_t, &info->base_addr) and
        dmi_stream_decode(stream, dmi_byte_t, &info->irq_number) and
        dmi_stream_decode(stream, dmi_byte_t, &info->connector) and
        dmi_stream_decode(stream, dmi_byte_t, &info->pinout) and
        dmi_stream_decode(stream, dmi_word_t, &info->capabilities.__value) and
        dmi_stream_decode(stream, dmi_byte_t, &info->security);

    return status;
}
