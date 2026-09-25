//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_UTILS_CODEC_H
#define OPENDMI_UTILS_CODEC_H

#pragma once

#include <opendmi/utils/endian.h>

#if (__BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__) && (__BYTE_ORDER__ != __ORDER_BIG_ENDIAN__)
#   error "Unsupported endianness"
#endif

__BEGIN_DECLS

/**
 * @internal
 * @brief Decode a BCD-encoded value from a raw byte buffer.
 *
 * Interprets each byte as two BCD digits (low nibble first) and converts the
 * sequence into an unsigned integer. Bytes are processed from lowest to
 * highest address.
 *
 * @param[in] value  Pointer to the BCD-encoded byte buffer.
 * @param[in] length Number of bytes in the buffer.
 *
 * @return Decoded unsigned integer value.
 */
__dmi_api uintmax_t __dmi_decode_bcd(const dmi_byte_t *value, size_t length);

/**
 * @internal
 * @brief Encode an unsigned integer into a BCD-encoded byte buffer.
 *
 * Performs the inverse of `__dmi_decode_bcd()`: spells the number out as two
 * decimal digits per byte, low nibble first, from the lowest address up. The
 * digits which do not fit into @p length bytes are dropped.
 *
 * @param[in]  value  Unsigned integer value.
 * @param[out] data   Buffer that receives the BCD-encoded bytes.
 * @param[in]  length Number of bytes in the buffer.
 */
__dmi_api void __dmi_encode_bcd(uintmax_t value, dmi_byte_t *data, size_t length);

__END_DECLS

/**
 * @brief Decode a single byte from SMBIOS wire format.
 *
 * @param[in] value Raw SMBIOS byte value.
 *
 * @return Host byte order value.
 */
__dmi_const
static inline uint8_t dmi_decode_byte(dmi_byte_t value) { return value; }

/**
 * @brief Encode a single byte into SMBIOS wire format.
 *
 * @param[in] value Host byte order value.
 *
 * @return SMBIOS wire format byte value.
 */
__dmi_const
static inline dmi_byte_t dmi_encode_byte(uint8_t value) { return value; }

/**
 * @fn dmi_word_t dmi_encode_word(uint16_t value)
 * @brief Encode a 16-bit value into SMBIOS wire format.
 *
 * Converts a host byte order 16-bit value into the little-endian byte order
 * used by SMBIOS structures.
 *
 * @param[in] value Host byte order 16-bit value.
 *
 * @return SMBIOS wire format (little-endian) 16-bit value.
 */
__dmi_const
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    static inline dmi_word_t dmi_encode_word(uint16_t value) { return value; }
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    static inline dmi_word_t dmi_encode_word(uint16_t value) { return dmi_bswap16(value); }
#endif

/**
 * @fn dmi_dword_t dmi_encode_dword(uint32_t value)
 * @brief Encode a 32-bit value into SMBIOS wire format.
 *
 * Converts a host byte order 32-bit value into the little-endian byte order
 * used by SMBIOS structures.
 *
 * @param[in] value Host byte order 32-bit value.
 *
 * @return SMBIOS wire format (little-endian) 32-bit value.
 */
__dmi_const
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    static inline dmi_dword_t dmi_encode_dword(uint32_t value) { return value; }
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    static inline dmi_dword_t dmi_encode_dword(uint32_t value) { return dmi_bswap32(value); }
#endif

/**
 * @fn dmi_qword_t dmi_encode_qword(uint64_t value)
 * @brief Encode a 64-bit value into SMBIOS wire format.
 *
 * Converts a host byte order 64-bit value into the little-endian byte order
 * used by SMBIOS structures.
 *
 * @param[in] value Host byte order 64-bit value.
 *
 * @return SMBIOS wire format (little-endian) 64-bit value.
 */
__dmi_const
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    static inline dmi_qword_t dmi_encode_qword(uint64_t value) { return value; }
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    static inline dmi_qword_t dmi_encode_qword(uint64_t value) { return dmi_bswap64(value); }
#endif

/**
 * @fn uint16_t dmi_decode_word(dmi_word_t value)
 * @brief Decode a 16-bit value from SMBIOS wire format.
 *
 * Converts a little-endian 16-bit value from an SMBIOS structure into host
 * byte order.
 *
 * @param[in] value SMBIOS wire format (little-endian) 16-bit value.
 *
 * @return Host byte order 16-bit value.
 */
