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

#include "calling-iface-internal.h"
#include "tokens-internal.h"

const dmi_attribute_t dmi_dell_calling_iface_token_attrs[] =
{
    DMI_ATTRIBUTE(dmi_dell_calling_iface_token_t, id, INTEGER, {
        .code  = "id",
        .name  = "Token ID",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_dell_calling_iface_token_t, location, INTEGER, {
        .code  = "location",
        .name  = "Location",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_dell_calling_iface_token_t, value, INTEGER, {
        .code  = "value",
        .name  = "Value",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    {}
};

/**
 * @internal
 * @brief Write a single token of the structure, see
 * `dmi_dell_token_encode_fn`.
 *
 * @param[in,out] encoder Encoder of the structure.
 * @param[in]     item    Token to write.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_dell_calling_iface_encode_token(dmi_encoder_t *encoder, const void *item);

bool dmi_dell_calling_iface_decode(dmi_decoder_t *decoder)
{
    dmi_entity_t *entity = dmi_decoder_entity(decoder);

    dmi_dell_calling_iface_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(dell_calling_iface));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);

    bool status =
        dmi_decoder_get(decoder, dmi_word_t, &info->cmd_io_address) and
        dmi_decoder_get(decoder, dmi_byte_t, &info->cmd_io_code) and
        dmi_decoder_get(decoder, dmi_dword_t, &info->supported_cmds);
    if (not status)
        return false;

    // Tokens are terminated by the end-of-table marker, which may be
    // truncated itself
    size_t capacity = dmi_decoder_remaining(decoder) / DMI_DELL_CALLING_IFACE_TOKEN_SIZE;
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

        dmi_dell_calling_iface_token_t token = { .id = id };

        status =
            dmi_decoder_get(decoder, dmi_word_t, &token.location) and
            dmi_decoder_get(decoder, dmi_word_t, &token.value);
        if (not status)
            return dmi_decoder_incomplete(decoder);

        if (id != DMI_DELL_TOKEN_UNUSED)
            info->tokens[info->token_count++] = token;
    }

    return true;
}

void dmi_dell_calling_iface_cleanup(dmi_entity_t *entity)
{
    dmi_dell_calling_iface_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(dell_calling_iface));
    if (info == nullptr)
        return;

    dmi_free(info->tokens);
}

bool dmi_dell_calling_iface_encode(dmi_encoder_t *encoder)
{
    const dmi_dell_calling_iface_t *info = dmi_entity_info(encoder->entity, DMI_TYPE(dell_calling_iface));
    if (info == nullptr)
        return false;

    bool status =
        dmi_encoder_put(encoder, dmi_word_t, info->cmd_io_address) and
        dmi_encoder_put(encoder, dmi_byte_t, info->cmd_io_code) and
        dmi_encoder_put(encoder, dmi_dword_t, info->supported_cmds);
    if (not status)
        return false;

    return dmi_dell_tokens_encode(encoder, info->tokens, sizeof(*info->tokens), info->token_count,
                                  DMI_DELL_CALLING_IFACE_TOKEN_SIZE, dmi_dell_calling_iface_encode_token);
}

static bool dmi_dell_calling_iface_encode_token(dmi_encoder_t *encoder, const void *item)
{
    const dmi_dell_calling_iface_token_t *token = item;

    return
        dmi_encoder_put(encoder, dmi_word_t, token->id) and
        dmi_encoder_put(encoder, dmi_word_t, token->location) and
        dmi_encoder_put(encoder, dmi_word_t, token->value);
}
