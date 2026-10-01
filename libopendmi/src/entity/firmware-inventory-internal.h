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
 * @brief Variants of a firmware version attribute.
 *
 * @details Version is shown as parsed according to the version format, or as
 * the original string, if it does not conform to the format.
 *
 * @param __string Member holding the version string.
 * @param __parsed Member holding the parsed version.
 */
#define dmi_firmware_version_variants(__string, __parsed)                                            \
    DMI_VARIANTS({                                                                                   \
        DMI_VARIANT(DMI_FIRMWARE_VERSION_FORMAT_SEMANTIC, dmi_firmware_inventory_t, __parsed.number, \
                    STRUCT, { .attrs = dmi_firmware_version_number_attrs }),                         \
        DMI_VARIANT(DMI_FIRMWARE_VERSION_FORMAT_HEX_32, dmi_firmware_inventory_t, __parsed.value,    \
                    INTEGER, { .flags = DMI_ATTRIBUTE_FLAG_HEX }),                                   \
        DMI_VARIANT(DMI_FIRMWARE_VERSION_FORMAT_HEX_64, dmi_firmware_inventory_t, __parsed.value,    \
                    INTEGER, { .flags = DMI_ATTRIBUTE_FLAG_HEX }),                                   \
        DMI_VARIANT_DEFAULT(dmi_firmware_inventory_t, __string, STRING, {}),                         \
        {}                                                                                           \
    })

extern const dmi_name_set_t dmi_firmware_version_format_names;
extern const dmi_name_set_t dmi_firmware_ident_format_names;
extern const dmi_name_set_t dmi_firmware_inventory_feature_names;
extern const dmi_name_set_t dmi_firmware_inventory_state_names;

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
