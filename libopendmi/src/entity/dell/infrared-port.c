//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/dell.h>
#include <opendmi/entity/dell/infrared-port.h>

static bool dmi_dell_infrared_port_decode(dmi_entity_t *entity);

static const dmi_name_set_t dmi_dell_infrared_proto_names =
{
    .code = "dell-infrared-protocols",
    .names = (const dmi_name_t[]) {
        DMI_NAME_UNSPEC(DMI_DELL_INFRARED_PROTO_UNSPEC),
        DMI_NAME_OTHER(DMI_DELL_INFRARED_PROTO_OTHER),
        DMI_NAME_UNKNOWN(DMI_DELL_INFRARED_PROTO_UNKNOWN),
        {
            .id   = DMI_DELL_INFRARED_PROTO_SIR,
            .code = "sir",
            .name = "SIR (Standard IR)"
        },
        {
            .id   = DMI_DELL_INFRARED_PROTO_FIR,
            .code = "fir",
            .name = "FIR (Fast IR)"
        },
        {
            .id   = DMI_DELL_INFRARED_PROTO_MIR,
            .code = "mir",
            .name = "MIR (Medium Speed IR)"
        },
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_dell_infrared_port_spec =
{
    .type            = DMI_TYPE(DELL_INFRARED_PORT),
    .code            = "dell-infrared-port",
    .name            = "Dell infrared port",
    .minimum_version = DMI_VERSION(2, 2, 0),
    .minimum_length  = 0x0D,
    .decoded_length  = sizeof(dmi_dell_infrared_port_t),
    .attributes      = (const dmi_attribute_t[]){
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
            .unit    = "baud"
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
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_dell_infrared_port_decode
    }
};

const char *dmi_dell_infrared_proto_name(dmi_dell_infrared_proto_t value)
{
    return dmi_name_lookup(&dmi_dell_infrared_proto_names, (int)value);
}

static bool dmi_dell_infrared_port_decode(dmi_entity_t *entity)
{
    dmi_dell_infrared_port_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(DELL_INFRARED_PORT));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;
    dmi_word_t speed_limit = 0;

    bool status =
        dmi_stream_decode_str(stream, &info->location) and
        dmi_stream_decode(stream, dmi_byte_t, &info->state) and
        dmi_stream_decode(stream, dmi_byte_t, &info->speed_limit_state) and
        dmi_stream_decode(stream, dmi_word_t, &speed_limit) and
        dmi_stream_decode_str(stream, &info->physical_port) and
        dmi_stream_decode_str(stream, &info->virtual_com_port) and
        dmi_stream_decode_str(stream, &info->virtual_lpt_port) and
        dmi_stream_decode(stream, dmi_byte_t, &info->protocol);

    info->speed_limit = (uint32_t)speed_limit * 100;

    return status;
}
