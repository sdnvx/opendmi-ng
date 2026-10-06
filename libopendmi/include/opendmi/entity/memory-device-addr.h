//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_DEVICE_ADDR_H
#define OPENDMI_ENTITY_MEMORY_DEVICE_ADDR_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_MEMORY_DEVICE_ADDR_T
#   define DMI_MEMORY_DEVICE_ADDR_T
    typedef struct dmi_memory_device_addr dmi_memory_device_addr_t;
#endif // !DMI_MEMORY_DEVICE_ADDR_T

/**
 * @brief Memory device mapped address structure (type 20).
 *
 * Maps a contiguous range of the physical address space to a memory device,
 * as a part of the range of a memory array mapped address structure. There
 * is one structure for each range, and only for the devices which have an
 * address mapped.
 *
 * @since SMBIOS 2.1
 */
struct dmi_memory_device_addr
{
    /**
     * @brief Physical address, in bytes, of a range of memory mapped to the
     * referenced memory device.
     */
    uint64_t start_address;

    /**
     * @brief Physical address, in bytes, of the last byte of a range of
     * memory mapped to the referenced memory device.
     *
     * When taken from the SMBIOS 2.1 field, which counts in kibibytes, this
     * is the last byte of the last kibibyte of the range.
     */
    uint64_t end_address;

    /**
     * @brief Address range size in bytes, computed from the starting and the
     * ending addresses, both of which belong to the range.
     */
    uint64_t range_size;

    /**
     * @brief Handle, or instance number, associated with the memory device
     * structure to which this address range is mapped. Multiple address
     * ranges can be mapped to a single memory device.
     */
    dmi_handle_t device_handle;

    /**
     * @brief Reference to the memory device structure to which this address
     * range is mapped, @c nullptr if the handle does not refer to one.
     * Multiple address ranges can be mapped to a single memory device.
     */
    dmi_entity_t *device;

    /**
     * @brief Handle, or instance number, associated with the memory array
     * mapped address structure to which this device address range is mapped.
     * Multiple address ranges can be mapped to a single memory array mapped
     * address.
     */
    dmi_handle_t array_address_handle;

    /**
     * @brief Reference to the memory array mapped address structure to which
     * this device address range is mapped, @c nullptr if the handle does not
     * refer to one. Multiple address ranges can be mapped to a single memory
     * array mapped address.
     */
    dmi_entity_t *array_address;

    /**
     * @brief Position of the referenced memory device in a row of the address
     * partition. For example, if two 8-bit devices form a 16-bit row, this
     * field's value is either 1 or 2. The value 0 is reserved. If the position
     * is unknown, the field contains `USHRT_MAX`.
     */
    unsigned short partition_pos;

    /**
     * @brief Position of the referenced memory device in an interleave. The
     * value 0 indicates non-interleaved, 1 indicates first interleave
     * position, 2 the second interleave position, and so on. If the position
     * is unknown, the field contains `USHRT_MAX`.
     *
     * Examples: In a 2:1 interleave, the value 1 indicates the device in the
     * "even" position. In a 4:1 interleave, the value 1 indicates the first
     * of four possible positions.
     */
    unsigned short interleave_pos;

    /**
     * @brief Maximum number of consecutive rows from the referenced memory
     * device that are accessed in a single interleaved transfer. If the device
     * is not part of an interleave, the field contains 0; if the interleave
     * configuration is unknown, the value is `USHRT_MAX`.
     */
    unsigned short interleave_depth;
};

/**
 * @brief Memory device mapped address entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_memory_device_addr_spec;

#endif // !OPENDMI_ENTITY_MEMORY_DEVICE_ADDR_H
