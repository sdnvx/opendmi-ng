//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_ARRAY_H
#define OPENDMI_ENTITY_MEMORY_ARRAY_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/entity/common.h>

/**
 * @brief Memory array location values.
 */
typedef enum dmi_memory_array_location
{
    DMI_MEMORY_ARRAY_LOCATION_UNSPEC          = 0x00, ///< Unspecified
    DMI_MEMORY_ARRAY_LOCATION_OTHER           = 0x01, ///< Other
    DMI_MEMORY_ARRAY_LOCATION_UNKNOWN         = 0x02, ///< Unknown
    DMI_MEMORY_ARRAY_LOCATION_MOTHERBOARD     = 0x03, ///< System board or motherboard
    DMI_MEMORY_ARRAY_LOCATION_ISA             = 0x04, ///< ISA add-on card
    DMI_MEMORY_ARRAY_LOCATION_EISA            = 0x05, ///< EISA add-on card
    DMI_MEMORY_ARRAY_LOCATION_PCI             = 0x06, ///< PCI add-on card
    DMI_MEMORY_ARRAY_LOCATION_MCA             = 0x07, ///< MCA add-on card
    DMI_MEMORY_ARRAY_LOCATION_PCMCIA          = 0x08, ///< PCMCIA add-on card
    DMI_MEMORY_ARRAY_LOCATION_PROPRIETARY     = 0x09, ///< Proprietary add-on card
    DMI_MEMORY_ARRAY_LOCATION_NUBUS           = 0x0A, ///< NuBus
    __DMI_MEMORY_ARRAY_LOCATION_UNASSIGNED_START = 0x0B,
    // Unassigned: 0x0B .. 0x9F
    __DMI_MEMORY_ARRAY_LOCATION_UNASSIGNED_END = 0x9F,
    DMI_MEMORY_ARRAY_LOCATION_PC_98_C20       = 0xA0, ///< PC-98/C20 add-on card
    DMI_MEMORY_ARRAY_LOCATION_PC_98_C24       = 0xA1, ///< PC-98/C24 add-on card
    DMI_MEMORY_ARRAY_LOCATION_PC_98_E         = 0xA2, ///< PC-98/E add-on card
    DMI_MEMORY_ARRAY_LOCATION_PC_98_LOCAL_BUS = 0xA3, ///< PC-98/Local bus add-on card
    DMI_MEMORY_ARRAY_LOCATION_CXL             = 0xA4, ///< CXL add-on card
    __DMI_MEMORY_ARRAY_LOCATION_COUNT
} dmi_memory_array_location_t;

/**
 * @brief Memory array usage values.
 */
typedef enum dmi_memory_array_usage
{
    DMI_MEMORY_ARRAY_USAGE_UNSPEC  = 0x00, ///< Unspecified
    DMI_MEMORY_ARRAY_USAGE_OTHER   = 0x01, ///< Other
    DMI_MEMORY_ARRAY_USAGE_UNKNOWN = 0x02, ///< Unknown
    DMI_MEMORY_ARRAY_USAGE_SYSTEM  = 0x03, ///< System memory
    DMI_MEMORY_ARRAY_USAGE_VIDEO   = 0x04, ///< Video memory
    DMI_MEMORY_ARRAY_USAGE_FLASH   = 0x05, ///< Flash memory
    DMI_MEMORY_ARRAY_USAGE_NVRAM   = 0x06, ///< Non-volatile RAM
    DMI_MEMORY_ARRAY_USAGE_CACHE   = 0x07, ///< Cache memory
    __DMI_MEMORY_ARRAY_USAGE_COUNT
} dmi_memory_array_usage_t;

/**
 * @brief Physical memory array entity.
 */
struct dmi_memory_array
{
    /**
     * @brief Physical location of the memory array, whether on the system
     * board or an add-in board.
     */
    dmi_memory_array_location_t location;

    /**
     * @brief Function for which the array is used.
     */
    dmi_memory_array_usage_t usage;

    /**
     * @brief Primary hardware error correction or detection method supported
     * by this memory array.
     */
    dmi_error_correct_type_t error_correction;

    /**
     * @brief Maximum memory capacity, in bytes, for this array. The value
     * `DMI_SIZE_MAX` means that the capacity is unknown.
     */
    dmi_size_t maximum_capacity;

    /**
     * @brief Handle, or instance number, associated with any error that was
     * previously detected for the array.
     *
     * If the system does not provide the error information structure, the
     * field contains 0xFFFE. Otherwise, the field contains either 0xFFFF (if
     * no error was detected) or the handle of the error-information structure.
     */
    dmi_handle_t error_info_handle;

    /**
     * @brief An error that was previously detected for the array.
     */
    dmi_entity_t *error_info;

    /**
     * @brief @brief Number of slots or sockets available for memory devices in this
     * array.
     *
     * This value represents the number of memory device structures that
     * compose this memory array. Each memory device has a reference to the
     * "owning" memory array.
     */
    unsigned int device_count;
};

typedef struct dmi_memory_array dmi_memory_array_t;

/**
 * @brief Physical memory array entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_memory_array_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_memory_array_location_name(dmi_memory_array_location_t value);
__dmi_api const char *dmi_memory_array_usage_name(dmi_memory_array_usage_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_MEMORY_ARRAY_H
