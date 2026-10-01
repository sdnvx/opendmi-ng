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

#include <opendmi/entity/dell/device-names.h>

const dmi_entity_spec_t dmi_dell_device_names_spec =
{
    .type        = DMI_TYPE(dell_device_names),
    .code        = "dell-device-names",
    .name        = "Dell device names",
    .description = (const char *[]){
        "Gives the devices of a kind, e.g. the processors or the memory "
        "devices, the names the management controller (iDRAC) knows them by.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x05,
        .decoded_length = sizeof(dmi_dell_device_names_t)
    },

    // Devices run to the end of the structure, which carries no number of them
    // of its own
    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_dell_device_names_t, unknown, dmi_byte_t),
        DMI_FIELD_ARRAY(dmi_dell_device_names_t, devices, device_count,
            .stride = sizeof(dmi_byte_t) + sizeof(dmi_handle_t) + sizeof(dmi_byte_t),
            .fields = DMI_FIELDS({
                DMI_FIELD_STRING(dmi_dell_device_name_entry_t, fqdd),
                DMI_FIELD(dmi_dell_device_name_entry_t, handle,  dmi_word_t),
                DMI_FIELD(dmi_dell_device_name_entry_t, unknown, dmi_byte_t),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_dell_device_names_t, unknown, INTEGER, {
            .code  = "unknown",
            .name  = "Unknown",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_dell_device_names_t, devices, device_count, STRUCT, {
            .code  = "devices",
            .name  = "Devices",
            .attrs = DMI_ATTRIBUTES({
                DMI_ATTRIBUTE(dmi_dell_device_name_entry_t, fqdd, STRING, {
                    .code = "fqdd",
                    .name = "Fully qualified device descriptor"
                }),
                DMI_ATTRIBUTE(dmi_dell_device_name_entry_t, handle, HANDLE, {
                    .code    = "handle",
                    .name    = "Handle",
                    .targets = dmi_types(DMI_TYPE(processor), DMI_TYPE(memory_device))
                }),
                DMI_ATTRIBUTE(dmi_dell_device_name_entry_t, unknown, INTEGER, {
                    .code  = "unknown",
                    .name  = "Unknown",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        {}
    })
};
