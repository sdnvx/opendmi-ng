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

static_assert(sizeof(dmi_dell_calling_iface_token_t) <= DMI_DELL_TOKEN_MAX_SIZE);

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
 * @brief Read the rest of a token, which follows its identifier.
 *
 * @param[in,out] decoder Decoder of the structure.
 * @param[in]     id      Identifier of the token.
 * @param[out]    item    Variable to store the token in.
 *
 * @return `true` on success, `false` if the structure ends in the middle of
 *         the token.
 */
static bool dmi_dell_calling_iface_decode_token(dmi_decoder_t *decoder, dmi_word_t id, void *item);

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

    bool status =
        dmi_decoder_get(decoder, dmi_word_t, &info->command_io_address) and
        dmi_decoder_get(decoder, dmi_byte_t, &info->command_io_code) and
        dmi_decoder_get(decoder, dmi_dword_t, &info->supported_commands);
    if (not status)
        return false;

    void *tokens = nullptr;

    status = dmi_dell_tokens_decode(decoder, sizeof(*info->tokens), DMI_DELL_CALLING_IFACE_TOKEN_SIZE,
                                    dmi_dell_calling_iface_decode_token, &tokens, &info->token_count);
    info->tokens = tokens;

    return status;
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
        dmi_encoder_put(encoder, dmi_word_t, info->command_io_address) and
        dmi_encoder_put(encoder, dmi_byte_t, info->command_io_code) and
        dmi_encoder_put(encoder, dmi_dword_t, info->supported_commands);
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

static bool dmi_dell_calling_iface_decode_token(dmi_decoder_t *decoder, dmi_word_t id, void *item)
{
    dmi_dell_calling_iface_token_t *token = item;

    token->id = id;

    return
        dmi_decoder_get(decoder, dmi_word_t, &token->location) and
        dmi_decoder_get(decoder, dmi_word_t, &token->value);
}
