//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_TPM_DEVICE_INTERNAL_H
#define OPENDMI_ENTITY_TPM_DEVICE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/tpm-device.h>

/**
 * @internal
 * @brief Offset of the vendor identifier, which is four characters long.
 */
#define DMI_TPM_DEVICE_VENDOR_OFFSET 0x04

/**
 * @internal
 * @brief Names of the TPM device characteristics.
 */
extern const dmi_name_set_t dmi_tpm_device_chars_names;

/**
 * @internal
 * @brief Decode the specification version.
 *
 * @details Specification version is one byte of major and one of minor.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Variable to store the version in.
 *
 * @return `true` if the data has been decoded, `false` otherwise.
 */
bool dmi_tpm_device_decode_version(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Encode the specification version, which undoes
 * `dmi_tpm_device_decode_version()`.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Variable holding the version.
 * @param[out] data  Data the field is to carry.
 *
 * @return Always `true`.
 */
bool dmi_tpm_device_encode_version(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @internal
 * @brief Decode the firmware version.
 *
 * @details Firmware version is two double words, of which the first one is
 * the more significant half of the number they spell together.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Variable to store the version in.
 *
 * @return `true` if the data has been decoded, `false` otherwise.
 */
bool dmi_tpm_device_decode_firmware_version(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Encode the firmware version, which undoes
 * `dmi_tpm_device_decode_firmware_version()`.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Variable holding the version.
 * @param[out] data  Data the field is to carry.
 *
 * @return Always `true`.
 */
bool dmi_tpm_device_encode_firmware_version(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @internal
 * @brief Derive the vendor and the format of the firmware version.
 *
 * @details Vendor and firmware version mean what the version of the
 * specification says they do, so they are read once the fields are there.
 *
 * Vendor identifier is four bytes of text, which some firmware stores as a
 * little-endian double word, so that it starts with the terminating zero,
 * e.g. `"\0XFI"` for `"IFX"`. Only printable characters are kept, and the
 * identifier ends at the first other.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_tpm_device_derive(dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the major specification version is either 1 or 2.
 *
 * @details TCG has published the 1.2 and the 2.0 specifications, and the
 * format of the firmware version follows the major one.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_tpm_device_lint_version(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the vendor identifier is not stored reversed.
 *
 * @details Firmware of some vendors stores the identifier as a little-endian
 * double word, so that it starts with the terminating zero, e.g. `"\0XFI"`
 * for `"IFX"`. The decoder puts it back in place, and the raw data still
 * shows it.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_tpm_device_lint_vendor(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_TPM_DEVICE_INTERNAL_H
