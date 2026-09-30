//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_COMMON_INTERNAL_H
#define OPENDMI_ENTITY_HPE_COMMON_INTERNAL_H

#pragma once

#include <opendmi/types.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/hpe/common.h>

extern const dmi_name_set_t dmi_hpe_flag_names;
extern const dmi_name_set_t dmi_hpe_encryption_names;

/**
 * @internal
 * @brief Text the bytes of a structure spell, e.g. a signature.
 *
 * Copies @p length bytes into @p buffer, which is at least one byte longer,
 * leaving the spaces out if @p trim is set, and terminates the copy.
 *
 * @return @p buffer, or @c nullptr if the bytes are not all printable or
 *         leave nothing but spaces.
 */
const char *dmi_hpe_text(const dmi_data_t *data, size_t length, char *buffer, bool trim);

#endif // !OPENDMI_ENTITY_HPE_COMMON_INTERNAL_H
