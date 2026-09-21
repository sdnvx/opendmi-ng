//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/sun.h>

#include <opendmi/entity/sun/port-ex-internal.h>

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
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x0C,
        .decoded_length  = sizeof(dmi_sun_port_ex_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_sun_port_ex_t, chassis_handle, WORD),
        DMI_FIELD(dmi_sun_port_ex_t, port_handle,    WORD),
        DMI_FIELD(dmi_sun_port_ex_t, device_type,    BYTE),
        DMI_FIELD(dmi_sun_port_ex_t, device_handle,  WORD),
        DMI_FIELD(dmi_sun_port_ex_t, phy,            BYTE),
        {}
    }),

    .attributes      = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_sun_port_ex_t, chassis_handle, HANDLE, {
            .code  = "chassis-handle",
            .name  = "Chassis handle",
            .targets = dmi_types(DMI_TYPE_CHASSIS),
        }),
        DMI_ATTRIBUTE(dmi_sun_port_ex_t, port_handle, HANDLE, {
            .code  = "port-handle",
            .name  = "Port connector handle",
            .targets = dmi_types(DMI_TYPE_PORT_CONNECTOR),
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
        {}
    })
};
