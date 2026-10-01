//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_ADDITIONAL_INFO_INTERNAL_H
#define OPENDMI_ENTITY_ADDITIONAL_INFO_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/additional-info.h>

// Operation handlers, see additional-info-handlers.c

/**
 * @internal
 * @brief Decode the additional information entries of a structure.
 *
 * @param[in,out] decoder Decoder of the structure.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_additional_info_decode(dmi_decoder_t *decoder);

/**
 * @internal
 * @brief Encode the additional information entries of a structure.
 *
 * @details Entries are written with the length of their value. The length
 * the source data declares is kept whenever it reads as the same value,
 * which it does when the structure ends before the value and the decoder has
 * taken what is there.
 *
 * @param[in,out] encoder Encoder of the structure.
 *
 * @error DMI_ERROR_INVALID_ARGUMENT Entry is too long for its length byte
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_additional_info_encode(dmi_encoder_t *encoder);

/**
 * @internal
 * @brief Free the entries of a decoded structure.
 *
 * @param[in,out] entity Structure being cleaned up.
 */
void dmi_additional_info_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_ADDITIONAL_INFO_INTERNAL_H
