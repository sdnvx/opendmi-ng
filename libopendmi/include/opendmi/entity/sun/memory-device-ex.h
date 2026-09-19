//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SUN_MEMORY_DEVICE_EX_H
#define OPENDMI_ENTITY_SUN_MEMORY_DEVICE_EX_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_sun_memory_device_ex dmi_sun_memory_device_ex_t;

/**
 * @brief Sun memory device extended information (type 145).
 */
struct dmi_sun_memory_device_ex
{
    /**
     * @brief Handle of the extended memory device structure.
     */
    dmi_handle_t memory_device_handle;

    /**
     * @brief DRAM channel.
     */
    uint8_t dram_channel;

    /**
     * @brief Number of chip selects, as specified in the structure.
     */
    uint8_t chip_select_total;

    /**
     * @brief Number of present chip selects.
     */
    size_t chip_select_count;

    /**
     * @brief Chip selects.
     */
    uint8_t *chip_selects;
};

/**
 * @brief Sun memory device extended information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_sun_memory_device_ex_spec;

#endif // !OPENDMI_ENTITY_SUN_MEMORY_DEVICE_EX_H
