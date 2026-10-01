//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_ARRAY_ADDR_H
#define OPENDMI_ENTITY_MEMORY_ARRAY_ADDR_H

#pragma once

#include <opendmi/entity/memory-array.h>

#ifndef DMI_MEMORY_ARRAY_ADDR_T
#   define DMI_MEMORY_ARRAY_ADDR_T
    typedef struct dmi_memory_array_addr dmi_memory_array_addr_t;
#endif // !DMI_MEMORY_ARRAY_ADDR_T

/**
 * @brief Memory array mapped address structure (type 19).
 *
 * Maps one contiguous range of physical addresses to a physical memory
 * array. There is one such structure for each range.
 *
 * @since SMBIOS 2.1
 */
struct dmi_memory_array_addr
{
    /**
     * @brief Physical address, in bytes, of a range of memory mapped to the
     * specified physical memory array.
     */
    uint64_t start_addr;

    /**
     * @brief Physical address, in bytes, of the last byte of a range of
     * memory mapped to the specified physical memory array. When the
     * structure gives the address in kilobytes, this is the last byte of the
     * last kilobyte of the range.
     */
    uint64_t end_addr;

    /**
     * @brief Mapped address range size in bytes, computed from the starting
     * and the ending addresses, both of which belong to the range.
     */
    uint64_t range_size;

    /**
     * @brief Handle, or instance number, associated with the physical memory
     * array to which this address range is mapped. Multiple address ranges can
     * be mapped to a single physical memory array.
     */
    dmi_handle_t array_handle;

    /**
     * @brief Reference to the physical memory array to which this address
     * range is mapped. Multiple address ranges can be mapped to a single
     * physical memory array.
     */
    dmi_entity_t *array;

    /**
     * @brief Number of memory devices that form a single row of memory for the
     * address partition defined by this structure.
     */
    unsigned short partition_width;
};

/**
 * @brief Memory array mapped address entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_memory_array_addr_spec;

#endif // !OPENDMI_ENTITY_MEMORY_ARRAY_ADDR_H
