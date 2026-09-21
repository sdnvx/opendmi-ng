//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/parallel-port-internal.h>

const dmi_entity_spec_t dmi_dell_parallel_port_spec =
{
    .type = DMI_TYPE(DELL_PARALLEL_PORT),
    .code = "dell-parallel-port",
    .name = "Dell parallel port",

    .params = {
        .minimum_version = DMI_VERSION(2, 2, 0),
        .minimum_length  = 0x0C,
        .decoded_length  = sizeof(dmi_dell_parallel_port_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_dell_parallel_port_t, base_addr,            WORD),
        DMI_FIELD(dmi_dell_parallel_port_t, irq_number,           BYTE),
        DMI_FIELD(dmi_dell_parallel_port_t, connector,            BYTE),
        DMI_FIELD(dmi_dell_parallel_port_t, pinout,               BYTE),
        DMI_FIELD(dmi_dell_parallel_port_t, capabilities.__value, WORD),
        DMI_FIELD(dmi_dell_parallel_port_t, security,             BYTE),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
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
        {}
    }),
};
