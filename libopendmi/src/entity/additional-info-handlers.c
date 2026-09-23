//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>
#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/utils.h>
#include <opendmi/writer.h>
#include <opendmi/internal.h>

#include <opendmi/entity/additional-info-internal.h>

bool dmi_additional_info_decode(dmi_entity_t *entity)
{
    dmi_additional_info_t *info;

    assert(entity != nullptr);

    info = dmi_entity_info(entity, DMI_TYPE(ADDITIONAL_INFO));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);
    dmi_reader_t  *reader  = dmi_entity_reader(entity);

    if (not dmi_reader_get(reader, dmi_byte_t, &info->entry_count)) {
        dmi_log_error(context, "Unable to decode additional information entries count: 0x%04X",
                      dmi_entity_handle(entity));
        return false;
    }

    info->entries = dmi_alloc_array(context, sizeof(dmi_additional_info_entry_t), info->entry_count);
    if (info->entries == nullptr)
        return false;

    for (size_t i = 0; i < info->entry_count; i++) {
        dmi_additional_info_entry_t *entry = &info->entries[i];

        size_t entry_length;

        bool status =
            dmi_reader_get(reader, dmi_byte_t, &entry_length) and
            dmi_reader_get(reader, dmi_word_t, &entry->ref_handle) and
            dmi_reader_get(reader, dmi_byte_t, &entry->ref_offset) and
            dmi_reader_get_string(reader, &entry->string);
        if (not status) {
            dmi_log_error(context,
                          "Additional information entry body truncated: 0x%04X[%zu]",
                          dmi_entity_handle(entity), i);
            return false;
        }

        // Entry length includes the entry header, and there is at least one
        // byte of value
        if (entry_length < DMI_ADDITIONAL_INFO_ENTRY_HEADER + 1) {
            dmi_log_error(context,
                          "Invalid additional information entry length: 0x%04X[%zu]: %zu bytes",
                          dmi_entity_handle(entity), i, entry_length);
            return false;
        }

        entry->value.length = entry_length - DMI_ADDITIONAL_INFO_ENTRY_HEADER;

        if (entry->ref_offset < sizeof(dmi_header_t)) {
            dmi_log_warning(context,
                            "Invalid additional info entry offset: 0x%04X[%zu]: offset=%u",
                            dmi_entity_handle(entity), i, entry->ref_offset);
        }

        size_t remaining = dmi_reader_remaining(reader);
        if (entry->value.length > remaining) {
            dmi_log_warning(context,
                            "Truncated additional information entry value: "
                            "0x%04X[%zu]: length=%zu remaining=%zu",
                            dmi_entity_handle(entity), i, entry->value.length, remaining);
            entry->value.length = remaining;
        }

        // Value is referenced in place, since its length is not limited
        if (not dmi_reader_get_binary(reader, entry->value.length, &entry->value)) {
            dmi_log_error(context, "Unable to decode additional information entry value: 0x%04X[%zu]",
                          dmi_entity_handle(entity), i);
            return false;
        }
    }

    return true;
}

void dmi_additional_info_cleanup(dmi_entity_t *entity)
{
    dmi_additional_info_t *info;

    assert(entity != nullptr);

    info = dmi_entity_info(entity, DMI_TYPE(ADDITIONAL_INFO));
    if (info == nullptr)
        return;

    dmi_free(info->entries);
}

//
// Entries are written with the length of their value. The length the source
// data declares is kept whenever it reads as the same value, which it does
// when the structure ends before the value and the decoder has taken what is
// there.
//
bool dmi_additional_info_encode(dmi_writer_t *writer)
{
    const dmi_additional_info_t *info = dmi_entity_info(writer->entity, DMI_TYPE(ADDITIONAL_INFO));
    if (info == nullptr)
        return false;

    if (not dmi_writer_put(writer, dmi_byte_t, info->entry_count))
        return false;

    for (size_t i = 0; i < info->entry_count; i++) {
        const dmi_additional_info_entry_t *entry = &info->entries[i];

        size_t length = DMI_ADDITIONAL_INFO_ENTRY_HEADER + entry->value.length;

        dmi_byte_t original = 0;

        if (dmi_writer_peek(writer, &original, sizeof(original)) and
            (original > DMI_ADDITIONAL_INFO_ENTRY_HEADER) and
            (dmi_writer_remaining(writer) >= DMI_ADDITIONAL_INFO_ENTRY_HEADER)) {
            size_t declared  = original - DMI_ADDITIONAL_INFO_ENTRY_HEADER;
            size_t remaining = dmi_writer_remaining(writer) - DMI_ADDITIONAL_INFO_ENTRY_HEADER;

            if (((declared < remaining) ? declared : remaining) == entry->value.length)
                length = original;
        }

        if (length > UINT8_MAX) {
            dmi_error_raise_ex(dmi_entity_context(writer->entity), DMI_ERROR_INVALID_ARGUMENT,
                               "0x%04x: entry %zu of %zu bytes", dmi_entity_handle(writer->entity),
                               i, length);
            return false;
        }

        bool status =
            dmi_writer_put(writer, dmi_byte_t, length) and
            dmi_writer_put(writer, dmi_word_t, entry->ref_handle) and
            dmi_writer_put(writer, dmi_byte_t, entry->ref_offset) and
            dmi_writer_put_string(writer, entry->string) and
            dmi_writer_put_bytes(writer, entry->value.data, entry->value.length);
        if (not status)
            return false;
    }

    return true;
}
