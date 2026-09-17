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

typedef struct dmi_tpm_device          dmi_tpm_device_t;
typedef union  dmi_tpm_device_features dmi_tpm_device_features_t;

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
     * @brief TPM version supported by the TPM device.
     */
    dmi_version_t spec_version;

    /**
     * @brief TPM vendor-specific value for firmware version.
     */
    uint64_t firmware_version;

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
