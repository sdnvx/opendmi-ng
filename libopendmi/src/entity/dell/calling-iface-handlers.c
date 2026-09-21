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

#include <opendmi/entity/dell/calling-iface-internal.h>

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

bool dmi_dell_calling_iface_decode(dmi_entity_t *entity)
{
    dmi_dell_calling_iface_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(DELL_CALLING_IFACE));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);
    dmi_stream_t  *stream  = dmi_entity_stream(entity);

    bool status =
        dmi_stream_decode(stream, dmi_word_t, &info->cmd_io_address) and
        dmi_stream_decode(stream, dmi_byte_t, &info->cmd_io_code) and
        dmi_stream_decode(stream, dmi_dword_t, &info->supported_cmds);
    if (not status)
        return false;

    // Tokens are terminated by the end-of-table marker, which may be
    // truncated itself
    size_t capacity = dmi_stream_remaining(stream) / DMI_DELL_CALLING_IFACE_TOKEN_SIZE;
    if (capacity > 0) {
        info->tokens = dmi_alloc_array(context, sizeof(*info->tokens), capacity);
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

        dmi_dell_calling_iface_token_t token = { .id = id };

        status =
            dmi_stream_decode(stream, dmi_word_t, &token.location) and
            dmi_stream_decode(stream, dmi_word_t, &token.value);
        if (not status)
            return dmi_entity_incomplete(entity);

        if (id != DMI_DELL_TOKEN_UNUSED)
            info->tokens[info->token_count++] = token;
    }

    return true;
}

void dmi_dell_calling_iface_cleanup(dmi_entity_t *entity)
{
    dmi_dell_calling_iface_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(DELL_CALLING_IFACE));
    if (info == nullptr)
        return;

    dmi_free(info->tokens);
}

static bool dmi_dell_calling_iface_encode_token(
        dmi_encoder_t                        *encoder,
        const dmi_dell_calling_iface_token_t *token)
{
    return
        dmi_encoder_put(encoder, dmi_word_t, token->id) and
        dmi_encoder_put(encoder, dmi_word_t, token->location) and
        dmi_encoder_put(encoder, dmi_word_t, token->value);
}

//
// Tokens are written in turn, and are terminated by the end-of-table marker.
//
bool dmi_dell_calling_iface_encode(dmi_encoder_t *encoder)
{
    const dmi_dell_calling_iface_t *info = dmi_entity_info(encoder->entity, DMI_TYPE(DELL_CALLING_IFACE));
    if (info == nullptr)
        return false;

    bool status =
        dmi_encoder_put(encoder, dmi_word_t, info->cmd_io_address) and
        dmi_encoder_put(encoder, dmi_byte_t, info->cmd_io_code) and
        dmi_encoder_put(encoder, dmi_dword_t, info->supported_cmds);
    if (not status)
        return false;

    // Tokens the decoder drops as unused are not in the model, so the records
    // of the source data are walked in the preserve mode: the unused ones are
    // kept as they are, and the rest are written from the model in turn
    size_t next = 0;

    while ((encoder->mode == DMI_ENCODE_MODE_PRESERVE) and (next < info->token_count)) {
        dmi_byte_t id[2];

        if (not dmi_encoder_peek(encoder, id, sizeof(id)))
            break;

        dmi_word_t original = (dmi_word_t)(id[0] | (id[1] << 8));

        if ((original == DMI_DELL_TOKEN_EOT) or (dmi_encoder_remaining(encoder) < DMI_DELL_CALLING_IFACE_TOKEN_SIZE))
            break;

        if (original == DMI_DELL_TOKEN_UNUSED) {
            if (not dmi_encoder_copy(encoder, DMI_DELL_CALLING_IFACE_TOKEN_SIZE))
                return false;
            continue;
        }

        if (not dmi_dell_calling_iface_encode_token(encoder, &info->tokens[next++]))
            return false;
    }

    // Tokens the source data has no records for, and all of them in the
    // canonical mode
    while (next < info->token_count) {
        if (not dmi_dell_calling_iface_encode_token(encoder, &info->tokens[next++]))
            return false;
    }

    // Marker ending the tokens is kept along with whatever follows it in the
    // preserve mode, and written in the canonical one
    if (encoder->mode == DMI_ENCODE_MODE_CANONICAL)
        return dmi_encoder_put(encoder, dmi_word_t, DMI_DELL_TOKEN_EOT);

    return true;
}
