//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_OEM_STRINGS_INTERNAL_H
#define OPENDMI_ENTITY_OEM_STRINGS_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/oem-strings.h>

/**
 * @internal
 * @brief Derive the array of the OEM strings of a structure.
 *
 * @details Strings of the structure are its values, so the array points at
 * the ones the structure carries rather than at anything read from the data.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Array cannot be allocated
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_oem_strings_derive(dmi_entity_t *entity);

/**
 * @internal
 * @brief Free the array of the OEM strings of a decoded structure.
 *
 * @param[in,out] entity Structure being cleaned up.
 */
void dmi_oem_strings_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_OEM_STRINGS_INTERNAL_H
