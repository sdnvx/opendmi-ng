//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/apple.h>

#include <opendmi/entity/apple/memory-spd-data.h>

const dmi_entity_spec_t dmi_apple_memory_spd_data_spec =
{
    .type        = DMI_TYPE(apple_memory_spd_data),
    .code        = "apple-memory-spd-data",
    .name        = "Apple memory SPD data",
    .description = (const char *[]){
        "Holds the serial presence detect (SPD) data of a memory module, or "
        "a part of it.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x0A,
        .decoded_length = sizeof(dmi_apple_memory_spd_data_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_apple_memory_spd_data_t, handle, dmi_word_t),
        DMI_FIELD(dmi_apple_memory_spd_data_t, offset, dmi_word_t),
        DMI_FIELD(dmi_apple_memory_spd_data_t, size,   dmi_word_t),
        DMI_FIELD_BINARY(dmi_apple_memory_spd_data_t, data, DMI_FIELD_LENGTH_REST),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_apple_memory_spd_data_t, handle, HANDLE, {
            .code    = "handle",
            .name    = "Memory device handle",
            .targets = dmi_types(DMI_TYPE(memory_device))
        }),
        DMI_ATTRIBUTE(dmi_apple_memory_spd_data_t, offset, INTEGER, {
            .code  = "offset",
            .name  = "Offset",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_apple_memory_spd_data_t, size, INTEGER, {
            .code = "size",
            .name = "Size",
            .unit = DMI_UNIT_BYTE
        }),
        DMI_ATTRIBUTE(dmi_apple_memory_spd_data_t, data, BINARY, {
            .code = "data",
            .name = "Data"
        }),
        {}
    })
};
