//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_UTILS_ENDIAN_H
#define OPENDMI_UTILS_ENDIAN_H

#pragma once

#include <opendmi/types.h>

/**
 * @brief Byte-swap an 8-bit value (no-op).
 *
 * @param[in] value The 8-bit value to swap (no effect).
 * @return The unchanged input value.
 */
static inline uint8_t dmi_bswap8(uint8_t value) { return value; }

__BEGIN_DECLS

/**
 * @internal
 * @brief Byte-swap a 16-bit value using software implementation.
 *
 * @param[in] value The 16-bit value to swap.
 * @return The byte-swapped value.
 */
__dmi_const
__dmi_api uint16_t __dmi_bswap16_compat(uint16_t value);

/**
 * @internal
 * @brief Byte-swap a 32-bit value using software implementation.
 *
 * @param[in] value The 32-bit value to swap.
 * @return The byte-swapped value.
 */
__dmi_const
__dmi_api uint32_t __dmi_bswap32_compat(uint32_t value);

/**
 * @internal
 * @brief Byte-swap a 64-bit value using software implementation.
 *
 * @param[in] value The 64-bit value to swap.
 * @return The byte-swapped value.
 */
__dmi_const
__dmi_api uint64_t __dmi_bswap64_compat(uint64_t value);

__END_DECLS

/**
 * @brief Byte-swap a 16-bit value (inline)
 *
 * This function reverses the byte order of a 16-bit value. It uses compiler
 * builtins when available for better performance, otherwise falls back to
 * a software implementation.
 *
 * @param[in] value The 16-bit value to swap.
 *
 * @return The byte-swapped value.
 */
__dmi_const
static inline uint16_t dmi_bswap16(uint16_t value)
{
#   if defined(_MSC_VER)
        return _byteswap_ushort(value);
#   elif defined(__GNUC__) || defined(__clang__)
        return __builtin_bswap16(value);
#   else
        return __dmi_bswap16_compat(value);
#   endif
}

/**
 * @brief Byte-swap a 32-bit value (inline).
 *
 * This function reverses the byte order of a 32-bit value. It uses compiler
 * builtins when available for better performance, otherwise falls back to
 * a software implementation.
 *
 * @param[in] value The 32-bit value to swap.
 *
 * @return The byte-swapped value.
 */
__dmi_const
static inline uint32_t dmi_bswap32(uint32_t value)
{
#   if defined(_MSC_VER)
        return _byteswap_ulong(value);
#   elif defined(__GNUC__) || defined(__clang__)
        return __builtin_bswap32(value);
#   else
        return __dmi_bswap32_compat(value);
#   endif
}

/**
 * @brief Byte-swap a 64-bit value (inline).
 *
 * This function reverses the byte order of a 64-bit value. It uses compiler
 * builtins when available for better performance, otherwise falls back to
 * a software implementation.
 *
 * @param[in] value The 64-bit value to swap.
 *
 * @return The byte-swapped value.
 */
__dmi_const
static inline uint64_t dmi_bswap64(uint64_t value)
{
#   if defined(_MSC_VER)
        return _byteswap_uint64(value);
#   elif defined(__GNUC__) || defined(__clang__)
        return __builtin_bswap64(value);
#   else
        return __dmi_bswap64_compat(value);
#   endif
}

/**
 * @brief Generic byte-swap function for various integer sizes.
 *
 * This macro provides a generic way to swap bytes of values based on their
 * type. Supported types: `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`.
 *
 * @param[in] value The value to byte-swap (type must be one of supported
 *                  types).
 *
 * @return The byte-swapped value.
 */
#define dmi_bswap(value) (_Generic((value),               \
                                   uint64_t: dmi_bswap64, \
                                   uint32_t: dmi_bswap32, \
                                   uint16_t: dmi_bswap16, \
                                   uint8_t:  dmi_bswap8   \
                                  )(value))

/**
 * @fn uint16_t dmi_hton16(uint16_t value)
 * @brief Convert unsigned 16-bit value from host to network byte order
 *
 * This function converts a 16-bit integer from host byte order to network
 * (big-endian) byte order. On big-endian systems, this is a no-op.
 *
 * @param[in] value The 16-bit value in host byte order.
 *
 * @return The value in network byte order.
 */
