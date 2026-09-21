//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/sun.h>

#include <opendmi/entity/sun/memory-array-ex-internal.h>

const dmi_entity_spec_t dmi_sun_memory_array_ex_spec =
{
    .type            = DMI_TYPE(SUN_MEMORY_ARRAY_EX),
    .code            = "sun-memory-array-ex",
    .name            = "Sun memory array extended information",
    .description     = (const char *[]){
        "Extends the physical memory array information (type 16) with the "
        "parent component and the PCI address of the memory controller.",
        //
        nullptr
    },
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x0A,
        .decoded_length  = sizeof(dmi_sun_memory_array_ex_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_sun_memory_array_ex_t, memory_array_handle, dmi_word_t),
        DMI_FIELD(dmi_sun_memory_array_ex_t, component_handle,    dmi_word_t),

        // Bus, device and function numbers are packed as in PCI routing IDs
        DMI_FIELD_BITS(dmi_sun_memory_array_ex_t, function_number, 3),
        DMI_FIELD_BITS(dmi_sun_memory_array_ex_t, device_number,   5),
        DMI_FIELD_BITS(dmi_sun_memory_array_ex_t, bus_number,      8),
        DMI_FIELD_PAD(dmi_word_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_sun_memory_array_ex_t, memory_array_handle, HANDLE, {
            .code  = "memory-array-handle",
            .name  = "Memory array handle",
            .targets = dmi_types(DMI_TYPE_MEMORY_ARRAY),
        }),
        DMI_ATTRIBUTE(dmi_sun_memory_array_ex_t, component_handle, HANDLE, {
            .code  = "component-handle",
            .name  = "Parent component handle"
        }),
        DMI_ATTRIBUTE(dmi_sun_memory_array_ex_t, bus_number, INTEGER, {
            .code  = "bus-number",
            .name  = "Bus number",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_sun_memory_array_ex_t, device_number, INTEGER, {
            .code  = "device-number",
            .name  = "Device number",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_sun_memory_array_ex_t, function_number, INTEGER, {
            .code  = "function-number",
            .name  = "Function number"
        }),
        {}
    })
};
