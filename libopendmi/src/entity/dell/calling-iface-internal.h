//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//

#ifndef OPENDMI_ENTITY_CALLING_IFACE_INTERNAL_H
#define OPENDMI_ENTITY_CALLING_IFACE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/dell/calling-iface.h>

/**
 * @internal
 * @brief Size of the token in the structure.
 */
#define DMI_DELL_CALLING_IFACE_TOKEN_SIZE 6


/**
 * @internal
 * @brief Attributes of a token of the calling interface.
 */
extern const dmi_attribute_t dmi_dell_calling_iface_token_attrs[];

/**
 * @internal
 * @brief Decode the structure along with its tokens.
 *
 * @details Tokens are terminated by the end-of-table marker, which may be
 * truncated itself. Unused tokens are dropped.
 *
 * @param[in,out] decoder Decoder of the structure.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_dell_calling_iface_decode(dmi_decoder_t *decoder);

/**
 * @internal
 * @brief Encode the structure along with its tokens.
 *
 * @details Tokens are written in turn, and are terminated by the end-of-table
 * marker, see `dmi_dell_tokens_encode()`.
 *
 * @param[in,out] encoder Encoder of the structure.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_dell_calling_iface_encode(dmi_encoder_t *encoder);

/**
 * @internal
 * @brief Release the tokens of a decoded structure.
 *
 * @param[in] entity Entity of the structure.
 */
void dmi_dell_calling_iface_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_CALLING_IFACE_INTERNAL_H
