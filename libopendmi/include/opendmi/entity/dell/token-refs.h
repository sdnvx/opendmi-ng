//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_DELL_TOKEN_REFS_H
#define OPENDMI_ENTITY_DELL_TOKEN_REFS_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_DELL_TOKEN_REFS_1_T
#   define DMI_DELL_TOKEN_REFS_1_T
    typedef struct dmi_dell_token_refs_1 dmi_dell_token_refs_1_t;
#endif // !DMI_DELL_TOKEN_REFS_1_T

#ifndef DMI_DELL_TOKEN_REFS_2_T
#   define DMI_DELL_TOKEN_REFS_2_T
    typedef struct dmi_dell_token_refs_2 dmi_dell_token_refs_2_t;
#endif // !DMI_DELL_TOKEN_REFS_2_T

/**
 * @brief Dell token references structure, type 1 (type 220).
 *
 * Lists tokens, most of which the calling interface (type 218) or the
 * indexed I/O access (type 212) of the same table defines, which the structures
 * of a table group by the device they belong to, e.g. tokens `0xF100` and
 * `0xF102`, `0xF110` and `0xF112`. The meaning of the groups is not established.
 * Reverse engineered from the data corpus.
 */
struct dmi_dell_token_refs_1
{
    /**
     * @brief Number of the elements of `tokens`: 8 or 9 in the known data,
     * as many as the structure holds.
     */
    size_t token_count;

    /**
     * @brief Tokens, zero for none, an array of `token_count` elements.
     */
    uint16_t *tokens;
};

/**
 * @brief Dell token references structure, type 2 (type 221).
 *
 * Lists tokens, most of which the calling interface (type 218) or the
 * indexed I/O access (type 212) of the same table defines, e.g. `0xF600` and `0xF601`, following two values
 * whose meaning is not established. Reverse engineered from the data corpus.
 */
struct dmi_dell_token_refs_2
{
    /**
     * @brief Value whose meaning is not established, `0`, `2` or `3` in the
     * known data.
     */
    uint8_t unknown_1;

    /**
     * @brief Value whose meaning is not established, `0` or `1` in the known
     * data.
     */
    uint16_t unknown_2;

    /**
     * @brief Tokens, zero for none.
     */
    uint16_t tokens[6];
};

/**
 * @brief Dell token references, type 1, entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_dell_token_refs_1_spec;

/**
 * @brief Dell token references, type 2, entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_dell_token_refs_2_spec;

#endif // !OPENDMI_ENTITY_DELL_TOKEN_REFS_H
