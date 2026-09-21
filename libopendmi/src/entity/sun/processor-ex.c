//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/module/sun.h>

#include <opendmi/entity/sun/processor-ex-internal.h>

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
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x08,
        .decoded_length  = sizeof(dmi_sun_processor_ex_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_sun_processor_ex_t, processor_handle, dmi_word_t),
        DMI_FIELD(dmi_sun_processor_ex_t, fru,              dmi_byte_t),

        DMI_FIELD_ARRAY(dmi_sun_processor_ex_t, apic_ids, apic_id_count,
            .count_length = sizeof(dmi_byte_t),
            .count_member = dmi_member(dmi_sun_processor_ex_t, apic_id_total),
            .fields       = DMI_FIELDS({
                DMI_FIELD_ELEMENT(dmi_sun_processor_ex_t, apic_ids, dmi_word_t),
                {}
            })),
        {}
    }),

    .attributes      = DMI_ATTRIBUTES({
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
        {}
    }),

    .handlers = {
        .cleanup = dmi_sun_processor_ex_cleanup
    }
};
