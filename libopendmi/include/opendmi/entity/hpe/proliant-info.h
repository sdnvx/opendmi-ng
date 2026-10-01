//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_PROLIANT_INFO_H
#define OPENDMI_ENTITY_HPE_PROLIANT_INFO_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_HPE_PROLIANT_INFO_T
#   define DMI_HPE_PROLIANT_INFO_T
    typedef struct dmi_hpe_proliant_info dmi_hpe_proliant_info_t;
#endif // !DMI_HPE_PROLIANT_INFO_T

/**
 * @brief HP/HPE ProLiant information (type 219).
 *
 * Gives the feature flags of a server, which the watchdog timer driver reads.
 */
struct dmi_hpe_proliant_info
{
    /**
     * @brief Power features, whose meaning is not documented.
     */
    uint32_t power_features;

    /**
     * @brief Omega features, whose meaning is not documented.
     */
    uint32_t omega_features;

    /**
     * @brief Miscellaneous features.
     */
    uint32_t misc_features;

    /**
     * @brief Whether the integrated CRU (iCRU) is supported, as bit 0 of the
     * miscellaneous features tells.
     */
    bool is_icru;

    /**
     * @brief Whether the firmware is UEFI one, as bits 10 and 12 of the
     * miscellaneous features tell, the way dmidecode reads them. The `hpwdt`
     * driver of Linux reads bits 3 and 10 instead.
     */
    bool is_uefi;

    /**
     * @brief Whether the structure holds the omega features, which the
     * structures of 8 bytes leave out. The field is not shown then.
     */
    bool has_omega_features;

    /**
     * @brief Whether the structure holds the miscellaneous features, which
     * the structures shorter than 20 bytes leave out. The field and the
     * flags told by it are not shown then.
     */
    bool has_misc_features;
};

/**
 * @brief HP/HPE ProLiant information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_proliant_info_spec;

#endif // !OPENDMI_ENTITY_HPE_PROLIANT_INFO_H
