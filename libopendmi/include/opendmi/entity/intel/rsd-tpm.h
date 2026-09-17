//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DMI_ENTITY_INTEL_RSD_TPM_H
#define DMI_ENTITY_INTEL_RSD_TPM_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_intel_rsd_tpm dmi_intel_rsd_tpm_t;

/**
 * @brief Intel RSD TPM status values.
 */
typedef enum dmi_intel_rsd_tpm_status
{
    DMI_INTEL_RSD_TPM_STATUS_DISABLED = 0x00, ///< Disabled
    DMI_INTEL_RSD_TPM_STATUS_ENABLED  = 0x01, ///< Enabled
} dmi_intel_rsd_tpm_status_t;

/**
 * @brief Intel RSD TPM information (type 195).
 */
struct dmi_intel_rsd_tpm
{
    /**
     * @brief 1-based index to be used by PSME to select the desired
     * configuration in the `Set TPM Configuration` IPMI command.
     */
    uint8_t config_index;

    /**
     * @brief The string that identifies the version of the TPM present on
     * the system.
     */
    const char *version;

    /**
     * @brief Current status of this specific TPM in the platform.
     */
    dmi_intel_rsd_tpm_status_t status;
};

/**
 * @brief Intel RSD TPM information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_intel_rsd_tpm_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_intel_rsd_tpm_status_name(dmi_intel_rsd_tpm_status_t value);

__END_DECLS

#endif // !DMI_ENTITY_INTEL_RSD_TPM_H
