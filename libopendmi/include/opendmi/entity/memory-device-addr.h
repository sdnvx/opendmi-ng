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

typedef struct dmi_memory_device_addr dmi_memory_device_addr_t;

/**
 * @brief Memory device mapped address structure (type 20).
 * @since SMBIOS 2.1
 */
struct dmi_memory_device_addr
{
    /**
     * @brief Physical address, in bytes, of a range of memory mapped to the
     * referenced memory device.
     */
    uint64_t start_addr;

    /**
     * @brief Physical ending address, in bytes, of the last of a range of
     * addresses mapped to the referenced memory device.
     */
    uint64_t end_addr;

    /**
     * @brief Address range size in bytes.
     */
    uint64_t range_size;

    /**
     * @brief Handle, or instance number, associated with the memory device
     * structure to which this address range is mapped Multiple address ranges
     * can be mapped to a single memory device.
     */
    dmi_handle_t device_handle;

    /**
     * @brief Reference to he memory device structure to which this address
     * range is mapped Multiple address ranges can be mapped to a single memory
     * device.
     */
    dmi_entity_t *device;

    /**
     * @brief Handle, or instance number, associated with the memory array
     * mapped address structure to which this device address range is mapped.
     * Multiple address ranges can be mapped to a single memory array mapped
     * address.
     */
    dmi_handle_t array_addr_handle;

    /**
     * @brief Reference to the memory array mapped address structure to which
     * this device address range is mapped. Multiple address ranges can be
     * mapped to a single memory array mapped address.
     */
    dmi_entity_t *array_addr;

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