__dmi_const
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    static inline uint16_t dmi_hton16(uint16_t value) { return dmi_bswap16(value); }
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    static inline uint16_t dmi_hton16(uint16_t value) { return value; }
#endif

/**
 * @fn uint32_t dmi_hton32(uint32_t value)
 * @brief Convert unsigned 32-bit value from host to network byte order
 *
 * This function converts a 32-bit integer from host byte order to network
 * (big-endian) byte order. On big-endian systems, this is a no-op.
 *
 * @param[in] value The 32-bit value in host byte order.
 *
 * @return The value in network byte order.
 */
__dmi_const
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    static inline uint32_t dmi_hton32(uint32_t value) { return dmi_bswap32(value); }
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    static inline uint32_t dmi_hton32(uint32_t value) { return value; }
#endif

/**
 * @fn uint64_t dmi_hton64(uint64_t value)
 * @brief Convert unsigned 64-bit value from host to network byte order
 *
 * This function converts a 64-bit integer from host byte order to network
 * (big-endian) byte order. On big-endian systems, this is a no-op.
 *
 * @param[in] value The 64-bit value in host byte order.
 *
 * @return The value in network byte order.
 */
__dmi_const
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    static inline uint64_t dmi_hton64(uint64_t value) { return dmi_bswap64(value); }
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    static inline uint64_t dmi_hton64(uint64_t value) { return value; }
#endif

/**
 * @fn uint16_t dmi_ntoh16(uint16_t value)
 * @brief Convert unsigned 16-bit value from network to host byte order.
 *
 * This function converts a 16-bit integer from network (big-endian) byte order
 * to host byte order. On big-endian systems, this is a no-op.
 *
 * @param[in] value The 16-bit value in network byte order.
 *
 * @return The value in host byte order.
 */
__dmi_const
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    static inline uint16_t dmi_ntoh16(uint16_t value) { return dmi_bswap16(value); }
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    static inline uint16_t dmi_ntoh16(uint16_t value) { return value; }
#endif

/**
 * @fn uint32_t dmi_ntoh32(uint32_t value)
 * @brief Convert unsigned 32-bit value from network to host byte order.
 *
 * This function converts a 32-bit integer from network (big-endian) byte order
 * to host byte order. On big-endian systems, this is a no-op.
 *
 * @param[in] value The 32-bit value in network byte order.
 *
 * @return The value in host byte order.
 */
__dmi_const
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    static inline uint32_t dmi_ntoh32(uint32_t value) { return dmi_bswap32(value); }
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    static inline uint32_t dmi_ntoh32(uint32_t value) { return value; }
#endif

/**
 * @fn uint64_t dmi_ntoh64(uint64_t value)
 * @brief Convert unsigned 64-bit value from network to host byte order.
 *
 * This function converts a 64-bit integer from network (big-endian) byte order
 * to host byte order. On big-endian systems, this is a no-op.
 *
 * @param[in] value The 64-bit value in network byte order.
 *
 * @return The value in host byte order.
 */
__dmi_const
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    static inline uint64_t dmi_ntoh64(uint64_t value) { return dmi_bswap64(value); }
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    static inline uint64_t dmi_ntoh64(uint64_t value) { return value; }
#endif

/**
 * @brief Generic host-to-network byte order conversion
 *
 * This macro provides a generic way to convert values from host to network
 * byte order based on their type. Supported types: `uint16_t`, `uint32_t`,
 * `uint64_t`.
 *
 * @param[in] value The value in host byte order (type must be one of
 *                  supported types).
 *
 * @return The value in network byte order.
 */
#define dmi_hton(value) (_Generic((value),              \
                                  uint64_t: dmi_hton64, \
                                  uint32_t: dmi_hton32, \
                                  uint16_t: dmi_hton16  \
                                 )(value))

/**
 * @brief Generic network-to-host byte order conversion.
 *
 * This macro provides a generic way to convert values from network to host
 * byte order based on their type. Supported types: `uint16_t`, `uint32_t`,
 * `uint64_t`.
 *
 * @param[in] value The value in network byte order (type must be one of
 *                  supported types).
 *
 * @return The value in host byte order.
 */
#define dmi_ntoh(value) (_Generic((value),              \
                                  uint64_t: dmi_ntoh64, \
                                  uint32_t: dmi_ntoh32, \
                                  uint16_t: dmi_ntoh16  \
                                 )(value))

#endif // !OPENDMI_UTILS_ENDIAN_H
