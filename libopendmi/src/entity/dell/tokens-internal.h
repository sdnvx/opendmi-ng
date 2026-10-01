//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_DELL_TOKENS_INTERNAL_H
#define OPENDMI_ENTITY_DELL_TOKENS_INTERNAL_H

#pragma once

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
