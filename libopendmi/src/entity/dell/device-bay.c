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
        // Intel Management Engine interface information, which Dell gives
        // the same type on some systems, is told by its signature, and the
        // structure is decoded for the other ones. Numbers of the strings do
        // not tell it, since the structures whose strings are the same are
        // written back with a single one
        .minimum_length = 0x09,
        .decoded_length = sizeof(dmi_dell_device_bay_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_dell_device_bay_t, unknown_1, dmi_byte_t),
        DMI_FIELD_STRING(dmi_dell_device_bay_t, name),
        DMI_FIELD_STRING(dmi_dell_device_bay_t, supported_devices),
        DMI_FIELD_STRING(dmi_dell_device_bay_t, installed_device),
        DMI_FIELD(dmi_dell_device_bay_t, unknown_2, dmi_byte_t),

        DMI_FIELD_GROUP(.present = dmi_member(dmi_dell_device_bay_t, has_unknown_strings)),
        DMI_FIELD_STRING(dmi_dell_device_bay_t, unknown_string_1),
        DMI_FIELD_STRING(dmi_dell_device_bay_t, unknown_string_2),
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
        // Structures of 11 bytes refer to two more strings
        DMI_ATTRIBUTE_VARIANT(dmi_dell_device_bay_t, has_unknown_strings, {
            .code     = "unknown-string-1",
            .name     = "Unknown string 1",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_dell_device_bay_t, unknown_string_1, STRING, {}),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_dell_device_bay_t, has_unknown_strings, {
            .code     = "unknown-string-2",
            .name     = "Unknown string 2",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_dell_device_bay_t, unknown_string_2, STRING, {}),
                {}
            })
        }),
        {}
    })
};
