//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_BACKPLANE_H
#define OPENDMI_ENTITY_HPE_BACKPLANE_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_hpe_backplane dmi_hpe_backplane_t;

/**
 * @brief HP/HPE HDD backplane FRU information (type 236), up to Gen10 Plus.
 *
 * Describes a drive backplane: where its FRU is, and the SAS expander and
 * the drive bays it has.
 */
struct dmi_hpe_backplane
{
    /**
     * @brief I2C address of the FRU of the backplane, shifted left by one
     * bit.
     */
    uint8_t i2c_address;

    /**
     * @brief Box number of the backplane.
     */
    uint16_t box_number;

    /**
     * @brief NVRAM ID of the backplane.
     */
    uint16_t nvram_id;

    /**
     * @brief WWID of the SAS expander, zero if unknown.
     */
    uint64_t wwid;

    /**
     * @brief Number of the SAS drive bays.
     */
    uint8_t bay_count;

    /**
     * @brief Number of the SAS drive bays behind port 0xA0, deprecated from
     * Gen10 Plus onwards.
     */
    uint8_t a0_bay_count;

    /**
     * @brief Number of the SAS drive bays behind port 0xA2, deprecated from
     * Gen10 Plus onwards.
     */
    uint8_t a2_bay_count;

    /**
     * @brief Name of the backplane, deprecated from Gen10 Plus onwards.
     */
    const char *name;

    /**
     * @brief Whether the numbers of the bays behind ports 0xA0 and 0xA2 and
     * the name are shown, which they are not from Gen10 Plus onwards.
     */
    bool has_legacy_details;
};

/**
 * @brief HP/HPE HDD backplane FRU information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_backplane_spec;

#endif // !OPENDMI_ENTITY_HPE_BACKPLANE_H
