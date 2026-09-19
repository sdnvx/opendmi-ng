//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_DELL_PROTECTED_AREA_2_H
#define OPENDMI_ENTITY_DELL_PROTECTED_AREA_2_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/entity/dell/common.h>

typedef struct dmi_dell_protected_area_2 dmi_dell_protected_area_2_t;

/**
 * @brief Dell protected area type 2 structure (type 215).
 */
struct dmi_dell_protected_area_2
{
    /**
     * @brief Token identifier of the protected value.
     */
    uint16_t token_id;

    /**
     * @brief Protected value length.
     */
    uint8_t value_length;

    /**
     * @brief Protected value format.
     */
    dmi_dell_value_format_t value_format;

    /**
     * @brief Validation key.
     */
    uint16_t validation_key;

    /**
     * @brief Index I/O port.
     */
    uint16_t index_port;

    /**
     * @brief Data I/O port.
     */
    uint16_t data_port;

    /**
     * @brief Checksum type of the protected value.
     */
    dmi_dell_check_type_t check_type;

    /**
     * @brief Index of the protected value.
     */
    uint8_t value_start;

    /**
     * @brief Index of the checksum value.
     */
    uint8_t check_index;

    /**
     * @brief Checksum type of the checked range.
     */
    dmi_dell_check_type_t range_check_type;

    /**
     * @brief First index of the checked range.
     */
    uint8_t range_check_start;

    /**
     * @brief Last index of the checked range.
     */
    uint8_t range_check_end;

    /**
     * @brief Index of the checksum value of the checked range.
     */
    uint8_t range_check_index;
};

/**
 * @brief Dell protected area type 2 entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_dell_protected_area_2_spec;

#endif // !OPENDMI_ENTITY_DELL_PROTECTED_AREA_2_H
