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

extern const dmi_name_set_t dmi_firmware_feature_names;
extern const dmi_name_set_t dmi_firmware_feature_ex_names;

/**
 * @internal
 * @brief Decode the release date of the firmware.
 *
 * @details Release date is written as a string, and the structures whose
 * string is missing or malformed carry no date at all.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Variable to store the date in.
 *
 * @return Always `true`.
 */
bool dmi_firmware_decode_date(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Decode the size of the firmware ROM.
 *
 * @details ROM size is carried as the number of the 64K granules it takes.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Variable to store the size in.
 *
 * @return `true` if the data has been decoded, `false` otherwise.
 */
bool dmi_firmware_decode_rom_size(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

bool dmi_firmware_decode_rom_size_ex(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Decode the version of the system or the embedded controller
 * firmware.
 *
 * @details Versions are one byte of major and one of minor, and the major
 * number of 0xFF says that the platform carries no version at all.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Variable to store the version in.
 *
 * @return `true` if the data has been decoded, `false` otherwise.
 */
bool dmi_firmware_decode_version(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Encode the release date of the firmware, which undoes
 * `dmi_firmware_decode_date()`.
 *
 * @details Release date is written the way the specification spells it,
 * mm/dd/yyyy, and no date is written as no string.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Variable holding the date.
 * @param[out] data  Data the field is to carry.
 *
 * @return Always `true`.
 */
bool dmi_firmware_encode_date(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

bool dmi_firmware_encode_rom_size(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @internal
 * @brief Encode the extended size of the firmware ROM.
 *
 * @details Extended size is written in megabytes whenever it fits, and in
 * gigabytes otherwise, the way the two most significant bits of the field
 * tell them apart.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Variable holding the size.
 * @param[out] data  Data the field is to carry.
 *
 * @return Always `true`.
 */
bool dmi_firmware_encode_rom_size_ex(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @internal
 * @brief Encode the version of the system or the embedded controller
 * firmware, which undoes `dmi_firmware_decode_version()`.
 *
 * @details Platform which carries no version says so by the major number of
 * 0xFF.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Variable holding the version.
 * @param[out] data  Data the field is to carry.
 *
 * @return Always `true`.
 */
bool dmi_firmware_encode_version(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @internal
 * @brief Check that a ROM size referring to the extended one is not given by
 * a structure which carries no extended size.
 *
 * @details Size of 0xFF means that the actual one is in the extended field,
 * which was added in SMBIOS 3.1, so a structure of an earlier version carries
 * no size at all.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_firmware_lint_rom_size(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the release date of the firmware is written as
 * mm/dd/yyyy.
 *
 * @details Release date is a string, which the specification requires to be
 * written as mm/dd/yyyy since SMBIOS 2.3, while the earlier two-digit year
 * leaves the century to the reader.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_firmware_lint_release_date(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_FIRMWARE_INTERNAL_H
