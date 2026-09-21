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

//
// Version is shown as parsed according to the version format, or as the
// original string, if it does not conform to the format
//
#define dmi_firmware_version_variants(__string, __parsed)                                   \
    DMI_VARIANTS({                                                                          \
        DMI_VARIANT(DMI_VERSION_FORMAT_SEMANTIC, dmi_firmware_inventory_t, __parsed.number, \
                    STRUCT, { .attrs = dmi_firmware_version_number_attrs }),                \
        DMI_VARIANT(DMI_VERSION_FORMAT_HEX_32, dmi_firmware_inventory_t, __parsed.value,    \
                    INTEGER, { .flags = DMI_ATTRIBUTE_FLAG_HEX }),                          \
        DMI_VARIANT(DMI_VERSION_FORMAT_HEX_64, dmi_firmware_inventory_t, __parsed.value,    \
                    INTEGER, { .flags = DMI_ATTRIBUTE_FLAG_HEX }),                          \
        DMI_VARIANT_DEFAULT(dmi_firmware_inventory_t, __string, STRING, {}),                \
        {}                                                                                  \
    })

// Value names, see firmware-inventory-names.c
extern const dmi_name_set_t dmi_version_format_names;
extern const dmi_name_set_t dmi_firmware_ident_format_names;
extern const dmi_name_set_t dmi_firmware_inventory_feature_names;
extern const dmi_name_set_t dmi_firmware_inventory_state_names;

// Operation handlers, see firmware-inventory-handlers.c
extern const dmi_attribute_t dmi_firmware_version_number_attrs[];
bool dmi_firmware_inventory_derive(dmi_entity_t *entity);
void dmi_firmware_inventory_cleanup(dmi_entity_t *entity);

// Checks the lint rules of the specification perform, see firmware-inventory-rules.c
void dmi_firmware_inventory_lint_version(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_FIRMWARE_INVENTORY_INTERNAL_H
