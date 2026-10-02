//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_DELL_TOKENS_INTERNAL_H
#define OPENDMI_ENTITY_DELL_TOKENS_INTERNAL_H

#pragma once

#include <opendmi/decoder.h>
#include <opendmi/encoder.h>

/**
 * @brief Token identifier of unused tokens.
 */
#define DMI_DELL_TOKEN_UNUSED 0x0000u

/**
 * @brief Token identifier of the end-of-table marker.
 */
#define DMI_DELL_TOKEN_EOT 0xFFFFu

/**
 * @internal
 * @brief Largest size of a token of a decoded structure, which a token is
 * read into before it is stored.
 */
#define DMI_DELL_TOKEN_MAX_SIZE 32

/**
 * @internal
 * @brief Function reading the rest of a single token of a structure, which
 * follows its identifier.
 *
 * @param[in,out] decoder Decoder of the structure.
 * @param[in]     id      Identifier of the token, which is already read.
 * @param[out]    token   Variable to store the token in, which is zeroed.
 *
 * @return `true` on success, `false` if the structure ends in the middle of
 *         the token.
 */
typedef bool dmi_dell_token_decode_fn(dmi_decoder_t *decoder, dmi_word_t id, void *token);

/**
 * @internal
 * @brief Read the tokens of a structure, which are terminated by the
 * end-of-table marker.
 *
 * @details The array of the tokens is allocated for as many tokens as the
 * rest of the structure is able to hold. Unused tokens are dropped, and the
 * tokens are counted only once they are read as a whole. A structure which
 * ends before the marker, or in the middle of the marker, is marked
 * incomplete, and keeps the tokens read up to that point. Whatever follows
 * the marker is skipped.
 *
 * @param[in,out] decoder     Decoder of the structure.
 * @param[in]     token_size  Size of a token of the decoded structure, at
 *                            most `DMI_DELL_TOKEN_MAX_SIZE`.
 * @param[in]     record_size Size of a token in the structure.
 * @param[in]     decode      Function reading a single token.
 * @param[out]    ptokens     Variable to store the array of the tokens in,
 *                            which is set even on failure.
 * @param[out]    pcount      Variable to store the number of the tokens in.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Array of the tokens cannot be allocated
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_dell_tokens_decode(
        dmi_decoder_t            *decoder,
        size_t                    token_size,
        size_t                    record_size,
        dmi_dell_token_decode_fn *decode,
        void                    **ptokens,
        size_t                   *pcount);

/**
 * @internal
 * @brief Function writing a single token of a structure.
 *
 * @param[in,out] encoder Encoder of the structure.
 * @param[in]     token   Token to write.
 *
 * @return `true` on success, `false` otherwise.
 */
typedef bool dmi_dell_token_encode_fn(dmi_encoder_t *encoder, const void *token);

/**
 * @internal
 * @brief Write the tokens of a structure, which are terminated by the
 * end-of-table marker.
 *
 * @details Tokens the decoder drops as unused are not in the model, so the
 * records of the source data are walked in the preserve mode: the unused ones
 * are kept as they are, and the rest are written from the model in turn. The
 * marker is kept along with whatever follows it in the preserve mode, and is
 * written in the canonical one.
 *
 * @param[in,out] encoder     Encoder of the structure.
 * @param[in]     tokens      Tokens of the decoded structure.
 * @param[in]     token_size  Size of a token of the decoded structure.
 * @param[in]     token_count Number of the tokens.
 * @param[in]     record_size Size of a token in the structure.
 * @param[in]     encode      Function writing a single token.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_dell_tokens_encode(
        dmi_encoder_t            *encoder,
        const void               *tokens,
        size_t                    token_size,
        size_t                    token_count,
        size_t                    record_size,
        dmi_dell_token_encode_fn *encode);

#endif // !OPENDMI_ENTITY_DELL_TOKENS_INTERNAL_H
