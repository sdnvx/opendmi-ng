//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/infrared-port-internal.h>

const dmi_entity_spec_t dmi_dell_infrared_port_spec =
{
    .type = DMI_TYPE(DELL_INFRARED_PORT),
    .code = "dell-infrared-port",
    .name = "Dell infrared port",

    .params = {
        .minimum_version = DMI_VERSION(2, 2, 0),
        .minimum_length  = 0x0D,
        .decoded_length  = sizeof(dmi_dell_infrared_port_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_dell_infrared_port_t, location,          STRING),
        DMI_FIELD(dmi_dell_infrared_port_t, state,             BYTE),
        DMI_FIELD(dmi_dell_infrared_port_t, speed_limit_state, BYTE),
        DMI_FIELD(dmi_dell_infrared_port_t, speed_limit,       WORD,
                  .convert = dmi_dell_infrared_port_convert_speed),
        DMI_FIELD(dmi_dell_infrared_port_t, physical_port,     STRING),
        DMI_FIELD(dmi_dell_infrared_port_t, virtual_com_port,  STRING),
        DMI_FIELD(dmi_dell_infrared_port_t, virtual_lpt_port,  STRING),
        DMI_FIELD(dmi_dell_infrared_port_t, protocol,          BYTE),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_dell_infrared_port_t, location, STRING, {
            .code    = "location",
            .name    = "Location"
        }),
        DMI_ATTRIBUTE(dmi_dell_infrared_port_t, state, ENUM, {
            .code    = "state",
            .name    = "State",
            .unspec  = dmi_value_ptr(DMI_DELL_ENABLE_STATE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_DELL_ENABLE_STATE_UNKNOWN),
            .values  = &dmi_dell_enable_state_names
        }),
        DMI_ATTRIBUTE(dmi_dell_infrared_port_t, speed_limit_state, ENUM, {
            .code    = "speed-limit-state",
            .name    = "Speed limit state",
            .unspec  = dmi_value_ptr(DMI_DELL_ENABLE_STATE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_DELL_ENABLE_STATE_UNKNOWN),
            .values  = &dmi_dell_enable_state_names
        }),
        DMI_ATTRIBUTE(dmi_dell_infrared_port_t, speed_limit, INTEGER, {
            .code    = "speed-limit",
            .name    = "Speed limit",
            .unit    = DMI_UNIT_BAUD
        }),
        DMI_ATTRIBUTE(dmi_dell_infrared_port_t, physical_port, STRING, {
            .code    = "physical-port",
            .name    = "Physical port"
        }),
        DMI_ATTRIBUTE(dmi_dell_infrared_port_t, virtual_com_port, STRING, {
            .code    = "virtual-com-port",
            .name    = "Virtual COM port"
        }),
        DMI_ATTRIBUTE(dmi_dell_infrared_port_t, virtual_lpt_port, STRING, {
            .code    = "virtual-lpt-port",
            .name    = "Virtual LPT port"
        }),
        DMI_ATTRIBUTE(dmi_dell_infrared_port_t, protocol, ENUM, {
            .code    = "protocol",
            .name    = "Protocol",
            .unspec  = dmi_value_ptr(DMI_DELL_INFRARED_PROTO_UNSPEC),
            .unknown = dmi_value_ptr(DMI_DELL_INFRARED_PROTO_UNKNOWN),
            .values  = &dmi_dell_infrared_proto_names
        }),
        {}
    })
};
