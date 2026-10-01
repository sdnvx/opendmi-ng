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

#include <opendmi/entity/dell/memory-ids-internal.h>

const dmi_entity_spec_t dmi_dell_memory_ids_spec =
{
    .type        = DMI_TYPE(dell_memory_ids),
    .code        = "dell-memory-ids",
    .name        = "Dell memory module identifiers",
    .description = (const char *[]){
        "Tells the manufacturer and the serial number of the memory modules, "
        "as their serial presence detect (SPD) data holds them.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x05,
        .decoded_length = sizeof(dmi_dell_memory_ids_t),
        // Modules are decoded only for the length of the identifiers known,
        // since the identifiers of other lengths may be laid out otherwise
        .signature      = DMI_SIGNATURE({
            .offset = 0x04,
            .bytes  = (const uint8_t[]){ 0x0C },
            .size   = 1
        })
    },

    // Modules run to the end of the structure, which carries no number of them
    // of its own
    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_dell_memory_ids_t, id_length, dmi_byte_t),
        DMI_FIELD_ARRAY(dmi_dell_memory_ids_t, modules, module_count,
            .stride = sizeof(dmi_handle_t) + 0x0C,
            .fields = DMI_FIELDS({
                DMI_FIELD(dmi_dell_memory_id_entry_t, handle, dmi_word_t),
                DMI_FIELD_BINARY(dmi_dell_memory_id_entry_t, manufacturer, sizeof(dmi_qword_t)),
                DMI_FIELD_BINARY(dmi_dell_memory_id_entry_t, serial_number, sizeof(dmi_dword_t)),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_dell_memory_ids_t, id_length, INTEGER, {
            .code = "id-length",
            .name = "Identifiers length",
            .unit = DMI_UNIT_BYTE
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_dell_memory_ids_t, modules, module_count, STRUCT, {
            .code  = "modules",
            .name  = "Modules",
            .attrs = DMI_ATTRIBUTES({
                DMI_ATTRIBUTE(dmi_dell_memory_id_entry_t, handle, HANDLE, {
                    .code    = "handle",
                    .name    = "Memory device handle",
                    .targets = dmi_types(DMI_TYPE(memory_device))
                }),
                DMI_ATTRIBUTE(dmi_dell_memory_id_entry_t, manufacturer, BINARY, {
                    .code  = "manufacturer",
                    .name  = "Manufacturer JEDEC ID"
                }),
                DMI_ATTRIBUTE(dmi_dell_memory_id_entry_t, serial_number, BINARY, {
                    .code  = "serial-number",
                    .name  = "Serial number",
                    .flags = DMI_ATTRIBUTE_FLAG_PRIVATE
                }),
                {}
            })
        }),
        {}
    }),

    .handlers = {
        .cleanup = dmi_dell_memory_ids_cleanup
    }
};

void dmi_dell_memory_ids_cleanup(dmi_entity_t *entity)
{
    dmi_dell_memory_ids_t *info = dmi_entity_info(entity, DMI_TYPE(dell_memory_ids));
    if (info == nullptr)
        return;

    dmi_free(info->modules);
}