__dmi_const
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    static inline uint16_t dmi_decode_word(dmi_word_t value) { return value; }
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    static inline uint16_t dmi_decode_word(dmi_word_t value) { return dmi_bswap16(value); }
#endif

/**
 * @fn uint32_t dmi_decode_dword(dmi_dword_t value)
 * @brief Decode a 32-bit value from SMBIOS wire format.
 *
 * Converts a little-endian 32-bit value from an SMBIOS structure into host
 * byte order.
 *
 * @param[in] value SMBIOS wire format (little-endian) 32-bit value.
 *
 * @return Host byte order 32-bit value.
 */
__dmi_const
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    static inline uint32_t dmi_decode_dword(dmi_dword_t value) { return value; }
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    static inline uint32_t dmi_decode_dword(dmi_dword_t value) { return dmi_bswap32(value); }
#endif

/**
 * @fn uint64_t dmi_decode_qword(dmi_qword_t value)
 * @brief Decode a 64-bit value from SMBIOS wire format.
 *
 * Converts a little-endian 64-bit value from an SMBIOS structure into host
 * byte order.
 *
 * @param[in] value SMBIOS wire format (little-endian) 64-bit value.
 *
 * @return Host byte order 64-bit value.
 */
__dmi_const
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    static inline uint64_t dmi_decode_qword(dmi_qword_t value) { return value; }
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    static inline uint64_t dmi_decode_qword(dmi_qword_t value) { return dmi_bswap64(value); }
#endif

/**
 * @brief Decode a value from SMBIOS wire format into host byte order.
 *
 * Type-generic macro that selects the appropriate decoding function based on
 * the type of @p value: `dmi_decode_qword` for `dmi_qword_t`,
 * `dmi_decode_dword` for `dmi_dword_t`, `dmi_decode_word` for `dmi_word_t`,
 * or `dmi_decode_byte` for `dmi_byte_t`.
 *
 * @param[in] value SMBIOS wire format value.
 *
 * @return Host byte order value of the same width.
 */
#define dmi_decode(value) (_Generic((value),                       \
                                    dmi_qword_t: dmi_decode_qword, \
                                    dmi_dword_t: dmi_decode_dword, \
                                    dmi_word_t:  dmi_decode_word,  \
                                    dmi_byte_t:  dmi_decode_byte   \
                                   )(value))

/**
 * @brief Encode a value from host byte order into SMBIOS wire format.
 *
 * Type-generic macro that selects the appropriate encoding function based on
 * the type of @p value: `dmi_encode_qword` for `uint64_t`,
 * `dmi_encode_dword` for `uint32_t`, `dmi_encode_word` for `uint16_t`,
 * or `dmi_encode_byte` for `uint8_t`.
 *
 * @param[in] value Host byte order value.
 *
 * @return SMBIOS wire format (little-endian) value of the same width.
 */
#define dmi_encode(value) (_Generic((value),                    \
                                    uint64_t: dmi_encode_qword, \
                                    uint32_t: dmi_encode_dword, \
                                    uint16_t: dmi_encode_word,  \
                                    uint8_t:  dmi_encode_byte   \
                                   )(value))

/**
 * @brief Decode a BCD-encoded value.
 *
 * Convenience macro that passes the address and size of @p value to
 * `__dmi_decode_bcd`. Accepts any integer type (`uint8_t`, `uint16_t`,
 * `uint32_t`, `uint64_t`).
 *
 * @param[in] value BCD-encoded integer value.
 *
 * @return Decoded unsigned integer value.
 */
#define dmi_decode_bcd(value) \
        __dmi_decode_bcd((const dmi_byte_t *)&(value), sizeof(value))

/**
 * @brief Encode a value as a binary-coded decimal.
 *
 * Convenience macro that spells @p value out as the decimal digits a value of
 * the type it is given fits, two per byte, using `__dmi_encode_bcd`. Accepts
 * any integer type (`uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`), and the
 * digits which do not fit into it are dropped.
 *
 * @param[in] value Unsigned integer value.
 *
 * @return BCD-encoded value of the same width.
 */
#define dmi_encode_bcd(value)                                              \
        ({                                                                 \
            __dmi_typeof(value) __encoded;                                 \
            __dmi_encode_bcd(value, (dmi_byte_t *)&__encoded, sizeof(__encoded)); \
            __encoded;                                                     \
        })

#endif // !OPEDMI_UTILS_CODEC_H
