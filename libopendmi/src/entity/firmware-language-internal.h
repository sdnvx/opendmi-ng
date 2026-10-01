//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_FIRMWARE_LANGUAGE_INTERNAL_H
#define OPENDMI_ENTITY_FIRMWARE_LANGUAGE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/firmware-language.h>

/**
 * @internal
 * @brief Names of the flags of the language information.
 */
extern const dmi_name_set_t dmi_firmware_language_flag_names;

/**
 * @internal
 * @brief Fill in the list of the languages available.
 *
 * @details Languages available are the strings of the structure, so the array
 * points at the ones the structure carries rather than at anything read from
 * the data.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_firmware_language_derive(dmi_entity_t *entity);

/**
 * @internal
 * @brief Free the list of the languages available.
 *
 * @param[in,out] entity Structure being cleaned up.
 */
void dmi_firmware_language_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_FIRMWARE_LANGUAGE_INTERNAL_H
