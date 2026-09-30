//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include <opendmi/entity/hpe/reserved-memory-internal.h>

const dmi_entity_spec_t dmi_hpe_reserved_memory_spec =
{
    .type        = DMI_TYPE(hpe_reserved_memory),
    .code        = "hpe-reserved-memory",
    .name        = "HP/HPE reserved memory location",
    .description = (const char *[]){
        "Tells where the memory regions the firmware reserves at boot are, "
        "e.g. the one the storage controller and iLO exchange the "
        "temperatures of the drives in.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x04,
        .decoded_length = sizeof(dmi_hpe_reserved_memory_t)
    },

    // Regions run to the end of the structure, which carries no number of
    // them of its own
    .fields = DMI_FIELDS({
        DMI_FIELD_ARRAY(dmi_hpe_reserved_memory_t, entries, entry_count,
            .stride = 16,
            .fields = DMI_FIELDS({
                DMI_FIELD_BINARY(dmi_hpe_reserved_memory_entry_t, signature_raw, 4),
                DMI_FIELD(dmi_hpe_reserved_memory_entry_t, address, dmi_qword_t),
                DMI_FIELD_BITS(dmi_hpe_reserved_memory_entry_t, raw_size,     31),
                DMI_FIELD_BITS(dmi_hpe_reserved_memory_entry_t, is_kilobytes, 1),
                DMI_FIELD_PAD(dmi_dword_t),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE_ARRAY(dmi_hpe_reserved_memory_t, entries, entry_count, STRUCT, {
            .code  = "entries",
            .name  = "Memory locations",
            .attrs = DMI_ATTRIBUTES({
                DMI_ATTRIBUTE(dmi_hpe_reserved_memory_entry_t, signature, STRING, {
                    .code = "signature",
                    .name = "Signature"
                }),
                DMI_ATTRIBUTE(dmi_hpe_reserved_memory_entry_t, address, ADDRESS, {
                    .code = "address",
                    .name = "Physical address"
                }),
                DMI_ATTRIBUTE(dmi_hpe_reserved_memory_entry_t, size, SIZE, {
                    .code = "size",
                    .name = "Size"
                }),
                {}
            })
        }),
        {}
    }),

    .handlers = {
        .derive  = dmi_hpe_reserved_memory_derive,
        .cleanup = dmi_hpe_reserved_memory_cleanup
    }
};

bool dmi_hpe_reserved_memory_derive(dmi_entity_t *entity)
{
    dmi_hpe_reserved_memory_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_reserved_memory));
    if (info == nullptr)
        return false;

    for (size_t i = 0; i < info->entry_count; i++) {
        dmi_hpe_reserved_memory_entry_t *entry = &info->entries[i];

        entry->size      = (uint64_t)entry->raw_size * (entry->is_kilobytes ? 1024 : 1);
        entry->signature = dmi_text_from_bytes(entry->signature_raw.data, entry->signature_raw.length,
                                        entry->signature_buffer, false);
    }

    return true;
}

void dmi_hpe_reserved_memory_cleanup(dmi_entity_t *entity)
{
    dmi_hpe_reserved_memory_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_reserved_memory));
    if (info == nullptr)
        return;

    dmi_free(info->entries);
}
