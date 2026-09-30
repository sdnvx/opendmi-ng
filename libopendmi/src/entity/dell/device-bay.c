//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/device-bay.h>

const dmi_entity_spec_t dmi_dell_device_bay_spec =
{
    .type        = DMI_TYPE(dell_device_bay),
    .code        = "dell-device-bay",
    .name        = "Dell device bay",
    .description = (const char *[]){
        "Describes a bay of a laptop or of its docking station, which takes "
        "one of several devices.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x09,
        .decoded_length = sizeof(dmi_dell_device_bay_t),
        // Structures of the same type other platforms carry are laid out
        // otherwise
        .signature      = DMI_SIGNATURE({
            .length = 0x09
        })
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_dell_device_bay_t, unknown_1, dmi_byte_t),
        DMI_FIELD_STRING(dmi_dell_device_bay_t, name),
        DMI_FIELD_STRING(dmi_dell_device_bay_t, supported_devices),
        DMI_FIELD_STRING(dmi_dell_device_bay_t, installed_device),
        DMI_FIELD(dmi_dell_device_bay_t, unknown_2, dmi_byte_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_dell_device_bay_t, unknown_1, INTEGER, {
            .code  = "unknown-1",
            .name  = "Unknown 1",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_device_bay_t, name, STRING, {
            .code = "name",
            .name = "Name"
        }),
        DMI_ATTRIBUTE(dmi_dell_device_bay_t, supported_devices, STRING, {
            .code = "supported-devices",
            .name = "Supported devices"
        }),
        DMI_ATTRIBUTE(dmi_dell_device_bay_t, installed_device, STRING, {
            .code = "installed-device",
            .name = "Installed device"
        }),
        DMI_ATTRIBUTE(dmi_dell_device_bay_t, unknown_2, INTEGER, {
            .code  = "unknown-2",
            .name  = "Unknown 2",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        {}
    })
};
