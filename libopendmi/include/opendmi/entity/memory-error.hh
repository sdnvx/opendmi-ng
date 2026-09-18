//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_MEMORY_ERROR_HH
#define OPENDMI_MEMORY_ERROR_HH

#pragma once

#include <type_traits>

namespace dmi {
    namespace capi {
#       include <opendmi/entity/memory-error.h>
    }

    using memory_error_type_t        = capi::dmi_memory_error_type_t;
    using memory_error_granularity_t = capi::dmi_memory_error_granularity_t;
    using memory_error_operation_t   = capi::dmi_memory_error_operation_t;

    enum class memory_error_type : std::underlying_type_t<memory_error_type_t> {
        unspecified          = capi::DMI_MEMORY_ERROR_TYPE_UNSPEC,               ///< Unspecified
        other                = capi::DMI_MEMORY_ERROR_TYPE_OTHER,                ///< Other
        unknown              = capi::DMI_MEMORY_ERROR_TYPE_UNKNOWN,              ///< Unknown
        ok                   = capi::DMI_MEMORY_ERROR_TYPE_OK,                   ///< OK
        bad_read             = capi::DMI_MEMORY_ERROR_TYPE_BAD_READ,             ///< Bad read
        parity               = capi::DMI_MEMORY_ERROR_TYPE_PARITY,               ///< Parity error
        single_bit           = capi::DMI_MEMORY_ERROR_TYPE_SINGLE_BIT,           ///< Single-bit error
        double_bit           = capi::DMI_MEMORY_ERROR_TYPE_DOUBLE_BIT,           ///< Double-bit error
        multi_bit            = capi::DMI_MEMORY_ERROR_TYPE_MULTI_BIT,            ///< Multi-bit error
        nibble               = capi::DMI_MEMORY_ERROR_TYPE_NIBBLE,               ///< Nibble error
        checksum             = capi::DMI_MEMORY_ERROR_TYPE_CHECKSUM,             ///< Checksum error
        crc                  = capi::DMI_MEMORY_ERROR_TYPE_CRC,                  ///< CRC error
        corrected_single_bit = capi::DMI_MEMORY_ERROR_TYPE_CORRECTED_SINGLE_BIT, ///< Corrected single-bit error
        corrected            = capi::DMI_MEMORY_ERROR_TYPE_CORRECTED,            ///< Corrected error
        uncorrectable        = capi::DMI_MEMORY_ERROR_TYPE_UNCORRECTABLE,        ///< Uncorrectable error
    };

    enum class memory_error_granularity : std::underlying_type_t<memory_error_granularity_t> {
        unspecified = capi::DMI_MEMORY_ERROR_GRANULARITY_UNSPEC,    ///< Unspecified
        other       = capi::DMI_MEMORY_ERROR_GRANULARITY_OTHER,     ///< Other
        unknown     = capi::DMI_MEMORY_ERROR_GRANULARITY_UNKNOWN,   ///< Unknown
        device      = capi::DMI_MEMORY_ERROR_GRANULARITY_DEVICE,    ///< Device level
        partition   = capi::DMI_MEMORY_ERROR_GRANULARITY_PARTITION, ///< Memory partition level
    };

    enum class memory_error_operation : std::underlying_type_t<memory_error_operation_t> {
        unspecified   = capi::DMI_MEMORY_ERROR_OPERATION_UNSPEC,        ///< Unspecified
        other         = capi::DMI_MEMORY_ERROR_OPERATION_OTHER,         ///< Other
        unknown       = capi::DMI_MEMORY_ERROR_OPERATION_UNKNOWN,       ///< Unknown
        read          = capi::DMI_MEMORY_ERROR_OPERATION_READ,          ///< Read
        write         = capi::DMI_MEMORY_ERROR_OPERATION_WRITE,         ///< Write
        partial_write = capi::DMI_MEMORY_ERROR_OPERATION_PARTIAL_WRITE, ///< Partial write
    };
}

#endif // !OPENDMI_MEMORY_ERROR_HH
