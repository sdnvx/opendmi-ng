//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_ERROR_H
#define OPENDMI_ENTITY_MEMORY_ERROR_H

#pragma once

#include <opendmi/utils/name.h>

/**
 * @brief Memory error types.
 */
typedef enum dmi_memory_error_type
{
    DMI_MEMORY_ERROR_TYPE_UNSPEC               = 0x00, ///< Unspecified
    DMI_MEMORY_ERROR_TYPE_OTHER                = 0x01, ///< Other
    DMI_MEMORY_ERROR_TYPE_UNKNOWN              = 0x02, ///< Unknown
    DMI_MEMORY_ERROR_TYPE_OK                   = 0x03, ///< OK
    DMI_MEMORY_ERROR_TYPE_BAD_READ             = 0x04, ///< Bad read
    DMI_MEMORY_ERROR_TYPE_PARITY               = 0x05, ///< Parity error
    DMI_MEMORY_ERROR_TYPE_SINGLE_BIT           = 0x06, ///< Single-bit error
    DMI_MEMORY_ERROR_TYPE_DOUBLE_BIT           = 0x07, ///< Double-bit error
    DMI_MEMORY_ERROR_TYPE_MULTI_BIT            = 0x08, ///< Multi-bit error
    DMI_MEMORY_ERROR_TYPE_NIBBLE               = 0x09, ///< Nibble error
    DMI_MEMORY_ERROR_TYPE_CHECKSUM             = 0x0A, ///< Checksum error
    DMI_MEMORY_ERROR_TYPE_CRC                  = 0x0B, ///< CRC error
    DMI_MEMORY_ERROR_TYPE_CORRECTED_SINGLE_BIT = 0x0C, ///< Corrected single-bit error
    DMI_MEMORY_ERROR_TYPE_CORRECTED            = 0x0D, ///< Corrected error
    DMI_MEMORY_ERROR_TYPE_UNCORRECTABLE        = 0x0E, ///< Uncorrectable error
    __DMI_MEMORY_ERROR_TYPE_COUNT
} dmi_memory_error_type_t;

/**
 * @brief Memory error granularities.
 */
typedef enum dmi_memory_error_granularity
{
    DMI_MEMORY_ERROR_GRANULARITY_UNSPEC    = 0x00, ///< Unspecified
    DMI_MEMORY_ERROR_GRANULARITY_OTHER     = 0x01, ///< Other
    DMI_MEMORY_ERROR_GRANULARITY_UNKNOWN   = 0x02, ///< Unknown
    DMI_MEMORY_ERROR_GRANULARITY_DEVICE    = 0x03, ///< Device level
    DMI_MEMORY_ERROR_GRANULARITY_PARTITION = 0x04, ///< Memory partition level
    __DMI_MEMORY_ERROR_GRANULARITY_COUNT
} dmi_memory_error_granularity_t;

/**
 * @brief Memory error operations.
 */
typedef enum dmi_memory_error_operation
{
    DMI_MEMORY_ERROR_OPERATION_UNSPEC        = 0x00, ///< Unspecified
    DMI_MEMORY_ERROR_OPERATION_OTHER         = 0x01, ///< Other
    DMI_MEMORY_ERROR_OPERATION_UNKNOWN       = 0x02, ///< Unknown
    DMI_MEMORY_ERROR_OPERATION_READ          = 0x03, ///< Read
    DMI_MEMORY_ERROR_OPERATION_WRITE         = 0x04, ///< Write
    DMI_MEMORY_ERROR_OPERATION_PARTIAL_WRITE = 0x05, ///< Partial write
    __DMI_MEMORY_ERROR_OPERATION_COUNT
} dmi_memory_error_operation_t;

extern const dmi_name_set_t dmi_memory_error_type_names;
extern const dmi_name_set_t dmi_memory_error_granularity_names;
extern const dmi_name_set_t dmi_memory_error_operation_names;

__BEGIN_DECLS

/**
 * @brief Get memory error type name.
 *
 * Returns the human-readable name of the memory error type.
 *
 * @param[in] value Memory error type value.
 *
 * @return The memory error type name string, or @c nullptr if @p value is out of
 * range.
 */
const char *dmi_memory_error_type_name(dmi_memory_error_type_t value);

/**
 * @brief Get memory error granularity name.
 *
 * Returns the human-readable name of the memory error granularity.
 *
 * @param[in] value Memory error granularity value.
 *
 * @return The memory error granularity name string, or @c nullptr if @p value is
 * out of range.
 */
const char *dmi_memory_error_granularity_name(dmi_memory_error_granularity_t value);

/**
 * @brief Get memory error operation name.
 *
 * Returns the human-readable name of the memory error operation.
 *
 * @param[in] value Memory error operation value.
 *
 * @return The memory error operation name string, or @c nullptr if @p value is out
 * of range.
 */
const char *dmi_memory_error_operation_name(dmi_memory_error_operation_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_MEMORY_ERROR_H
