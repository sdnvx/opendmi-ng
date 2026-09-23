//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/writer.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/indexed-io-internal.h>

const dmi_attribute_t dmi_dell_indexed_io_token_attrs[] =
{
    DMI_ATTRIBUTE(dmi_dell_indexed_io_token_t, id, INTEGER, {
        .code  = "id",
        .name  = "Token ID",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_dell_indexed_io_token_t, location, INTEGER, {
        .code  = "location",
        .name  = "Location",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_dell_indexed_io_token_t, and_mask, INTEGER, {
        .code  = "and-mask",
        .name  = "AND mask",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE_VARIANT(dmi_dell_indexed_io_token_t, is_string, {
        .code     = "or-value",
        .name     = "OR value",
        .variants = DMI_VARIANTS({
            DMI_VARIANT(false, dmi_dell_indexed_io_token_t, or_value, INTEGER, {
                .flags = DMI_ATTRIBUTE_FLAG_HEX
            }),
            {}
        })
    }),
    DMI_ATTRIBUTE_VARIANT(dmi_dell_indexed_io_token_t, is_string, {
        .code     = "string-length",
        .name     = "String length",
        .variants = DMI_VARIANTS({
            DMI_VARIANT(true, dmi_dell_indexed_io_token_t, string_length, INTEGER, {}),
            {}
        })
    }),
    {}
};

bool dmi_dell_indexed_io_decode(dmi_entity_t *entity)
{
    dmi_dell_indexed_io_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(DELL_INDEXED_IO));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);
    dmi_reader_t  *reader  = dmi_entity_reader(entity);

    dmi_byte_t check_type = 0;

    bool status =
        dmi_reader_get(reader, dmi_word_t, &info->index_port) and
        dmi_reader_get(reader, dmi_word_t, &info->data_port) and
        dmi_reader_get(reader, dmi_byte_t, &check_type) and
        dmi_reader_get(reader, dmi_byte_t, &info->check_start) and
        dmi_reader_get(reader, dmi_byte_t, &info->check_end) and
        dmi_reader_get(reader, dmi_byte_t, &info->check_index);
    if (not status)
        return false;

    info->check_type = dmi_cast(info->check_type, check_type);

    // Tokens are terminated by the end-of-table marker, which may be
    // truncated itself
    size_t capacity = dmi_reader_remaining(reader) / DMI_DELL_INDEXED_IO_TOKEN_SIZE;
    if (capacity > 0) {
        info->tokens = dmi_alloc_array(context, sizeof(*info->tokens), capacity);
        if (info->tokens == nullptr)
            return false;
    }

    while (true) {
        dmi_word_t id = 0;

        if (not dmi_reader_get(reader, dmi_word_t, &id))
            return dmi_entity_incomplete(entity);

        if (id == DMI_DELL_TOKEN_EOT) {
            dmi_reader_skip(reader, dmi_reader_remaining(reader));
            break;
        }

        dmi_dell_indexed_io_token_t token = { .id = id };
        dmi_byte_t value = 0;

        status =
            dmi_reader_get(reader, dmi_byte_t, &token.location) and
            dmi_reader_get(reader, dmi_byte_t, &token.and_mask) and
            dmi_reader_get(reader, dmi_byte_t, &value);
        if (not status)
            return dmi_entity_incomplete(entity);

        if (id == DMI_DELL_TOKEN_UNUSED)
            continue;

        // String tokens have no mask, and the value is the string length
        token.is_string = (token.and_mask == 0);
        if (token.is_string)
            token.string_length = value;
        else
            token.or_value = value;

        info->tokens[info->token_count++] = token;
    }

    return true;
}

void dmi_dell_indexed_io_cleanup(dmi_entity_t *entity)
{
    dmi_dell_indexed_io_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(DELL_INDEXED_IO));
    if (info == nullptr)
        return;

    dmi_free(info->tokens);
}

static bool dmi_dell_indexed_io_encode_token(
        dmi_writer_t                      *writer,
        const dmi_dell_indexed_io_token_t *token)
{
    // String tokens have no mask, and the value is the string length
    dmi_byte_t value = token->is_string ? token->string_length : token->or_value;

    return
        dmi_writer_put(writer, dmi_word_t, token->id) and
        dmi_writer_put(writer, dmi_byte_t, token->location) and
        dmi_writer_put(writer, dmi_byte_t, token->and_mask) and
        dmi_writer_put(writer, dmi_byte_t, value);
}

//
// Tokens are written in turn, and are terminated by the end-of-table marker.
//
bool dmi_dell_indexed_io_encode(dmi_writer_t *writer)
{
    const dmi_dell_indexed_io_t *info = dmi_entity_info(writer->entity, DMI_TYPE(DELL_INDEXED_IO));
    if (info == nullptr)
        return false;

    bool status =
        dmi_writer_put(writer, dmi_word_t, info->index_port) and
        dmi_writer_put(writer, dmi_word_t, info->data_port) and
        dmi_writer_put(writer, dmi_byte_t, info->check_type) and
        dmi_writer_put(writer, dmi_byte_t, info->check_start) and
        dmi_writer_put(writer, dmi_byte_t, info->check_end) and
        dmi_writer_put(writer, dmi_byte_t, info->check_index);
    if (not status)
        return false;

    // Tokens the decoder drops as unused are not in the model, so the records
    // of the source data are walked in the preserve mode: the unused ones are
    // kept as they are, and the rest are written from the model in turn
    size_t next = 0;

    while ((writer->mode == DMI_ENCODE_MODE_PRESERVE) and (next < info->token_count)) {
        dmi_byte_t id[2];

        if (not dmi_writer_peek(writer, id, sizeof(id)))
            break;

        dmi_word_t original = (dmi_word_t)(id[0] | (id[1] << 8));

        if ((original == DMI_DELL_TOKEN_EOT) or (dmi_writer_remaining(writer) < DMI_DELL_INDEXED_IO_TOKEN_SIZE))
            break;

        if (original == DMI_DELL_TOKEN_UNUSED) {
            if (not dmi_writer_copy(writer, DMI_DELL_INDEXED_IO_TOKEN_SIZE))
                return false;
            continue;
        }

        if (not dmi_dell_indexed_io_encode_token(writer, &info->tokens[next++]))
            return false;
    }

    // Tokens the source data has no records for, and all of them in the
    // canonical mode
    while (next < info->token_count) {
        if (not dmi_dell_indexed_io_encode_token(writer, &info->tokens[next++]))
            return false;
    }

    // Marker ending the tokens is kept along with whatever follows it in the
    // preserve mode, and written in the canonical one
    if (writer->mode == DMI_ENCODE_MODE_CANONICAL)
        return dmi_writer_put(writer, dmi_word_t, DMI_DELL_TOKEN_EOT);

    return true;
}
