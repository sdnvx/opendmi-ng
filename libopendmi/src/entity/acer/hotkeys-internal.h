//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_ACER_HOTKEYS_INTERNAL_H
#define OPENDMI_ENTITY_ACER_HOTKEYS_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/acer/hotkeys.h>

/**
 * @internal
 * @brief Names of the communication functions.
 */
extern const dmi_name_set_t dmi_acer_comm_function_names;

/**
 * @internal
 * @brief Derive the key of the communication function.
 *
 * @details Key of the communication function is the one the first hotkey holds,
 * and it is left as is in a structure holding no hotkeys.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_acer_hotkeys_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_ACER_HOTKEYS_INTERNAL_H
