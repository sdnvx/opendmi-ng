//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_UTILS_UUID_H
#define OPENDMI_UTILS_UUID_H

#pragma once

#include <opendmi/types.h>

/**
 * @brief UUID value.
 */
typedef union dmi_uuid dmi_uuid_t;

/**
 * @brief UUID type.
 */
dmi_packed_union(dmi_uuid)
{
    /**
     * @brief Raw value.
     */
    dmi_byte_t __value[16];

    dmi_packed_struct()
    {
        /**
         * @brief Low field of the timestamp.
         */
        uint32_t time_low;

        /**
         * @brief Middle field of the timestamp.
         */
        uint16_t time_mid;

        /**
         * @brief High field of the timestamp multiplexed with the version number.
         */
        uint16_t time_hi_and_version;

        /**
         * @brief High field of the clock sequence multiplexed with the variant.
         */
        uint8_t clock_seq_hi_and_reserved;

        /**
         * @brief Low field of the clock sequence.
         */
        uint8_t clock_seq_low;

        /**
         * @brief Spatially unique node identifier.
         */
        uint8_t node[6];
    };
};

dmi_static_assert_value_union(dmi_uuid);

__BEGIN_DECLS

/**
 * @brief Decodes a UUID from SMBIOS byte order into RFC 4122 representation.
 *
 * SMBIOS stores the first three UUID fields (`time_low`, `time_mid`,
 * and `time_hi_and_version`) in little-endian byte order. RFC 4122 requires
 * all multi-byte fields to be in big-endian (network) byte order. This
 * function copies the raw SMBIOS value and byte-swaps those three fields so
 * that the returned `dmi_uuid_t` conforms to RFC 4122. The
 * `clock_seq_hi_and_reserved`, `clock_seq_low`, and `node` fields are stored
 * identically in both formats and are not modified.
 *
 * @param[in] value  Raw 16-byte UUID as stored in an SMBIOS structure.
 *
 * @return A `dmi_uuid_t` with fields in RFC 4122 (big-endian) byte order.
 */
__dmi_api dmi_uuid_t dmi_uuid_decode(const dmi_byte_t value[16]);

/**
 * @brief Encodes a UUID from RFC 4122 representation into SMBIOS byte order.
 *
 * Performs the inverse of `dmi_uuid_decode()`: byte-swaps the `time_low`,
 * `time_mid`, and `time_hi_and_version` fields from big-endian (RFC 4122) back
 * to little-endian (SMBIOS) byte order, then writes the resulting 16 bytes
 * into @p out. The `clock_seq_hi_and_reserved`, `clock_seq_low`, and `node`
 * fields are copied without modification.
 *
 * @param[in]  value  UUID in RFC 4122 (big-endian) byte order.
 * @param[out] out    Buffer of exactly 16 bytes that receives the UUID in
 *                    SMBIOS (little-endian) byte order.
 */
__dmi_api void dmi_uuid_encode(dmi_uuid_t value, uint8_t out[16]);

__END_DECLS

#endif // !OPENDMI_UTILS_UUID_H
