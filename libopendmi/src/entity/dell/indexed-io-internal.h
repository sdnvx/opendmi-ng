//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INDEXED_IO_INTERNAL_H
#define OPENDMI_ENTITY_INDEXED_IO_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/dell/indexed-io.h>

/**
 * @internal
 * @brief Size of the token in the structure.
 */
#define DMI_DELL_INDEXED_IO_TOKEN_SIZE 5

/**
 * @internal
 * @brief Attributes of a token of a Dell indexed I/O structure.
 */
extern const dmi_attribute_t dmi_dell_indexed_io_token_attrs[];

/**
 * @internal
 * @brief Decode a Dell indexed I/O structure.
 *
 * @details Tokens follow the ports and the checksum information, and are
 * terminated by the end-of-table marker, which may be truncated itself.
 * Unused tokens are skipped, and a token without a mask is a string one,
 * whose value is the length of the string.
 *
 * @param[in,out] decoder Decoder of the structure.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Array of the tokens cannot be allocated
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_dell_indexed_io_decode(dmi_decoder_t *decoder);

/**
 * @internal
 * @brief Encode a Dell indexed I/O structure.
 *
 * @details Tokens are written in turn, and are terminated by the end-of-table
 * marker.
 *
 * @param[in,out] encoder Encoder of the structure.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_dell_indexed_io_encode(dmi_encoder_t *encoder);

/**
 * @internal
 * @brief Free the tokens of a decoded structure.
 *
 * @param[in,out] entity Structure being cleaned up.
 */
void dmi_dell_indexed_io_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_INDEXED_IO_INTERNAL_H
