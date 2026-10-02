//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_FIRMWARE_INVENTORY_INTERNAL_H
#define OPENDMI_ENTITY_FIRMWARE_INVENTORY_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/firmware-inventory.h>

/**
 * @internal
 * @brief Names of the formats of a firmware version.
 */
extern const dmi_name_set_t dmi_firmware_version_format_names;

/**
 * @internal
 * @brief Names of the formats of a firmware identifier.
 */
extern const dmi_name_set_t dmi_firmware_ident_format_names;

/**
 * @internal
 * @brief Names of the characteristics of a firmware component.
 */
extern const dmi_name_set_t dmi_firmware_inventory_feature_names;

/**
 * @internal
 * @brief Names of the states of a firmware component.
 */
extern const dmi_name_set_t dmi_firmware_inventory_state_names;

/**
 * @internal
 * @brief Attributes of a firmware version number, which is a major and a minor
 * number.
 */
extern const dmi_attribute_t dmi_firmware_version_number_attrs[];

/**
 * @internal
 * @brief Parse the version and the identifier of a firmware component.
 *
 * @details Versions and identifiers are written as strings, which are read
 * according to the formats the structure declares for them.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_firmware_inventory_derive(dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the installed version of a firmware component is not
 * older than the lowest supported one.
 *
 * @details Versions are comparable when they are written the same way, and
 * the one installed is not older than the oldest one the component supports.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_firmware_inventory_lint_version(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_FIRMWARE_INVENTORY_INTERNAL_H
