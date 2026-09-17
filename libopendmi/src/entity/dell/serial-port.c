//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/dell.h>
#include <opendmi/entity/dell/serial-port.h>

static bool dmi_dell_serial_port_decode(dmi_entity_t *entity);

static const dmi_name_set_t dmi_dell_serial_port_connector_type_names =
{
    .code  = "dell-serial-port-connector-types",
    .names = (const dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_UNKNOWN),

        {
            .id   = DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_DB_9_M,
            .code = "db-9-m",
            .name = "DB-9 male"
        },
        {
            .id   = DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_DB_9_F,
            .code = "db-9-f",
            .name = "DB-9 female"
        },
        {
            .id   = DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_DB_25_M,
            .code = "db-25-m",
            .name = "DB-25 male"
        },
        {
            .id   = DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_DB_25_F,
            .code = "db-25-f",
            .name = "DB-25 female"
        },
        {
            .id   = DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_RJ_11,
            .code = "rj-11",
            .name = "RJ-11"
        },
        {
            .id   = DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_RJ_45,
            .code = "rj-45",
            .name = "RJ-45"
        },
        {
            .id   = DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_PROPRIETARY,
            .code = "proprietary",
            .name = "Proprietary"
        },
        {
            .id   = DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_CIRCULAR_DIN_8_M,
            .code = "circular-din-8-m",
            .name = "Circular DIN-8 male"
        },
        {
            .id = DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_CIRCULAR_DIN_8_F,
            .code = "circular-din-8-f",
            .name = "Circular DIN-8 female"
        },
        {
            .id   = DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_IEEE_1284_C_14,
            .code = "ieee-1284-c-14",
            .name = "Mini-Centronics Type-14"
        },
        {
            .id   = DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_IEEE_1284_C_26,
            .code = "ieee-1284-c-26",
            .name = "Mini-Centronics Type-26"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_dell_serial_port_caps_names =
{
    .code  = "dell-serial-port-capabilities",
    .names = (const dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_DELL_SERIAL_PORT_CAPS_UNSPEC),
        DMI_NAME_OTHER(DMI_DELL_SERIAL_PORT_CAPS_OTHER),
        DMI_NAME_UNKNOWN(DMI_DELL_SERIAL_PORT_CAPS_UNKNOWN),

        {
            .id   = DMI_DELL_SERIAL_PORT_CAPS_XT_AT,
            .code = "xt-at",
            .name = "XT/AT compatible"
        },
        {
            .id   = DMI_DELL_SERIAL_PORT_CAPS_16450,
            .code = "16450",
            .name = "16450 compatible"
        },
        {
            .id   = DMI_DELL_SERIAL_PORT_CAPS_16550,
            .code = "16550",
            .name = "16550 compatible"
        },
        {
            .id   = DMI_DELL_SERIAL_PORT_CAPS_16550A,
            .code = "16550a",
            .name = "16550A compatible"
        },
        {
            .id   = DMI_DELL_SERIAL_PORT_CAPS_8251,
            .code = "8251",
            .name = "8251 compatible"
        },
        {
            .id   = DMI_DELL_SERIAL_PORT_CAPS_8251_FIFO,
            .code = "8251-fifo",
            .name = "8251 FIFO compatible"
        },
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_dell_serial_port_spec =
{
    .type            = DMI_TYPE(DELL_SERIAL_PORT),
    .code            = "dell-serial-port",
    .name            = "Dell serial port",
    .minimum_version = DMI_VERSION(2, 2, 0),
    .minimum_length  = 0x0C,
    .decoded_length  = sizeof(dmi_dell_serial_port_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_dell_serial_port_t, base_addr, ADDRESS, {
            .code    = "base-address",
            .name    = "Base address",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_serial_port_t, irq_number, INTEGER, {
            .code    = "irq-number",
            .name    = "IRQ number"
        }),
        DMI_ATTRIBUTE(dmi_dell_serial_port_t, connector, ENUM, {
            .code    = "connector",
            .name    = "Connector type",
            .unspec  = dmi_value_ptr(DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_UNKNOWN),
            .values  = &dmi_dell_serial_port_connector_type_names
        }),
        DMI_ATTRIBUTE(dmi_dell_serial_port_t, capabilities, ENUM, {
            .code    = "capabilities",
            .name    = "Capabilities",
            .values  = &dmi_dell_serial_port_caps_names
        }),
        DMI_ATTRIBUTE(dmi_dell_serial_port_t, maximum_speed, INTEGER, {
            .code    = "maximum-speed",
            .name    = "Maximum speeed",
            .unit    = "baud"
        }),
        DMI_ATTRIBUTE(dmi_dell_serial_port_t, security, ENUM, {
            .code    = "security",
            .name    = "Security",
            .unspec  = dmi_value_ptr(DMI_DELL_PORT_SECURITY_UNSPEC),
            .unknown = dmi_value_ptr(DMI_DELL_PORT_SECURITY_UNKNOWN),
            .values  = &dmi_dell_port_security_names
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_dell_serial_port_decode
    }
};

const char *dmi_dell_serial_port_connector_type_name(dmi_dell_serial_port_connector_type_t value)
{
    return dmi_name_lookup(&dmi_dell_serial_port_connector_type_names, (int)value);
}

const char *dmi_dell_serial_port_caps_name(dmi_dell_serial_port_caps_t value)
{
    return dmi_name_lookup(&dmi_dell_serial_port_caps_names, (int)value);
}

static bool dmi_dell_serial_port_decode(dmi_entity_t *entity)
{
    dmi_dell_serial_port_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(DELL_SERIAL_PORT));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;
    dmi_word_t maximum_speed = 0;

    bool status =
        dmi_stream_decode(stream, dmi_word_t, &info->base_addr) and
        dmi_stream_decode(stream, dmi_byte_t, &info->irq_number) and
        dmi_stream_decode(stream, dmi_byte_t, &info->connector) and
        dmi_stream_decode(stream, dmi_byte_t, &info->capabilities) and
        dmi_stream_decode(stream, dmi_word_t, &maximum_speed) and
        dmi_stream_decode(stream, dmi_byte_t, &info->security);

    info->maximum_speed = (uint32_t)maximum_speed * 100;

    return status;
}
