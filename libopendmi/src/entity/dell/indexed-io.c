//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/module/dell.h>
#include <opendmi/entity/dell/indexed-io.h>

/**
 * @brief Token identifier of unused tokens.
 */
#define DMI_DELL_TOKEN_UNUSED 0x0000u

/**
 * @brief Token identifier of the end-of-table marker.
 */
#define DMI_DELL_TOKEN_EOT 0xFFFFu

/**
 * @brief Size of the token in the structure.
 */
#define DMI_DELL_INDEXED_IO_TOKEN_SIZE 5

static bool dmi_dell_indexed_io_decode(dmi_entity_t *entity);
static void dmi_dell_indexed_io_cleanup(dmi_entity_t *entity);

static const dmi_attribute_t dmi_dell_indexed_io_token_attrs[] =
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
        .variants = (const dmi_attribute_variant_t[]){
            DMI_VARIANT(false, dmi_dell_indexed_io_token_t, or_value, INTEGER, {
                .flags = DMI_ATTRIBUTE_FLAG_HEX
            }),
            DMI_VARIANT_NULL
        }
    }),
    DMI_ATTRIBUTE_VARIANT(dmi_dell_indexed_io_token_t, is_string, {
        .code     = "string-length",
        .name     = "String length",
        .variants = (const dmi_attribute_variant_t[]){
            DMI_VARIANT(true, dmi_dell_indexed_io_token_t, string_length, INTEGER, {}),
            DMI_VARIANT_NULL
        }
    }),
    DMI_ATTRIBUTE_NULL
};

const dmi_entity_spec_t dmi_dell_indexed_io_spec =
{
    .type            = DMI_TYPE(DELL_INDEXED_IO),
    .code            = "dell-indexed-io",
    .name            = "Dell indexed IO",
    .description     = (const char *[]){
        "Describes tokens, which are stored in CMOS and accessed through the "
        "indexed I/O ports, and the checksum of the CMOS area.",
        //
        nullptr
    },
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x0C,
    .decoded_length  = sizeof(dmi_dell_indexed_io_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_dell_indexed_io_t, index_port, INTEGER, {
            .code   = "index-port",
            .name   = "Index port",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_indexed_io_t, data_port, INTEGER, {
            .code   = "data-port",
            .name   = "Data port",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_indexed_io_t, check_type, ENUM, {
            .code   = "check-type",
            .name   = "Checksum type",
            .values = &dmi_dell_check_type_names
        }),
        DMI_ATTRIBUTE(dmi_dell_indexed_io_t, check_start, INTEGER, {
            .code   = "check-start",
            .name   = "Checked range start",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_indexed_io_t, check_end, INTEGER, {
            .code   = "check-end",
            .name   = "Checked range end",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_indexed_io_t, check_index, INTEGER, {
            .code   = "check-index",
            .name   = "Checksum index",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_dell_indexed_io_t, tokens, token_count, STRUCT, {
            .code   = "tokens",
            .name   = "Tokens",
            .attrs  = dmi_dell_indexed_io_token_attrs
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode  = dmi_dell_indexed_io_decode,
        .cleanup = dmi_dell_indexed_io_cleanup
    }
};

static bool dmi_dell_indexed_io_decode(dmi_entity_t *entity)
{
    dmi_dell_indexed_io_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(DELL_INDEXED_IO));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;
    dmi_byte_t check_type = 0;

    bool status =
        dmi_stream_decode(stream, dmi_word_t, &info->index_port) and
        dmi_stream_decode(stream, dmi_word_t, &info->data_port) and
        dmi_stream_decode(stream, dmi_byte_t, &check_type) and
        dmi_stream_decode(stream, dmi_byte_t, &info->check_start) and
        dmi_stream_decode(stream, dmi_byte_t, &info->check_end) and
        dmi_stream_decode(stream, dmi_byte_t, &info->check_index);
    if (not status)
        return false;

    info->check_type = dmi_cast(info->check_type, check_type);

    // Tokens are terminated by the end-of-table marker, which may be
    // truncated itself
    size_t capacity = dmi_stream_remaining(stream) / DMI_DELL_INDEXED_IO_TOKEN_SIZE;
    if (capacity > 0) {
        info->tokens = dmi_alloc_array(entity->context, sizeof(*info->tokens), capacity);
        if (info->tokens == nullptr)
            return false;
    }

    while (true) {
        dmi_word_t id = 0;

        if (not dmi_stream_decode(stream, dmi_word_t, &id))
            return dmi_entity_incomplete(entity);

        if (id == DMI_DELL_TOKEN_EOT) {
            dmi_stream_skip(stream, dmi_stream_remaining(stream));
            break;
        }

        dmi_dell_indexed_io_token_t token = { .id = id };
        dmi_byte_t value = 0;

        status =
            dmi_stream_decode(stream, dmi_byte_t, &token.location) and
            dmi_stream_decode(stream, dmi_byte_t, &token.and_mask) and
            dmi_stream_decode(stream, dmi_byte_t, &value);
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

static void dmi_dell_indexed_io_cleanup(dmi_entity_t *entity)
{
    dmi_dell_indexed_io_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(DELL_INDEXED_IO));
    if (info == nullptr)
        return;

    dmi_free(info->tokens);
}
