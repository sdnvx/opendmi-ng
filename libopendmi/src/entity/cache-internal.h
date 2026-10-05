//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_CACHE_INTERNAL_H
#define OPENDMI_ENTITY_CACHE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/cache.h>

/**
 * @internal
 * @brief Names of the cache types.
 */
extern const dmi_name_set_t dmi_cache_type_names;

/**
 * @internal
 * @brief Names of the operational modes of a cache.
 */
extern const dmi_name_set_t dmi_cache_mode_names;

/**
 * @internal
 * @brief Names of the associativities of a cache.
 */
extern const dmi_name_set_t dmi_cache_assoc_names;

/**
 * @internal
 * @brief Names of the locations of a cache relative to the processor.
 */
extern const dmi_name_set_t dmi_cache_location_names;

/**
 * @internal
 * @brief Names of the SRAM types of a cache.
 */
extern const dmi_name_set_t dmi_cache_sram_type_names;

/**
 * @internal
 * @brief Decode the level of a cache.
 *
 * @details Levels are counted from zero in the data and from one everywhere
 * else.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Variable to store the level in.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_cache_decode_level(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Decode a cache size carried in a field of two bytes.
 *
 * @details Sizes are carried in granules, whose width the most significant
 * bit of the field says.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Variable to store the size in.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_cache_decode_size(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Decode a cache size carried in a field of four bytes.
 *
 * @details Sizes are carried in granules, whose width the most significant
 * bit of the field says, see `dmi_cache_size_ex()`.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Variable to store the size in.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_cache_decode_size_ex(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Encode the level of a cache, which undoes `dmi_cache_decode_level()`.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Variable holding the level.
 * @param[out] data  Data the field is to carry.
 *
 * @return Always `true`.
 */
bool dmi_cache_encode_level(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @internal
 * @brief Encode a cache size into a field of two bytes.
 *
 * @details Sizes are written in granules of one kibibyte whenever they fit,
 * and of sixty-four kibibytes otherwise.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Variable holding the size.
 * @param[out] data  Data the field is to carry.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_cache_encode_size(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @internal
 * @brief Encode a cache size into a field of four bytes, which undoes
 * `dmi_cache_decode_size_ex()`.
 *
 * @details Sizes are written in granules of one kibibyte whenever they fit,
 * and of sixty-four kibibytes otherwise.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Variable holding the size.
 * @param[out] data  Data the field is to carry.
 *
 * @return Always `true`.
 */
bool dmi_cache_encode_size_ex(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @internal
 * @brief Check that the installed size of a cache does not exceed its maximum
 * size.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_cache_lint_size(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the current SRAM type of a cache is one of the supported
 * ones.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_cache_lint_sram(dmi_lint_t *lint, const dmi_entity_t *entity);


/**
 * @internal
 * @brief Decode cache size from a 16-bit SMBIOS value.
 *
 * Converts the raw 16-bit cache size field into a size in bytes. Bit 15
 * determines the granularity: 1 Kb when clear, 64 Kb when set.
 *
 * @param[in] value Raw 16-bit cache size value.
 *
 * @return Cache size in bytes.
 */
dmi_size_t dmi_cache_size(uint16_t value);

/**
 * @internal
 * @brief Decode cache size from a 32-bit SMBIOS value.
 *
 * Converts the raw 32-bit extended cache size field into a size in bytes.
 * Bit 31 determines the granularity: 1 Kb when clear, 64 Kb when set.
 * Used for caches larger than 2047 MiB (SMBIOS 3.1+).
 *
 * @param[in] value Raw 32-bit cache size value.
 *
 * @return Cache size in bytes.
 */
dmi_size_t dmi_cache_size_ex(uint32_t value);

#endif // !OPENDMI_ENTITY_CACHE_INTERNAL_H
