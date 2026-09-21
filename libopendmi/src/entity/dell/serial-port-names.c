//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/serial-port-internal.h>

const dmi_name_set_t dmi_dell_serial_port_connector_type_names =
{
    .code  = "dell-serial-port-connector-type",
    .names = DMI_NAMES({
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
        {}
    })
};

const dmi_name_set_t dmi_dell_serial_port_caps_names =
{
    .code  = "dell-serial-port-capability",
    .names = DMI_NAMES({
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
        {}
    })
};

const char *dmi_dell_serial_port_connector_type_name(dmi_dell_serial_port_connector_type_t value)
{
    return dmi_name_lookup(&dmi_dell_serial_port_connector_type_names, (int)value);
}

const char *dmi_dell_serial_port_caps_name(dmi_dell_serial_port_caps_t value)
{
    return dmi_name_lookup(&dmi_dell_serial_port_caps_names, (int)value);
}
