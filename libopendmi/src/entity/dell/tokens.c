//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <assert.h>

#include <opendmi/decoder.h>
#include <opendmi/encoder.h>
#include <opendmi/entity.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>

#include "tokens-internal.h"

/**
 * @internal
 * @brief Write the tokens in place of the records of the source data, which
 * keeps the unused records as they are.
 *
 * @details Walk stops at the end-of-table marker, or once every token has
 * been written.
 *
 * @param[in,out] encoder     Encoder of the structure.
 * @param[in]     tokens      Tokens of the decoded structure.
 * @param[in]     token_size  Size of a token of the decoded structure.
 * @param[in]     token_count Number of the tokens.
 * @param[in]     record_size Size of a token in the structure.
 * @param[in]     encode      Function writing a single token.
 * @param[out]    pnext       Variable to store the number of the tokens
 *                            written in.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_dell_tokens_preserve(
        dmi_encoder_t            *encoder,
        const dmi_byte_t         *tokens,
        size_t                    token_size,
        size_t                    token_count,
        size_t                    record_size,
        dmi_dell_token_encode_fn *encode,
        size_t                   *pnext);

bool dmi_dell_tokens_decode(
        dmi_decoder_t            *decoder,
        size_t                    token_size,
        size_t                    record_size,
        dmi_dell_token_decode_fn *decode,
        void                    **ptokens,
        size_t                   *pcount)
{
    assert(token_size <= DMI_DELL_TOKEN_MAX_SIZE);

    dmi_byte_t *tokens = nullptr;
    size_t      count  = 0;

    *ptokens = nullptr;
    *pcount  = 0;

    // Tokens are terminated by the end-of-table marker, which may be
    // truncated itself
    size_t capacity = dmi_decoder_remaining(decoder) / record_size;
    if (capacity > 0) {
        tokens = dmi_alloc_array(dmi_entity_context(dmi_decoder_entity(decoder)), token_size, capacity);
        if (tokens == nullptr)
            return false;

        *ptokens = tokens;
    }

    while (true) {
        dmi_word_t id = 0;

        if (not dmi_decoder_get(decoder, dmi_word_t, &id))
            return dmi_decoder_incomplete(decoder);

        if (id == DMI_DELL_TOKEN_EOT)
            break;

        // Token is read aside, since the array has no room for a token the
        // structure ends in the middle of. The union aligns the bytes for
        // the token structures, which hold nothing wider than 64 bits.
        union {
            dmi_byte_t bytes[DMI_DELL_TOKEN_MAX_SIZE];
            uint64_t   align;
        } token = {};

        if (not decode(decoder, id, token.bytes))
            return dmi_decoder_incomplete(decoder);

        if (id == DMI_DELL_TOKEN_UNUSED)
            continue;

        memcpy(tokens + count * token_size, token.bytes, token_size);
        *pcount = ++count;
    }

    dmi_decoder_skip(decoder, dmi_decoder_remaining(decoder));

    return true;
}

bool dmi_dell_tokens_encode(
        dmi_encoder_t            *encoder,
        const void               *tokens,
        size_t                    token_size,
        size_t                    token_count,
        size_t                    record_size,
        dmi_dell_token_encode_fn *encode)
{
    const dmi_byte_t *items = tokens;
    size_t next = 0;

    if (encoder->mode == DMI_ENCODE_MODE_PRESERVE) {
        if (not dmi_dell_tokens_preserve(encoder, items, token_size, token_count, record_size, encode, &next))
            return false;
    }

    // Tokens the source data has no records for, and all of them in the
    // canonical mode
    for (; next < token_count; next++) {
        if (not encode(encoder, items + next * token_size))
            return false;
    }

    // Marker ending the tokens is kept along with whatever follows it in the
    // preserve mode, and written in the canonical one
    if (encoder->mode == DMI_ENCODE_MODE_CANONICAL)
        return dmi_encoder_put(encoder, dmi_word_t, DMI_DELL_TOKEN_EOT);

    return true;
}

static bool dmi_dell_tokens_preserve(
        dmi_encoder_t            *encoder,
        const dmi_byte_t         *tokens,
        size_t                    token_size,
        size_t                    token_count,
        size_t                    record_size,
        dmi_dell_token_encode_fn *encode,
        size_t                   *pnext)
{
    size_t next = 0;

    while (next < token_count) {
        dmi_byte_t id[2];

        if (not dmi_encoder_peek(encoder, id, sizeof(id)))
            break;

        dmi_word_t original = (dmi_word_t)(id[0] | (id[1] << 8));

        if ((original == DMI_DELL_TOKEN_EOT) or (dmi_encoder_remaining(encoder) < record_size))
            break;

        if (original == DMI_DELL_TOKEN_UNUSED) {
            if (not dmi_encoder_copy(encoder, record_size))
                return false;
            continue;
        }

        if (not encode(encoder, tokens + next * token_size))
            return false;

        next++;
    }

    *pnext = next;

    return true;
}
