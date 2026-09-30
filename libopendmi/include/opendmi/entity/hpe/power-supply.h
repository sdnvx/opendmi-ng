//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_POWER_SUPPLY_H
#define OPENDMI_ENTITY_HPE_POWER_SUPPLY_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_hpe_power_supply dmi_hpe_power_supply_t;

/**
 * @brief Method the FRU of a power supply is accessed by.
 */
typedef enum dmi_hpe_fru_access
{
    DMI_HPE_FRU_ACCESS_NONE    = 0x00, ///< Not available
    DMI_HPE_FRU_ACCESS_IPMI    = 0x01, ///< IPMI I2C
    DMI_HPE_FRU_ACCESS_ILO     = 0x02, ///< iLO
    DMI_HPE_FRU_ACCESS_CHASSIS = 0x03  ///< Chassis manager
} dmi_hpe_fru_access_t;

/**
 * @brief HP/HPE power supply information (type 230).
 *
 * Supplements the system power supply information (type 39) with the actual
 * manufacturer of the power supply and the way its FRU is accessed.
 */
struct dmi_hpe_power_supply
{
    /**
     * @brief Handle of the system power supply structure (type 39).
     */
    dmi_handle_t power_supply_handle;

    /**
     * @brief Actual manufacturer.
     */
    const char *manufacturer;

    /**
     * @brief Revision.
     */
    const char *revision;

    /**
     * @brief Method the FRU is accessed by.
     */
    dmi_hpe_fru_access_t fru_access;

    /**
     * @brief I2C bus, or I2C segment with iLO. Set to `UINT8_MAX` when not
     * available.
     */
    uint8_t i2c_bus;

    /**
     * @brief I2C address, shifted left by one bit. Set to `UINT8_MAX` when
     * not available.
     */
    uint8_t i2c_address;
};

/**
 * @brief HP/HPE power supply information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_power_supply_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_hpe_fru_access_name(dmi_hpe_fru_access_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_HPE_POWER_SUPPLY_H
