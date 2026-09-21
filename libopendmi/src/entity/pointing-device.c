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

#include <opendmi/entity/pointing-device-internal.h>

const dmi_entity_spec_t dmi_pointing_device_spec =
{
    .code            = "pointing-device",
    .name            = "Built-in pointing device",
    .description     = (const char *[]){
        "This structure describes the attributes of the built-in pointing "
        "device for the system. The presence of this structure does not imply "
        "that the built-in pointing device is active for the system\'s use.",
        //
        nullptr
    },
    .type            = DMI_TYPE(POINTING_DEVICE),
    .params = {
        .minimum_version = DMI_VERSION(2, 1, 0),
        .minimum_length  = 0x07,
        .decoded_length  = sizeof(dmi_pointing_device_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_pointing_device_t, type,         BYTE),
        DMI_FIELD(dmi_pointing_device_t, interface,    BYTE),
        DMI_FIELD(dmi_pointing_device_t, button_count, BYTE),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_pointing_device_t, type, ENUM, {
            .code    = "type",
            .name    = "Type",
            .unspec  = dmi_value_ptr(DMI_POINTING_DEVICE_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_POINTING_DEVICE_TYPE_UNKNOWN),
            .values  = &dmi_pointing_device_type_names
        }),
        DMI_ATTRIBUTE(dmi_pointing_device_t, interface, ENUM, {
            .code    = "interface",
            .name    = "Interface type",
            .unspec  = dmi_value_ptr(DMI_POINTING_DEVICE_IFACE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_POINTING_DEVICE_IFACE_UNKNOWN),
            .values  = &dmi_pointing_device_iface_names
        }),
        DMI_ATTRIBUTE(dmi_pointing_device_t, button_count, INTEGER, {
            .code    = "button-count",
            .name    = "Button count"
        }),
        {}
    }),
};
