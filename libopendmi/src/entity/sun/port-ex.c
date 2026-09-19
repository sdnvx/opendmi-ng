//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/sun.h>
#include <opendmi/entity/sun/port-ex.h>

static bool dmi_sun_port_ex_decode(dmi_entity_t *entity);

const dmi_entity_spec_t dmi_sun_port_ex_spec =
{
    .type            = DMI_TYPE(SUN_PORT_EX),
    .code            = "sun-port-ex",
    .name            = "Sun port extended information",
    .description     = (const char *[]){
        "Extends the port connector information (type 8) with the chassis "
        "containing the port and the device connected to it.",
        //
        nullptr
    },
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x0C,
    .decoded_length  = sizeof(dmi_sun_port_ex_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_sun_port_ex_t, chassis_handle, HANDLE, {
            .code  = "chassis-handle",
            .name  = "Chassis handle"
        }),
        DMI_ATTRIBUTE(dmi_sun_port_ex_t, port_handle, HANDLE, {
            .code  = "port-handle",
            .name  = "Port connector handle"
        }),
        DMI_ATTRIBUTE(dmi_sun_port_ex_t, device_type, INTEGER, {
            .code  = "device-type",
            .name  = "Device type"
        }),
        DMI_ATTRIBUTE(dmi_sun_port_ex_t, device_handle, HANDLE, {
            .code  = "device-handle",
            .name  = "Device handle"
        }),
        DMI_ATTRIBUTE(dmi_sun_port_ex_t, phy, INTEGER, {
            .code  = "phy",
            .name  = "PHY number"
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_sun_port_ex_decode
    }
};

static bool dmi_sun_port_ex_decode(dmi_entity_t *entity)
{
    dmi_sun_port_ex_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SUN_PORT_EX));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    return
        dmi_stream_decode(stream, dmi_word_t, &info->chassis_handle) and
        dmi_stream_decode(stream, dmi_word_t, &info->port_handle) and
        dmi_stream_decode(stream, dmi_byte_t, &info->device_type) and
        dmi_stream_decode(stream, dmi_word_t, &info->device_handle) and
        dmi_stream_decode(stream, dmi_byte_t, &info->phy);
}
