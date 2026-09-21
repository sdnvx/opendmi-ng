//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/module/sun.h>

#include <opendmi/entity/sun/memory-device-ex-internal.h>

const dmi_entity_spec_t dmi_sun_memory_device_ex_spec =
{
    .type            = DMI_TYPE(SUN_MEMORY_DEVICE_EX),
    .code            = "sun-memory-device-ex",
    .name            = "Sun memory device extended information",
    .description     = (const char *[]){
        "Extends the memory device information (type 17) with the DRAM "
        "channel and chip selects of the memory device.",
        //
        nullptr
    },
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x08,
        .decoded_length  = sizeof(dmi_sun_memory_device_ex_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_sun_memory_device_ex_t, memory_device_handle, dmi_word_t),
        DMI_FIELD(dmi_sun_memory_device_ex_t, dram_channel,         dmi_byte_t),

        DMI_FIELD_ARRAY(dmi_sun_memory_device_ex_t, chip_selects, chip_select_count,
            .count_length = sizeof(dmi_byte_t),
            .count_member = dmi_member(dmi_sun_memory_device_ex_t, chip_select_total),
            .fields       = DMI_FIELDS({
                DMI_FIELD_ELEMENT(dmi_sun_memory_device_ex_t, chip_selects, dmi_byte_t),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_sun_memory_device_ex_t, memory_device_handle, HANDLE, {
            .code  = "memory-device-handle",
            .name  = "Memory device handle",
            .targets = dmi_types(DMI_TYPE_MEMORY_DEVICE),
        }),
        DMI_ATTRIBUTE(dmi_sun_memory_device_ex_t, dram_channel, INTEGER, {
            .code  = "dram-channel",
            .name  = "DRAM channel"
        }),
        DMI_ATTRIBUTE(dmi_sun_memory_device_ex_t, chip_select_total, INTEGER, {
            .code  = "chip-select-total",
            .name  = "Number of chip selects"
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_sun_memory_device_ex_t, chip_selects, chip_select_count, INTEGER, {
            .code  = "chip-selects",
            .name  = "Chip selects"
        }),
        {}
    }),

    .handlers = {
        .cleanup = dmi_sun_memory_device_ex_cleanup
    }
};
