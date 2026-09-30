//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_APPLE_MEMORY_SPD_DATA_H
#define OPENDMI_ENTITY_APPLE_MEMORY_SPD_DATA_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_apple_memory_spd_data dmi_apple_memory_spd_data_t;

/**
 * @brief Apple memory SPD data structure (type 130).
 *
 * Holds the serial presence detect (SPD) data of a memory module, or a part
 * of it. Laid out as the `AppleSmBios.h` header of OpenCore describes it.
 */
struct dmi_apple_memory_spd_data
{
    /**
     * @brief Handle of the memory device (type 17) of the module.
     */
    dmi_handle_t handle;

    /**
     * @brief Offset of the data within the SPD data of the module.
     */
    uint16_t offset;

    /**
     * @brief Length of the data, in bytes.
     */
    uint16_t size;

    /**
     * @brief SPD data.
     */
    dmi_binary_t data;
};

/**
 * @brief Apple memory SPD data entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_apple_memory_spd_data_spec;

#endif // !OPENDMI_ENTITY_APPLE_MEMORY_SPD_DATA_H
