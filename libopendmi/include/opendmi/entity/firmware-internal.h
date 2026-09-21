//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_FIRMWARE_INTERNAL_H
#define OPENDMI_ENTITY_FIRMWARE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/firmware.h>

/**
 * @internal
 * @brief Offsets of the fields whose raw values the rules are checked
 * against, and the value standing for the extended ROM size.
 */
#define DMI_FIRMWARE_DATE_OFFSET      0x08

#define DMI_FIRMWARE_ROM_SIZE_OFFSET  0x09

#define DMI_FIRMWARE_ROM_SIZE_EXTENDED 0xFF

// Value names, see firmware-names.c
extern const dmi_name_set_t dmi_firmware_feature_names;
extern const dmi_name_set_t dmi_firmware_feature_ex_names;

// Operation handlers, see firmware-handlers.c
bool dmi_firmware_decode_date(
        dmi_entity_t      *entity,
        const dmi_field_t *field,
        void              *value);
uintmax_t dmi_firmware_convert_rom_size(uintmax_t raw);
uintmax_t dmi_firmware_convert_rom_size_ex(uintmax_t raw);
uintmax_t dmi_firmware_convert_version(uintmax_t raw);

// Checks the lint rules of the specification perform, see firmware-rules.c
void dmi_firmware_lint_rom_size(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_firmware_lint_release_date(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_FIRMWARE_INTERNAL_H
