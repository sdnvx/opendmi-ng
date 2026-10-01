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

extern const dmi_name_set_t dmi_tpm_device_feature_names;


/**
 * @internal
 * @brief Decode the vendor identifier.
 *
 * @details Vendor identifier is four bytes of text, which some firmware stores
 * as a little-endian double word, so that it starts with the terminating zero,
 * e.g. `"\0XFI"` for `"IFX"`. Only printable characters are kept, and the
 * identifier ends at the first other.
 */
bool dmi_tpm_device_decode_vendor_id(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);
bool dmi_tpm_device_encode_vendor_id(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @internal
 * @brief Decode the specification version.
 *
 * @details Specification version is one byte of major and one of minor.
 */
bool dmi_tpm_device_decode_version(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

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
 */
bool dmi_tpm_device_decode_firmware_version(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

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
 */
bool dmi_tpm_device_derive(dmi_entity_t *entity);

// Checks the lint rules of the specification perform, see tpm-device-rules.c
void dmi_tpm_device_lint_version(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the vendor identifier is not stored reversed.
 *
 * @details Firmware of some vendors stores the identifier as a little-endian
 * double word, so that it starts with the terminating zero, e.g. `"\0XFI"`
 * for `"IFX"`. The decoder puts it back in place, and the raw data still
 * shows it.
 */
void dmi_tpm_device_lint_vendor(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_TPM_DEVICE_INTERNAL_H
