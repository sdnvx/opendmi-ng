//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/sun.h>
#include <opendmi/entity/sun/memory-array-ex.h>

static bool dmi_sun_memory_array_ex_decode(dmi_entity_t *entity);

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
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x0A,
    .decoded_length  = sizeof(dmi_sun_memory_array_ex_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_sun_memory_array_ex_t, memory_array_handle, HANDLE, {
            .code  = "memory-array-handle",
            .name  = "Memory array handle"
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
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode  = dmi_sun_memory_array_ex_decode
    }
};

static bool dmi_sun_memory_array_ex_decode(dmi_entity_t *entity)
{
    dmi_sun_memory_array_ex_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SUN_MEMORY_ARRAY_EX));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;
    dmi_word_t bdf = 0;

    bool status =
        dmi_stream_decode(stream, dmi_word_t, &info->memory_array_handle) and
        dmi_stream_decode(stream, dmi_word_t, &info->component_handle) and
        dmi_stream_decode(stream, dmi_word_t, &bdf);
    if (not status)
        return false;

    // Bus, device and function numbers are packed as in PCI routing IDs
    info->bus_number      = (uint8_t)(bdf >> 8);
    info->device_number   = (uint8_t)((bdf >> 3) & 0x1F);
    info->function_number = (uint8_t)(bdf & 0x07);

    return true;
}
