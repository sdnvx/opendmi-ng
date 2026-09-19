//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_BIS_ENTRY_POINT_H
#define OPENDMI_ENTITY_BIS_ENTRY_POINT_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_bis_entry_point       dmi_bis_entry_point_t;
typedef struct dmi_bis_real_mode_address dmi_bis_real_mode_address_t;

/**
 * @brief Real mode (16:16) address.
 */
struct dmi_bis_real_mode_address
{
    /**
     * @brief Segment.
     */
    uint16_t segment;

    /**
     * @brief Offset.
     */
    uint16_t offset;
};

/**
 * @brief Boot Integrity Services (BIS) entry point structure (type 31).
 */
struct dmi_bis_entry_point
{
    /**
     * @brief Checksum, which makes the sum of all bytes of the structure
     * equal to zero.
     */
    uint8_t checksum;

    /**
     * @brief BIS entry point for 16-bit real mode.
     */
    dmi_bis_real_mode_address_t entry_point_16;

    /**
     * @brief BIS entry point for 32-bit protected mode.
     */
    uint32_t entry_point_32;

    /**
     * @brief Set if the checksum is valid.
     */
    bool is_valid;
};

/**
 * @brief Boot Integrity Services (BIS) entry point entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_bis_entry_point_spec;

#endif // !OPENDMI_ENTITY_BIS_ENTRY_POINT_H
