//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_TPM_DEVICE_H
#define OPENDMI_ENTITY_TPM_DEVICE_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_tpm_device           dmi_tpm_device_t;
typedef union  dmi_tpm_device_features  dmi_tpm_device_features_t;
typedef struct dmi_tpm_firmware_version dmi_tpm_firmware_version_t;

/**
 * @brief Firmware version formats, depending on the TPM version.
 */
typedef enum dmi_tpm_firmware_version_format
{
    DMI_TPM_FIRMWARE_VERSION_FORMAT_RAW,   ///< Unknown TPM version, raw value
    DMI_TPM_FIRMWARE_VERSION_FORMAT_TPM_1, ///< Revision of TCPA_VERSION structure
    DMI_TPM_FIRMWARE_VERSION_FORMAT_TPM_2  ///< TPM_PT_FIRMWARE_VERSION_1 and _2 properties
} dmi_tpm_firmware_version_format_t;

/**
 * @brief TPM 2.0 firmware version.
 */
struct dmi_tpm_firmware_version
{
    /**
     * @brief Major version number, the high word of TPM_PT_FIRMWARE_VERSION_1.
     */
    uint16_t major;

    /**
     * @brief Minor version number, the low word of TPM_PT_FIRMWARE_VERSION_1.
     */
    uint16_t minor;

    /**
     * @brief Vendor-specific value of TPM_PT_FIRMWARE_VERSION_2.
     */
    uint32_t vendor_specific;
};

/**
 * @brief TPM device characteristics structure (type 43).
 */
dmi_packed_union(dmi_tpm_device_features)
{
    /**
     * @brief Raw value.
     */
    uint64_t __value;

    dmi_packed_struct()
    {
        /**
         * @brief Reserved for future use.
         */
        uint64_t reserved_1 : 2;

        /**
         * @brief TPM Device Characteristics are not supported.
         */
        uint64_t is_unsupported : 1;

        /**
         * @brief Family configurable via firmware update. For example, switching
         * between TPM 1.2 and TPM 2.0.
         */
        uint64_t is_update_configurable : 1;

        /**
         * @brief Family configurable via platform software support, such as
         * firmware setup. For example, switching between TPM 1.2 and TPM 2.0.
         */
        uint64_t is_software_configurable : 1;

        /**
         * @brief Family configurable via OEM proprietary mechanism. For example,
         * switching between TPM 1.2 and TPM 2.0.
         */
        uint64_t is_proprietary_configurable : 1;

        /**
         * @brief Reserved for future use.
         */
        uint64_t reserved_2 : 58;
    };
};

dmi_static_assert_value_union(dmi_tpm_device_features);

/**
 * @brief TPM device.
 */
struct dmi_tpm_device
{
    /**
     * @brief Vendor identifier, as defined by TCG Vendor ID (see CAP_VID in
     * TCG Vendor ID Registry).
     */
    char vendor_id[5];

    /**
     * @brief Vendor identifier as a string of printable characters,
     * @c nullptr if there are none. Points to `vendor_id`, which is
     * terminated.
     */
    const char *vendor;

    /**
     * @brief TPM version supported by the TPM device.
     */
    dmi_version_t spec_version;

    /**
     * @brief TPM vendor-specific value for firmware version, both double
     * words as they are stored.
     */
    uint64_t firmware_version;

    /**
     * @brief Format of firmware version, depending on the TPM version.
     * Selects the field containing the parsed firmware version.
     */
    dmi_tpm_firmware_version_format_t firmware_version_format;

    /**
     * @brief Firmware revision of TPM 1.x (revMajor and revMinor of
     * TCPA_VERSION).
     */
    dmi_version_t firmware_revision;

    /**
     * @brief Firmware version of TPM 2.0.
     */
    dmi_tpm_firmware_version_t firmware_version_2;

    /**
     * @brief descriptive information of the TPM device.
     */
    const char *description;

    /**
     * @brief TPM device characteristics information.
     */
    dmi_tpm_device_features_t features;

    /**
     * @brief OEM- or firmware vendor-specific information.
     */
    uint32_t oem_defined;
};

/**
 * @brief TPM device entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_tpm_device_spec;

#endif // !OPENDMI_ENTITY_TPM_DEVICE_H
