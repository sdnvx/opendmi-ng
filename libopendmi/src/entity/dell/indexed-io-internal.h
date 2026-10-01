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

#include "tokens-internal.h"

/**
 * @brief Size of the token in the structure.
 */
#define DMI_DELL_INDEXED_IO_TOKEN_SIZE 5

extern const dmi_attribute_t dmi_dell_indexed_io_token_attrs[];

bool dmi_dell_indexed_io_decode(dmi_decoder_t *decoder);

/**
 * @internal
 * @brief Encode a Dell indexed I/O structure.
 *
 * @details Tokens are written in turn, and are terminated by the end-of-table
 * marker.
 */
bool dmi_dell_indexed_io_encode(dmi_encoder_t *encoder);

void dmi_dell_indexed_io_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_INDEXED_IO_INTERNAL_H
