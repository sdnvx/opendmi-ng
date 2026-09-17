//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_ERROR_32_H
#define OPENDMI_ENTITY_MEMORY_ERROR_32_H

#pragma once

#include <opendmi/entity/memory-error.h>

typedef struct dmi_memory_error_32 dmi_memory_error_32_t;

/**
 * @brief 32-Bit memory error information structure (type 18).
 * @since SMBIOS 2.1
 */
struct dmi_memory_error_32
{
    /**
     * @brief Type of error that is associated with the current status reported
     * for the memory array or device.
     */
    dmi_memory_error_type_t type;

    /**
     * @brief Granularity (for example, device versus partition) to which the
     * error can be resolved.
     */
    dmi_memory_error_granularity_t granularity;

    /**
     * @brief Memory access operation that caused the error.
     */
    dmi_memory_error_operation_t operation;

    /**
     * @brief Vendor-specific ECC syndrome or CRC data associated with the
     * erroneous access. If the value is unknown, this field contains 0.
     */
    uint32_t vendor_syndrome;

    /**
     * @brief Physical address of the error based on the addressing of
     * the bus to which the memory array is connected. If the address is
     * unknown, this field contains `0x80000000`.
     */
    uint32_t array_addr;

    /**
     * @brief Physical address of the error relative to the start of
     * the failing memory device, in bytes. If the address is unknown, this
     * field contains `0x80000000`.
     */
    uint32_t device_addr;

    /**
     * @brief Range, in bytes, within which the error can be determined, when
     * an error address is given. If the range is unknown, this field contains
     * `0`
     */
    uint32_t resolution;
};

/**
 * @brief 32-bit memory error information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_memory_error_32_spec;

#endif // !OPENDMI_ENTITY_MEMORY_ERROR_32_H
