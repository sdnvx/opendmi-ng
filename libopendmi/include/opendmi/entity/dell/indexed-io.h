//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_DELL_INDEXED_IO_H
#define OPENDMI_ENTITY_DELL_INDEXED_IO_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/entity/dell/common.h>

typedef struct dmi_dell_indexed_io       dmi_dell_indexed_io_t;
typedef struct dmi_dell_indexed_io_token dmi_dell_indexed_io_token_t;

/**
 * @brief Token of Dell indexed IO structure.
 */
struct dmi_dell_indexed_io_token
{
    /**
     * @brief Token identifier.
     */
    uint16_t id;

    /**
     * @brief CMOS location (index).
     */
    uint8_t location;

    /**
     * @brief AND mask applied to the value at the location. Zero for string
     * tokens.
     */
    uint8_t and_mask;

    /**
     * @brief Set if the token is a string token.
     */
    bool is_string;

    /**
     * @brief OR value applied to the value at the location, for boolean
     * tokens.
     */
    uint8_t or_value;

    /**
     * @brief String length, for string tokens.
     */
    uint8_t string_length;
};

/**
 * @brief Dell indexed IO structure (type 212).
 */
struct dmi_dell_indexed_io
{
    /**
     * @brief Index I/O port.
     */
    uint16_t index_port;

    /**
     * @brief Data I/O port.
     */
    uint16_t data_port;

    /**
     * @brief Checksum type.
     */
    dmi_dell_check_type_t check_type;

    /**
     * @brief First index of the checked range.
     */
    uint8_t check_start;

    /**
     * @brief Last index of the checked range.
     */
    uint8_t check_end;

    /**
     * @brief Index of the checksum value. Checksum is not used if the checked
     * range and the checksum index are all zeroes.
     */
    uint8_t check_index;

    /**
     * @brief Number of tokens.
     */
    size_t token_count;

    /**
     * @brief Tokens, excluding unused ones and the end-of-table marker.
     */
    dmi_dell_indexed_io_token_t *tokens;
};

/**
 * @brief Dell indexed IO entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_dell_indexed_io_spec;

#endif // !OPENDMI_ENTITY_DELL_INDEXED_IO_H
