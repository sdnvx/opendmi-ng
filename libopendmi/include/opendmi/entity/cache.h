//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_CACHE_H
#define OPENDMI_ENTITY_CACHE_H

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <opendmi/entity.h>
#include <opendmi/entity/common.h>

/**
 * @brief Cache types.
 */
typedef enum dmi_cache_type
{
    DMI_CACHE_TYPE_UNSPEC      = 0x00, ///< Unspecified
    DMI_CACHE_TYPE_OTHER       = 0x01, ///< Other
    DMI_CACHE_TYPE_UNKNOWN     = 0x02, ///< Unknown
    DMI_CACHE_TYPE_INSTRUCTION = 0x03, ///< Instruction cache
    DMI_CACHE_TYPE_DATA        = 0x04, ///< Data cache
    DMI_CACHE_TYPE_UNIFIED     = 0x05, ///< Unified cache
    __DMI_CACHE_TYPE_COUNT
} dmi_cache_type_t;

/**
 * @brief Cache operational modes.
 */
typedef enum dmi_cache_mode
{
    DMI_CACHE_MODE_WRITE_THROUGH = 0x00, ///< Write through
    DMI_CACHE_MODE_WRITE_BACK    = 0x01, ///< Write back
    DMI_CACHE_MODE_VARIABLE      = 0x02, ///< Varies with memory address
    DMI_CACHE_MODE_UNKNOWN       = 0x03, ///< Unknown
    __DMI_CACHE_MODE_COUNT
} dmi_cache_mode_t;

/**
 * @brief Cache associativity types.
 */
typedef enum dmi_cache_assoc
{
    DMI_CACHE_ASSOC_UNSPEC  = 0x00, ///< Unspecified
    DMI_CACHE_ASSOC_OTHER   = 0x01, ///< Other
    DMI_CACHE_ASSOC_UNKNOWN = 0x02, ///< Unknown
    DMI_CACHE_ASSOC_DIRECT  = 0x03, ///< Direct mapped
    DMI_CACHE_ASSOC_2WAY    = 0x04, ///< 2-way set-associative
    DMI_CACHE_ASSOC_4WAY    = 0x05, ///< 4-way set-associative
    DMI_CACHE_ASSOC_FULL    = 0x06, ///< Fully associative
    DMI_CACHE_ASSOC_8WAY    = 0x07, ///< 8-way set-associative
    DMI_CACHE_ASSOC_16WAY   = 0x08, ///< 16-way set-associative
    DMI_CACHE_ASSOC_12WAY   = 0x09, ///< 12-way set-associative
    DMI_CACHE_ASSOC_24WAY   = 0x0A, ///< 24-way set-associative
    DMI_CACHE_ASSOC_32WAY   = 0x0B, ///< 32-way set-associative
    DMI_CACHE_ASSOC_48WAY   = 0x0C, ///< 48-way set-associative
    DMI_CACHE_ASSOC_64WAY   = 0x0D, ///< 64-way set-associative
    DMI_CACHE_ASSOC_20WAY   = 0x0E, ///< 20-way set-associative
    __DMI_CACHE_ASSOC_COUNT
} dmi_cache_assoc_t;

/**
 * @brief Cache location, relative to the CPU module.
 */
typedef enum dmi_cache_location
{
    DMI_CACHE_LOCATION_INTERNAL = 0x00, ///< Internal
    DMI_CACHE_LOCATION_EXTERNAL = 0x01, ///< External
    DMI_CACHE_LOCATION_RESERVED = 0x02, ///< Reserved
    DMI_CACHE_LOCATION_UNKNOWN  = 0x03, ///< Unknown
    __DMI_CACHE_LOCATION_COUNT
} dmi_cache_location_t;

/**
 * @brief Cache SRAM type.
 */
dmi_packed_union(dmi_cache_sram_type)
{
    /**
     * @brief Encoded type value.
     */
    dmi_word_t __value;

    dmi_packed_struct()
    {
        dmi_word_t other          : 1; ///< Other
        dmi_word_t unknown        : 1; ///< Unknown
        dmi_word_t non_burst      : 1; ///< Non-burst
        dmi_word_t burst          : 1; ///< Burst
        dmi_word_t pipeline_burst : 1; ///< Pipeline burst
        dmi_word_t synchonous     : 1; ///< Synchronous
        dmi_word_t asynchronous   : 1; ///< Asynchronous
    };
};

