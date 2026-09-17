//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_UTILS_H
#define OPENDMI_UTILS_H

#pragma once

#include <opendmi/types.h>

__BEGIN_DECLS

/**
 * @brief Allocates a zero-initialised block of memory.
 *
 * A thin wrapper around `calloc(1, size)`. On allocation failure and when
 * @p context is not @c nullptr, raises a `DMI_ERROR_OUT_OF_MEMORY` error on
 * @p context.
 *
 * @param context DMI context used for error reporting, or @c nullptr to suppress
 *                error reporting.
 * @param size    Number of bytes to allocate.
 * @return Pointer to the allocated block, or @c nullptr on failure.
 */
void *dmi_alloc(dmi_context_t *context, size_t size);

/**
 * @brief Frees a previously allocated block of memory.
 *
 * A null-safe wrapper around `free()`. Does nothing if @p ptr is @c nullptr.
 *
 * @param ptr Pointer to the block to free, or @c nullptr.
 */
void dmi_free(void *ptr);

/**
 * @brief Verifies an SMBIOS-style 8-bit checksum.
 *
 * Computes the sum of all bytes in @p data over @p length bytes. The checksum
 * is valid when the sum, taken modulo 256, equals zero.
 *
 * @param data   Pointer to the data block to verify.
 * @param length Number of bytes to sum.
 * @return @c true if the checksum is valid (byte sum equals zero), @c false
 *         otherwise. Returns @c false and sets `errno` to @c EINVAL if
 *         @p data is @c nullptr.
 */
bool dmi_checksum(const void *data, size_t length);

/**
 * @brief Raises a 32-bit unsigned integer to a non-negative integer power.
 *
 * Computes @p value raised to the power @p factor using binary exponentiation.
 * Returns @c 1 when @p factor is @c 0.
 *
 * @param value  Base value.
 * @param factor Exponent.
 * @return @p value raised to the power @p factor.
 */
uint32_t dmi_ipow32(uint32_t value, unsigned int factor);

/**
 * @brief Raises a 64-bit unsigned integer to a non-negative integer power.
 *
 * Computes @p value raised to the power @p factor using binary exponentiation.
 * Returns @c 1 when @p factor is @c 0.
 *
 * @param value  Base value.
 * @param factor Exponent.
 * @return @p value raised to the power @p factor.
 */
uint64_t dmi_ipow64(uint64_t value, unsigned int factor);

/**
 * @brief Reads the contents of a file into a newly allocated buffer.
 *
 * Opens the file at @p path, allocates a buffer of the requested size (or
 * sized to the file), reads the contents, and stores the number of bytes read
 * in @p plength. The caller is responsible for freeing the returned buffer
 * with `dmi_free`(3).
 *
 * On any failure an error is raised on @p context.
 *
 * @param context  DMI context.
 * @param path     Path to the file to read.
 * @param offset   Offset to read from, or a negative value to read from the
 *                 beginning of the file.
 * @param plength  On input, the maximum number of bytes to read, or zero to
 *                 read the whole file. On success, receives the number of
 *                 bytes read, which may be less than requested.
 * @return Pointer to a newly allocated buffer containing the file data, or
 *         @c nullptr on failure. @p plength is not modified on failure.
 */
dmi_data_t *dmi_file_get(
        dmi_context_t *context,
        const char    *path,
        off_t         offset,
        size_t       *plength);

#if !defined(_WIN32)
/**
 * @brief Reads a region of a device or file into a newly allocated buffer.
 *
 * Opens @p path, maps @p length bytes starting at physical offset @p base
 * into memory using `mmap`(2), copies the data into an allocated buffer, then
 * unmaps the region. Page alignment is handled internally.
 *
 * The caller is responsible for freeing the returned buffer with `dmi_free`(3).
 *
 * On any failure an error is raised on @p context.
 *
 * @note Not available on Windows.
 *
 * @param context DMI context.
 * @param path    Path to the device or file to read (e.g. `/dev/mem`).
 * @param base    Physical byte offset to start reading from.
 * @param length  Number of bytes to read; must be greater than zero.
 * @return Pointer to a newly allocated buffer containing the requested data,
 *         or @c nullptr on failure.
 */
    dmi_data_t *dmi_memory_get(dmi_context_t *context, const char *path, size_t base, size_t length);
#endif // !defined(_WIN32)

__END_DECLS

/**
 * @brief Allocates a zero-initialised array.
 *
 * Equivalent to `dmi_alloc(context, size * count)`. On allocation failure and
 * when @p context is not @c nullptr, raises a `DMI_ERROR_OUT_OF_MEMORY` error on
 * @p context.
 *
 * @param context DMI context used for error reporting, or @c nullptr to suppress
 *                error reporting.
 * @param size    Size of each element in bytes.
 * @param count   Number of elements.
 * @return Pointer to the allocated array, or @c nullptr on failure.
 */
static inline void *dmi_alloc_array(dmi_context_t *context, size_t size, size_t count)
{
    return dmi_alloc(context, size * count);
}

#endif // !OPENDMI_UTILS_H
