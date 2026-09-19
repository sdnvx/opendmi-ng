//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/module/sun.h>
#include <opendmi/entity/sun/memory-device-ex.h>

static bool dmi_sun_memory_device_ex_decode(dmi_entity_t *entity);
static void dmi_sun_memory_device_ex_cleanup(dmi_entity_t *entity);

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
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x08,
    .decoded_length  = sizeof(dmi_sun_memory_device_ex_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_sun_memory_device_ex_t, memory_device_handle, HANDLE, {
            .code  = "memory-device-handle",
            .name  = "Memory device handle"
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
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode  = dmi_sun_memory_device_ex_decode,
        .cleanup = dmi_sun_memory_device_ex_cleanup
    }
};

static bool dmi_sun_memory_device_ex_decode(dmi_entity_t *entity)
{
    dmi_sun_memory_device_ex_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SUN_MEMORY_DEVICE_EX));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    bool status =
        dmi_stream_decode(stream, dmi_word_t, &info->memory_device_handle) and
        dmi_stream_decode(stream, dmi_byte_t, &info->dram_channel) and
        dmi_stream_decode(stream, dmi_byte_t, &info->chip_select_total);
    if (not status)
        return false;

    if (info->chip_select_total == 0)
        return true;

    info->chip_selects = dmi_alloc_array(entity->context, sizeof(*info->chip_selects),
                                         info->chip_select_total);
    if (info->chip_selects == nullptr)
        return false;

    for (size_t i = 0; i < info->chip_select_total; i++) {
        if (not dmi_stream_decode(stream, dmi_byte_t, &info->chip_selects[i]))
            return dmi_entity_incomplete(entity);

        info->chip_select_count++;
    }

    return true;
}

static void dmi_sun_memory_device_ex_cleanup(dmi_entity_t *entity)
{
    dmi_sun_memory_device_ex_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SUN_MEMORY_DEVICE_EX));
    if (info == nullptr)
        return;

    dmi_free(info->chip_selects);
}