dmi_static_assert_value_union(dmi_cache_sram_type);

#ifndef DMI_CACHE_SRAM_TYPE_T
#define DMI_CACHE_SRAM_TYPE_T
typedef union dmi_cache_sram_type dmi_cache_sram_type_t;
#endif // !DMI_CACHE_SRAM_TYPE_T

/**
 * @brief Cache information structure (type 7).
 *
 * The information in this structure defines the attributes of CPU cache
 * device in the system. One structure is specified for each such device,
 * whether the device is internal to or external to the CPU module. Cache
 * modules can be associated with a processor structure in one or two ways
 * depending on the SMBIOS version.
 */
struct dmi_cache
{
    /**
     * @brief Socket designator.
     */
    const char *socket_designator;

    /**
     * @brief Cache level, 1 to 8.
     */
    unsigned short level;

    /**
     * @brief Socketed cache flag (e.g., cache on a stick).
     */
    bool socketed;

    /**
     * @brief Location, relative to the CPU module.
     */
    dmi_cache_location_t location;

    /**
     * @brief Enable flag (at boot time).
     */
    bool enabled;

    /**
     * @brief Operational mode.
     */
    dmi_cache_mode_t mode;

    /**
     * @brief Maximum cache size that can be installed, in bytes.
     */
    dmi_size_t maximum_size;

    /**
     * @brief Installed cache size, in bytes.
     */
    dmi_size_t installed_size;

    /**
     * @brief Supported SRAM type.
     */
    dmi_cache_sram_type_t supported_sram;

    /**
     * @brief Current SRAM type.
     */
    dmi_cache_sram_type_t current_sram;

    /**
     * @brief Cache module speed, in nanoseconds. The value is 0 if the speed
     * is unknown.
     */
    unsigned int speed;

    /**
     * @brief Error-correction scheme supported by this cache component.
     */
    dmi_error_correct_type_t error_correction;

    /**
     * @brief Logical type of cache.
     */
    dmi_cache_type_t type;

    /**
     * @brief Associativity of the cache.
     */
    dmi_cache_assoc_t associativity;
};

#ifndef DMI_CACHE_T
#define DMI_CACHE_T
typedef struct dmi_cache dmi_cache_t;
#endif // !DMI_CACHE_T

/**
 * @brief Cache information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_cache_spec;

__BEGIN_DECLS

/**
 * @brief Get cache type name.
 *
 * Returns the human-readable name of the cache type.
 *
 * @param[in] value Cache type value.
 *
 * @return The cache type name string, or @c nullptr if @p value is out of range.
 */
__dmi_api const char *dmi_cache_type_name(dmi_cache_type_t value);

/**
 * @brief Get cache operational mode name.
 *
 * Returns the human-readable name of the cache operational mode.
 *
 * @param[in] value Cache mode value.
 *
 * @return The cache mode name string, or @c nullptr if @p value is out of range.
 */
__dmi_api const char *dmi_cache_mode_name(dmi_cache_mode_t value);

/**
 * @brief Get cache associativity name.
 *
 * Returns the human-readable name of the cache associativity type.
 *
 * @param[in] value Cache associativity value.
 *
 * @return The cache associativity name string, or @c nullptr if @p value is out of
 * range.
 */
__dmi_api const char *dmi_cache_assoc_name(dmi_cache_assoc_t value);

/**
 * @brief Get cache location name.
 *
 * Returns the human-readable name of the cache location relative to the CPU
 * module.
 *
 * @param[in] value Cache location value.
 *
 * @return The cache location name string, or @c nullptr if @p value is out of
 * range.
 */
__dmi_api const char *dmi_cache_location_name(dmi_cache_location_t value);

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
__dmi_api dmi_size_t dmi_cache_size(uint16_t value);

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
__dmi_api dmi_size_t dmi_cache_size_ex(uint32_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_CACHE_H
