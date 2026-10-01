//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_RESERVED_MEMORY_H
#define OPENDMI_ENTITY_HPE_RESERVED_MEMORY_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_HPE_RESERVED_MEMORY_T
#   define DMI_HPE_RESERVED_MEMORY_T
    typedef struct dmi_hpe_reserved_memory dmi_hpe_reserved_memory_t;
#endif // !DMI_HPE_RESERVED_MEMORY_T

#ifndef DMI_HPE_RESERVED_MEMORY_ENTRY_T
#   define DMI_HPE_RESERVED_MEMORY_ENTRY_T
    typedef struct dmi_hpe_reserved_memory_entry dmi_hpe_reserved_memory_entry_t;
#endif // !DMI_HPE_RESERVED_MEMORY_ENTRY_T

/**
 * @brief Memory region the firmware reserves.
 */
struct dmi_hpe_reserved_memory_entry
{
    /**
     * @brief Signature, as the structure holds it.
     */
    dmi_binary_t signature_raw;

    /**
     * @brief Signature, which `signature` points to.
     */
    char signature_buffer[5];

    /**
     * @brief Signature, which tells what the region is for, e.g. `$HDD`, or
     * @c nullptr if it is not printable.
     */
    const char *signature;

    /**
     * @brief Physical address.
     */
    uint64_t address;

    /**
     * @brief Size in the units `is_kilobytes` tells.
     */
    uint32_t raw_size;

    /**
     * @brief Whether the size is in kilobytes rather than in bytes.
     */
    bool is_kilobytes;

    /**
     * @brief Size in bytes.
     */
    uint64_t size;
};

/**
 * @brief HP/HPE reserved memory location (type 229).
 *
 * Tells where the memory regions the firmware reserves at boot are, e.g. the
 * one the storage controller and iLO exchange the temperatures of the drives
 * in.
 */
struct dmi_hpe_reserved_memory
{
    /**
     * @brief Number of regions.
     */
    size_t entry_count;

    /**
     * @brief Regions.
     */
    dmi_hpe_reserved_memory_entry_t *entries;
};

/**
 * @brief HP/HPE reserved memory location entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_reserved_memory_spec;

#endif // !OPENDMI_ENTITY_HPE_RESERVED_MEMORY_H
