//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/module/sun.h>
#include <opendmi/entity/sun/processor-ex.h>

static bool dmi_sun_processor_ex_decode(dmi_entity_t *entity);
static void dmi_sun_processor_ex_cleanup(dmi_entity_t *entity);

const dmi_entity_spec_t dmi_sun_processor_ex_spec =
{
    .type            = DMI_TYPE(SUN_PROCESSOR_EX),
    .code            = "sun-processor-ex",
    .name            = "Sun processor extended information",
    .description     = (const char *[]){
        "Extends the processor information (type 4) with initial APIC IDs of "
        "the processor strands.",
        //
        nullptr
    },
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x08,
    .decoded_length  = sizeof(dmi_sun_processor_ex_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_sun_processor_ex_t, processor_handle, HANDLE, {
            .code  = "processor-handle",
            .name  = "Processor handle",
            .targets = dmi_types(DMI_TYPE_PROCESSOR),
        }),
        DMI_ATTRIBUTE(dmi_sun_processor_ex_t, fru, INTEGER, {
            .code  = "fru",
            .name  = "FRU indicator"
        }),
        DMI_ATTRIBUTE(dmi_sun_processor_ex_t, apic_id_total, INTEGER, {
            .code  = "apic-id-total",
            .name  = "Number of APIC IDs"
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_sun_processor_ex_t, apic_ids, apic_id_count, INTEGER, {
            .code  = "apic-ids",
            .name  = "Initial APIC IDs",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode  = dmi_sun_processor_ex_decode,
        .cleanup = dmi_sun_processor_ex_cleanup
    }
};

static bool dmi_sun_processor_ex_decode(dmi_entity_t *entity)
{
    dmi_sun_processor_ex_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SUN_PROCESSOR_EX));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);
    dmi_stream_t  *stream  = dmi_entity_stream(entity);

    bool status =
        dmi_stream_decode(stream, dmi_word_t, &info->processor_handle) and
        dmi_stream_decode(stream, dmi_byte_t, &info->fru) and
        dmi_stream_decode(stream, dmi_byte_t, &info->apic_id_total);
    if (not status)
        return false;

    if (info->apic_id_total == 0)
        return true;

    info->apic_ids = dmi_alloc_array(context, sizeof(*info->apic_ids), info->apic_id_total);
    if (info->apic_ids == nullptr)
        return false;

    // APIC IDs count is incremented only for completely present values
    for (size_t i = 0; i < info->apic_id_total; i++) {
        if (not dmi_stream_decode(stream, dmi_word_t, &info->apic_ids[i]))
            return dmi_entity_incomplete(entity);

        info->apic_id_count++;
    }

    return true;
}

static void dmi_sun_processor_ex_cleanup(dmi_entity_t *entity)
{
    dmi_sun_processor_ex_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SUN_PROCESSOR_EX));
    if (info == nullptr)
        return;

    dmi_free(info->apic_ids);
}
