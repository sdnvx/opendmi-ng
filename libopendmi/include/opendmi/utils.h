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
#include <opendmi/error.h>

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
 *
 * @error DMI_ERROR_ARGUMENT_NULL Data is `nullptr`
 *
 * @return @c true if the checksum is valid (byte sum equals zero), @c false
 *         otherwise, or if @p data is @c nullptr.
 */
__dmi_api bool dmi_checksum_test(const void *data, size_t length);

/**
 * @brief Computes an SMBIOS-style 8-bit checksum.
 *
 * Computes the value which, when added to all bytes in @p data over
 * @p length bytes, results in zero sum modulo 256. The checksum field within
 * @p data must be set to zero before the computation.
 *
 * @param[in]  data      Pointer to the data block to compute checksum of.
 * @param[in]  length    Number of bytes to sum.
 * @param[out] pchecksum Variable to store the checksum in.
 *
 * @error DMI_ERROR_ARGUMENT_NULL Data or checksum pointer is `nullptr`
 *
 * @return @c true if the checksum is computed, @c false if @p data or
 *         @p pchecksum is @c nullptr, in which case the variable is left as
 *         it is.
 */
__dmi_api bool dmi_checksum_calc(const void *data, size_t length, uint8_t *pchecksum);

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
 * @error DMI_ERROR_ARGUMENT_NULL Path is `nullptr`
 * @error DMI_ERROR_FILE_OPEN_FAILED File cannot be opened
 * @error DMI_ERROR_FILE_STAT_FAILED Length of the file cannot be told
 * @error DMI_ERROR_FILE_READ_FAILED File cannot be read
 * @error DMI_ERROR_OUT_OF_MEMORY Buffer cannot hold the data
 * @error DMI_ERROR_SERVICE_UNAVAILABLE Files are not accessible, as in the Linux kernel
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
 * @note Not available on Windows. In the Linux kernel @p path is ignored, and
 *       the region is mapped with `memremap()` instead.
 *
 * @param buffer Buffer to read the region into.
 * @param path   Path to the device or file to read (e.g. `/dev/mem`).
 * @param base   Physical byte offset to start reading from.
 * @param length Number of bytes to read; must be greater than zero.
 *
 * @error DMI_ERROR_ARGUMENT_NULL Path is `nullptr`
 * @error DMI_ERROR_ARGUMENT_INVALID Length is zero, or the process runs as root and the path is not
 *        a character device
 * @error DMI_ERROR_FILE_OPEN_FAILED Device cannot be opened
 * @error DMI_ERROR_FILE_STAT_FAILED Device cannot be examined
 * @error DMI_ERROR_FILE_MAP_FAILED Region cannot be mapped, lies beyond the end of a regular file, or
 *        lies beyond the range the system can address
 * @error DMI_ERROR_OUT_OF_MEMORY Buffer cannot hold the data
 *
 * @return `true` on success, `false` otherwise.
 */
    __dmi_api bool dmi_memory_load(dmi_buffer_t *buffer, const char *path, uint64_t base, size_t length);
#endif // !defined(_WIN32)

__END_DECLS

/**
 * @brief Allocates a zero-initialised array.
 *
 * Equivalent to `dmi_alloc(context, size * count)`, except that a product of
 * @p size and @p count which does not fit in `size_t` fails rather than wraps
 * around, since counts often come from firmware data. On allocation failure
 * and when @p context is not @c nullptr, raises a `DMI_ERROR_OUT_OF_MEMORY`
 * error on @p context.
 *
 * @param context DMI context used for error reporting, or @c nullptr to suppress
 *                error reporting.
 * @param size    Size of each element in bytes.
 * @param count   Number of elements.
 * @return Pointer to the allocated array, or @c nullptr on failure.
 */
static inline void *dmi_alloc_array(dmi_context_t *context, size_t size, size_t count)
{
    // Array which does not fit in the address space cannot be allocated
    if ((size != 0) && (count > SIZE_MAX / size)) {
        if (context != nullptr)
            dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);

        return nullptr;
    }

    return dmi_alloc(context, size * count);
}

#endif // !OPENDMI_UTILS_H
