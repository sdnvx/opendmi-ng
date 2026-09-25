//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_BUFFER_H
#define OPENDMI_BUFFER_H

#pragma once

#include <opendmi/types.h>

#ifndef DMI_BUFFER_T
#   define DMI_BUFFER_T
    typedef struct dmi_buffer dmi_buffer_t;
#endif // !DMI_BUFFER_T

/**
 * @brief Memory holding SMBIOS data.
 *
 * A buffer owns the bytes it holds, whether they have been read from a
 * device, from a dump, or written by the framework itself, so that everything
 * reading them — the decoded structures, the strings they refer to and the
 * binary members referring to the data in place — is valid for as long as the
 * buffer is.
 *
 * Data is read and written through `dmi_reader_t` and `dmi_writer_t`, which
 * are cursors over a range of a buffer and hold no memory of their own.
 *
 * Buffers live on their own rather than within the scope which makes them,
 * because the structures decoded from a buffer refer to it, and outlive
 * whatever has read it in.
 *
 * @note All fields are maintained internally. Do not modify them directly;
 *       use the buffer API instead.
 */
struct dmi_buffer
{
    /**
     * @brief Context the buffer belongs to, whose memory it holds and whose
     * error queue its errors are raised against.
     */
    dmi_context_t *context;

    /**
     * @brief Data the buffer holds, or @c nullptr while it holds none.
     */
    dmi_byte_t *data;

    /**
     * @brief Number of the bytes the buffer holds.
     */
    size_t length;

    /**
     * @internal
     * @brief Number of the bytes the memory of the buffer fits, which is no
     * business of anything but the buffer itself.
     */
    size_t capacity;
};

__BEGIN_DECLS

/**
 * @brief Create an empty buffer.
 *
 * No memory for the data is allocated until the buffer is written to or
 * grown.
 *
 * @param[in] context Context the buffer belongs to.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Buffer cannot be allocated
 *
 * @return Buffer on success, or @c nullptr on failure.
 */
__dmi_api dmi_buffer_t *dmi_buffer_create(dmi_context_t *context);

/**
 * @brief Get the context a buffer belongs to.
 *
 * @param[in] buffer Buffer to query.
 *
 * @return Context of the buffer, or @c nullptr if @p buffer is @c nullptr or
 *         belongs to none.
 */
__dmi_api dmi_context_t *dmi_buffer_context(const dmi_buffer_t *buffer);

/**
 * @brief Check whether a buffer holds no data.
 *
 * A buffer holds no data before anything has been put into it, and after it
 * has been cleared, whether it keeps the memory it has had or not.
 *
 * @param[in] buffer Buffer to check.
 *
 * @return `true` if the buffer holds no data, `false` otherwise.
 */
__dmi_api bool dmi_buffer_is_empty(const dmi_buffer_t *buffer);

/**
 * @brief Set the number of the bytes the buffer holds.
 *
 * The bytes a longer buffer gains are set to zero, and the ones a shorter
 * buffer loses are dropped without the memory being given back, so that the
 * buffer emptied with a length of zero writes the data next without
 * allocating again.
 *
 * @param[in,out] buffer Buffer to resize.
 * @param[in]     length Number of the bytes the buffer is to hold.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory of the buffer cannot grow
 *
 * @return `true` on success, `false` otherwise.
 */
__dmi_api bool dmi_buffer_resize(dmi_buffer_t *buffer, size_t length);

/**
 * @brief Replace the data of a buffer with a copy of the given data.
 *
 * Buffers own their memory, so the data is copied rather than referred to,
 * and the caller keeps whatever it has passed.
 *
 * @param[in,out] buffer Buffer to fill.
 * @param[in]     ptr    Data to copy, which may be @c nullptr when @p length
 *                       is zero.
 * @param[in]     length Number of the bytes to copy.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory of the buffer cannot grow
 *
 * @return `true` on success, `false` otherwise.
 */
__dmi_api bool dmi_buffer_assign(dmi_buffer_t *buffer, const void *ptr, size_t length);

/**
 * @brief Read data out of a buffer.
 *
 * @param[in]  buffer Buffer to read from.
 * @param[out] ptr    Buffer that receives the data.
 * @param[in]  offset Byte offset from the beginning of the data.
 * @param[in]  length Number of the bytes to read.
 *
 * @return `true` on success, `false` if the buffer holds fewer bytes than the
 *         requested range.
 */
__dmi_api bool dmi_buffer_read(
        const dmi_buffer_t *buffer,
        void               *ptr,
        size_t              offset,
        size_t              length);

/**
 * @brief Write data into a buffer.
 *
 * The data grows if the bytes are written past its end, and the ones between
 * the end and @p offset are set to zero, the way the bytes a longer buffer
 * gains are.
 *
 * @param[in,out] buffer Buffer to write to.
 * @param[in]     ptr    Data to write.
 * @param[in]     offset Byte offset from the beginning of the data.
 * @param[in]     length Number of the bytes to write.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory of the buffer cannot grow
 *
 * @return `true` on success, `false` otherwise.
 */
__dmi_api bool dmi_buffer_write(
        dmi_buffer_t *buffer,
        const void   *ptr,
        size_t        offset,
        size_t        length);

/**
 * @brief Get a pointer to the data a buffer holds at a given offset.
 *
 * The data is referred to in place rather than copied, which is how the
 * strings and the binary members of the decoded structures refer to it, and
 * stays valid for as long as the buffer holds it: writing to the buffer may
 * move the data, and destroying the buffer frees it.
 *
 * @param[in] buffer Buffer to refer to.
 * @param[in] offset Byte offset from the beginning of the data.
 * @param[in] length Number of the bytes to refer to, which may be zero.
 *
 * @return Pointer to the data, or @c nullptr if the buffer holds fewer bytes
 *         than the requested range.
 */
__dmi_api const dmi_data_t *dmi_buffer_at(
        const dmi_buffer_t *buffer,
        size_t              offset,
        size_t              length);

/**
 * @brief Drop the data of a buffer, keeping the memory it holds for the data
 * written next.
 *
 * @param[in,out] buffer Buffer to clear.
 */
__dmi_api void dmi_buffer_clear(dmi_buffer_t *buffer);

/**
 * @brief Free a buffer along with the data it holds.
 *
 * Everything referring to the data of the buffer, such as the binary members
 * of the structures decoded from it, refers to freed memory afterwards.
 *
 * @param[in,out] buffer Buffer to destroy, or @c nullptr.
 */
__dmi_api void dmi_buffer_destroy(dmi_buffer_t *buffer);

__END_DECLS

#endif // !OPENDMI_BUFFER_H
