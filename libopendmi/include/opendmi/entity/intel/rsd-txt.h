//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DMI_ENTITY_INTEL_RSD_TXT_H
#define DMI_ENTITY_INTEL_RSD_TXT_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_intel_rsd_txt dmi_intel_rsd_txt_t;

/**
 * @brief Intel RSD TXT status values.
 */
typedef enum dmi_intel_rsd_txt_status
{
    DMI_INTEL_RSD_TXT_STATUS_DISABLED = 0x00, ///< Disabled
    DMI_INTEL_RSD_TXT_STATUS_ENABLED  = 0x01, ///< Enabled
} dmi_intel_rsd_txt_status_t;

/**
 * @brief Intel RSD TXT information (type 196).
 */
struct dmi_intel_rsd_txt
{
    /**
     * @brief Current status of Intel TXT in the platform.
     */
    dmi_intel_rsd_txt_status_t status;
};

/**
 * @brief Intel RSD TXT information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_intel_rsd_txt_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_intel_rsd_txt_status_name(dmi_intel_rsd_txt_status_t value);

__END_DECLS

#endif // !DMI_ENTITY_INTEL_RSD_TXT_H
