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
#include <opendmi/buffer.h>

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
__dmi_api void *dmi_alloc(dmi_context_t *context, size_t size);

/**
 * @brief Frees a previously allocated block of memory.
 *
 * A null-safe wrapper around `free()`. Does nothing if @p ptr is @c nullptr.
 *
 * @param ptr Pointer to the block to free, or @c nullptr.
 */
__dmi_api void dmi_free(void *ptr);

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
__dmi_api bool dmi_checksum_test(const void *data, size_t length);

/**
 * @brief Computes an SMBIOS-style 8-bit checksum.
 *
 * Computes the value which, when added to all bytes in @p data over
 * @p length bytes, results in zero sum modulo 256. The checksum field within
 * @p data must be set to zero before the computation.
 *
 * @param data   Pointer to the data block to compute checksum of.
 * @param length Number of bytes to sum.
 * @return Checksum value. Returns @c 0 and sets `errno` to @c EINVAL if
 *         @p data is @c nullptr.
 */
__dmi_api uint8_t dmi_checksum_calc(const void *data, size_t length);

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
__dmi_api uint32_t dmi_ipow32(uint32_t value, unsigned int factor);

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
__dmi_api uint64_t dmi_ipow64(uint64_t value, unsigned int factor);

/**
 * @brief Reads the contents of a file into a buffer.
 *
 * Opens the file at @p path and reads its contents into @p buffer, which is
 * made to hold exactly the bytes read: fewer than requested, when the file
 * holds fewer. The data the buffer held before is dropped, and so is whatever
 * has been read when the reading fails.
 *
 * On any failure an error is raised on the context of the buffer.
 *
 * @param buffer  Buffer to read the contents into.
 * @param path    Path to the file to read.
 * @param offset  Offset to read from, or a negative value to read from the
 *                beginning of the file.
 * @param length  Maximum number of bytes to read, or zero to read the whole
 *                file.
 *
 * @error DMI_ERROR_NULL_ARGUMENT Path is `nullptr`
 * @error DMI_ERROR_FILE_OPEN File cannot be opened
 * @error DMI_ERROR_FILE_STAT Length of the file cannot be told
 * @error DMI_ERROR_FILE_READ File cannot be read
 * @error DMI_ERROR_OUT_OF_MEMORY Buffer cannot hold the data
 *
 * @return `true` on success, `false` otherwise.
 */
__dmi_api bool dmi_file_load(
        dmi_buffer_t *buffer,
        const char   *path,
        off_t         offset,
        size_t        length);

#if !defined(_WIN32)
/**
 * @brief Reads a region of a device or file into a buffer.
 *
 * Opens @p path, maps @p length bytes starting at physical offset @p base
 * into memory using `mmap`(2), copies the data into @p buffer, then unmaps
 * the region. Page alignment is handled internally. The data the buffer held
 * before is dropped, and so is whatever has been read when the reading fails.
 *
 * On any failure an error is raised on the context of the buffer.
 *
 * @note Not available on Windows.
 *
 * @param buffer Buffer to read the region into.
 * @param path   Path to the device or file to read (e.g. `/dev/mem`).
 * @param base   Physical byte offset to start reading from.
 * @param length Number of bytes to read; must be greater than zero.
 *
 * @error DMI_ERROR_NULL_ARGUMENT Path is `nullptr`, or length is zero
 * @error DMI_ERROR_FILE_OPEN Device cannot be opened
 * @error DMI_ERROR_FILE_MAP Region cannot be mapped
 * @error DMI_ERROR_OUT_OF_MEMORY Buffer cannot hold the data
 *
 * @return `true` on success, `false` otherwise.
 */
    __dmi_api bool dmi_memory_load(dmi_buffer_t *buffer, const char *path, size_t base, size_t length);
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
