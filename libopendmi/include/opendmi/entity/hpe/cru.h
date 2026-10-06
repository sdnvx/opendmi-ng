//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_CRU_H
#define OPENDMI_ENTITY_HPE_CRU_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_HPE_CRU_T
#   define DMI_HPE_CRU_T
    typedef struct dmi_hpe_cru dmi_hpe_cru_t;
#endif // !DMI_HPE_CRU_T

/**
 * @brief HP/HPE 64-bit CRU information (type 212), up to Gen8.
 *
 * Tells where the 64-bit entry point of the Compaq ROM Utility (CRU) services
 * of the firmware is, which the watchdog timer driver calls.
 */
struct dmi_hpe_cru
{
    /**
     * @brief Signature, as the structure holds it.
     */
    dmi_binary_t signature_raw;

    /**
     * @brief Signature, `$CRU` for the CRU services, or @c nullptr if it is
     * not printable. Some servers carry records of other signatures too, e.g.
     * `$SHF`, whose meaning is not known.
     *
     * The string belongs to the structure, and is freed along with it.
     */
    char *signature;

    /**
     * @brief Physical address of the service area.
     */
    uint64_t address;

    /**
     * @brief Length of the service area.
     */
    uint32_t length;

    /**
     * @brief Offset of the entry point within the service area.
     */
    uint32_t offset;

    /**
     * @brief Physical address of the entry point.
     */
    uint64_t entry_point;

    /**
     * @brief Whether the record describes the CRU services, as its signature
     * `$CRU` tells. The address, the length, the offset and the entry point
     * are shown for these records only.
     */
    bool is_cru;
};

/**
 * @brief HP/HPE 64-bit CRU information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_cru_spec;

#endif // !OPENDMI_ENTITY_HPE_CRU_H
