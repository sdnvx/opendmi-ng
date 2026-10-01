//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/encoder.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/module/dell.h>

#include "indexed-io-internal.h"

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

/**
 * @internal
 * @brief Write a single token of the structure.
 *
 * @param[in,out] encoder Encoder of the structure.
 * @param[in]     item    Token to write.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_dell_indexed_io_encode_token(dmi_encoder_t *encoder, const void *item);

bool dmi_dell_indexed_io_decode(dmi_decoder_t *decoder)
{
    dmi_entity_t *entity = dmi_decoder_entity(decoder);

    dmi_dell_indexed_io_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(dell_indexed_io));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);

    dmi_byte_t check_type = 0;

    bool status =
        dmi_decoder_get(decoder, dmi_word_t, &info->index_port) and
        dmi_decoder_get(decoder, dmi_word_t, &info->data_port) and
        dmi_decoder_get(decoder, dmi_byte_t, &check_type) and
        dmi_decoder_get(decoder, dmi_byte_t, &info->check_start) and
        dmi_decoder_get(decoder, dmi_byte_t, &info->check_end) and
        dmi_decoder_get(decoder, dmi_byte_t, &info->check_index);
    if (not status)
        return false;

    info->check_type = dmi_cast(info->check_type, check_type);

    // Tokens are terminated by the end-of-table marker, which may be
    // truncated itself
    size_t capacity = dmi_decoder_remaining(decoder) / DMI_DELL_INDEXED_IO_TOKEN_SIZE;
    if (capacity > 0) {
        info->tokens = dmi_alloc_array(context, sizeof(*info->tokens), capacity);
        if (info->tokens == nullptr)
            return false;
    }

    while (true) {
        dmi_word_t id = 0;

        if (not dmi_decoder_get(decoder, dmi_word_t, &id))
            return dmi_decoder_incomplete(decoder);

        if (id == DMI_DELL_TOKEN_EOT) {
            dmi_decoder_skip(decoder, dmi_decoder_remaining(decoder));
            break;
        }

        dmi_dell_indexed_io_token_t token = { .id = id };
        dmi_byte_t value = 0;

        status =
            dmi_decoder_get(decoder, dmi_byte_t, &token.location) and
            dmi_decoder_get(decoder, dmi_byte_t, &token.and_mask) and
            dmi_decoder_get(decoder, dmi_byte_t, &value);
        if (not status)
            return dmi_decoder_incomplete(decoder);

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

    info = dmi_entity_info(entity, DMI_TYPE(dell_indexed_io));
    if (info == nullptr)
        return;

    dmi_free(info->tokens);
}

bool dmi_dell_indexed_io_encode(dmi_encoder_t *encoder)
{
    const dmi_dell_indexed_io_t *info = dmi_entity_info(encoder->entity, DMI_TYPE(dell_indexed_io));
    if (info == nullptr)
        return false;

    bool status =
        dmi_encoder_put(encoder, dmi_word_t, info->index_port) and
        dmi_encoder_put(encoder, dmi_word_t, info->data_port) and
        dmi_encoder_put(encoder, dmi_byte_t, info->check_type) and
        dmi_encoder_put(encoder, dmi_byte_t, info->check_start) and
        dmi_encoder_put(encoder, dmi_byte_t, info->check_end) and
        dmi_encoder_put(encoder, dmi_byte_t, info->check_index);
    if (not status)
        return false;

    return dmi_dell_tokens_encode(encoder, info->tokens, sizeof(*info->tokens), info->token_count,
                                  DMI_DELL_INDEXED_IO_TOKEN_SIZE, dmi_dell_indexed_io_encode_token);
}

static bool dmi_dell_indexed_io_encode_token(dmi_encoder_t *encoder, const void *item)
{
    const dmi_dell_indexed_io_token_t *token = item;

    // String tokens have no mask, and the value is the string length
    dmi_byte_t value = token->is_string ? token->string_length : token->or_value;

    return
        dmi_encoder_put(encoder, dmi_word_t, token->id) and
        dmi_encoder_put(encoder, dmi_byte_t, token->location) and
        dmi_encoder_put(encoder, dmi_byte_t, token->and_mask) and
        dmi_encoder_put(encoder, dmi_byte_t, value);
}
