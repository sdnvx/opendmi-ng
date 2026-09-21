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

// Value names, see tpm-device-names.c
extern const dmi_name_set_t dmi_tpm_device_feature_names;

// Operation handlers, see tpm-device-handlers.c
bool dmi_tpm_device_decode_vendor_id(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);
bool dmi_tpm_device_encode_vendor_id(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);
bool dmi_tpm_device_decode_version(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);
bool dmi_tpm_device_encode_version(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);
bool dmi_tpm_device_decode_firmware_version(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);
bool dmi_tpm_device_encode_firmware_version(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);
bool dmi_tpm_device_derive(dmi_entity_t *entity);

// Checks the lint rules of the specification perform, see tpm-device-rules.c
void dmi_tpm_device_lint_version(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_tpm_device_lint_vendor(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_TPM_DEVICE_INTERNAL_H
