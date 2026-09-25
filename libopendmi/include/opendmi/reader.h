//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_READER_H
#define OPENDMI_READER_H

#pragma once

#include <opendmi/buffer.h>
#include <opendmi/utils/codec.h>

#ifndef DMI_READER_T
#   define DMI_READER_T
    typedef struct dmi_reader dmi_reader_t;
#endif // !DMI_READER_T

#ifndef DMI_READER_MARK_T
#   define DMI_READER_MARK_T
    typedef struct dmi_reader_mark dmi_reader_mark_t;
#endif // !DMI_READER_MARK_T

/**
 * @brief Sequential reader of SMBIOS data.
 *
 * Reads bytes out of a range of a `dmi_buffer_t`, which holds them, the way
 * `dmi_writer_t` writes them into one. What the bytes stand for is of no
 * concern to the reader: the range it is given is a structure body, an entry
 * point or whatever else the caller reads.
 *
 * @note All fields are maintained internally. Do not modify them directly;
 *       use the reader API instead.
 */
struct dmi_reader
{
    /**
     * @brief Buffer holding the data being read.
     */
    const dmi_buffer_t *buffer;

    /**
     * @brief Offset the range being read begins at within the buffer.
     */
    size_t base;

    /**
     * @brief Number of the bytes of the range being read.
     */
    size_t length;

    /**
     * @brief Current byte offset from the beginning of the range.
     *
     * Advanced by sequential read operations. May be repositioned with
     * `dmi_reader_seek`(3) or reset to zero with `dmi_reader_reset`(3).
     */
    size_t position;
};

/**
 * @brief Position of a reader cursor, taken with `dmi_reader_mark`(3).
 *
 * Marks are values: they are copied rather than allocated, nest without any
 * bookkeeping, and need no release. A mark stays valid for as long as the
 * reader reads the same range of the same buffer.
 *
 * @note All fields are maintained internally. Do not read or modify them
 *       directly; pass the mark to `dmi_reader_rewind`(3) instead.
 */
struct dmi_reader_mark
{
    /**
     * @brief Buffer the reader was reading when the mark was taken.
     */
    const dmi_buffer_t *buffer;

    /**
     * @brief Offset the range being read begins at, which tells the marks of
     * the readers of different ranges apart.
     */
    size_t base;

    /**
     * @brief Cursor position the mark was taken at.
     */
    size_t position;
};

__BEGIN_DECLS

/**
 * @brief Initialize a reader of a range of a buffer.
 *
 * The cursor is positioned at the beginning of the range, and the reader
 * reads nothing outside it.
 *
 * @param[out] reader Reader to initialize.
 * @param[in]  buffer Buffer holding the data to read.
 * @param[in]  offset Offset the range begins at within the buffer.
 * @param[in]  length Number of the bytes of the range.
 *
 * @return `true` on success, `false` if @p reader or @p buffer is @c nullptr,
 *         or if the buffer holds fewer bytes than the range.
 */
__dmi_api bool dmi_reader_initialize(
        dmi_reader_t       *reader,
        const dmi_buffer_t *buffer,
        size_t              offset,
        size_t              length);

/**
 * @brief Seek to a byte position within the reader.
 *
 * Repositions the reader cursor to @p position bytes from the beginning of
 * the range. The end of the range is accepted, since it is where the cursor
 * stands once everything has been read, and a range may be read from there
 * with nothing left in it.
 *
 * @param[in,out] reader   Reader to reposition.
 * @param[in]     position Byte offset from the beginning of the range.
 *
 * @return `true` on success, `false` if @p position is past the end of the
 *         range.
 */
__dmi_api bool dmi_reader_seek(dmi_reader_t *reader, size_t position);

/**
 * @brief Take a mark of the current position of the reader cursor.
 *
 * Marks name a place to come back to without counting the bytes read in
 * between, which is what records of a declared length need: mark the
 * beginning of the record, read whatever fields it holds, then rewind and
 * skip the length of the record.
 *
 * @param[in] reader Reader to mark.
 *
 * @return Mark of the current position, which is rejected by
 *         `dmi_reader_rewind`(3) if @p reader is @c nullptr.
 */
__dmi_api dmi_reader_mark_t dmi_reader_mark(const dmi_reader_t *reader);

/**
 * @brief Reposition the reader cursor to a mark taken earlier.
 *
 * @param[in,out] reader Reader to reposition.
 * @param[in]     mark   Mark taken from the same reader with
 *                       `dmi_reader_mark`(3).
 *
 * @return `true` on success, `false` if the mark was taken from a reader
 *         reading another range.
 */
__dmi_api bool dmi_reader_rewind(dmi_reader_t *reader, dmi_reader_mark_t mark);

/**
 * @brief Read data from the reader at the current position.
 *
 * Copies @p length bytes from the reader into the buffer pointed to by
 * @p ptr and advances the reader cursor.
 *
 * @param[in,out] reader Reader to read from.
 * @param[out]    ptr    Buffer that receives the data; must be at least
 *                       @p length bytes.
 * @param[in]     length Number of bytes to read.
 *
 * @return `true` on success, `false` if there are fewer than @p length bytes
 *         remaining in the reader.
 */
__dmi_api bool dmi_reader_get_bytes(dmi_reader_t *reader, void *ptr, size_t length);

/**
 * @brief Read data from the reader at a given offset without advancing the
 *        cursor.
 *
 * Copies @p length bytes starting at @p offset from the beginning of the
 * range into the buffer pointed to by @p ptr. The reader cursor is not
 * modified.
 *
 * @param[in]  reader Reader to read from.
 * @param[out] ptr    Buffer that receives the data; must be at least
 *                    @p length bytes.
 * @param[in]  offset Byte offset from the beginning of the range.
 * @param[in]  length Number of bytes to read.
 *
 * @return `true` on success, `false` if the requested range exceeds the
 *         reader bounds.
 */
__dmi_api bool dmi_reader_get_bytes_at(const dmi_reader_t *reader, void *ptr, size_t offset, size_t length);

/**
 * @brief Refer to the bytes at the current position, without copying them.
 *
 * The bytes belong to the buffer being read and stay valid for as long as it
 * holds them. The cursor is advanced, the way it is by reading them.
 *
 * @param[in,out] reader Reader to read from.
 * @param[in]     length Number of bytes to refer to, may be zero.
 * @param[out]    ptr    Pointer to the bytes, set to @c nullptr if @p length
 *                       is zero.
 *
 * @return `true` on success, `false` if there are fewer than @p length bytes
 *         remaining in the range.
 */
__dmi_api bool dmi_reader_ref_bytes(dmi_reader_t *reader, size_t length, const dmi_data_t **ptr);

/**
 * @brief Advance the reader cursor without reading data.
 *
 * Moves the reader cursor forward by @p length bytes, effectively discarding
 * that many bytes from the current position.
 *
 * @param[in,out] reader Reader to advance.
 * @param[in]     length Number of bytes to skip.
 *
 * @return `true` on success, `false` if there are fewer than @p length bytes
 *         remaining in the reader.
 */
__dmi_api bool dmi_reader_skip(dmi_reader_t *reader, size_t length);

/**
 * @brief Advance the reader cursor from a mark taken earlier.
 *
 * Records of a declared length are stepped over by that length rather than
 * by the number of bytes read from them: mark the beginning of the record,
 * read whatever fields it is known to hold, then advance by the length of
 * the record from the mark.
 *
 * Counting from the current position is what `dmi_reader_skip`(3) does.
 *
 * @param[in,out] reader Reader to advance.
 * @param[in]     from   Mark to count @p length from, taken from the same
 *                       reader with `dmi_reader_mark`(3).
 * @param[in]     length Number of bytes to advance by.
 *
 * @return `true` on success, `false` if the resulting position is beyond the
 *         end of the range, or if @p from was taken from a reader reading
 *         another range.
 */
__dmi_api bool dmi_reader_skip_ex(
        dmi_reader_t      *reader,
        dmi_reader_mark_t  from,
        size_t             length);

/**
 * @brief Return the current position of the reader cursor.
 *
 * The position is the offset of the next byte to be read from the beginning
 * of the range, so reading a field right after the call gives the offset of
 * that field within the range.
 *
 * @param[in] reader Reader to query.
 *
 * @return Offset of the cursor from the beginning of the range, or zero if
 *         @p reader is @c nullptr.
 */
__dmi_api size_t dmi_reader_tell(const dmi_reader_t *reader);

/**
 * @brief Return the number of unread bytes in the reader.
 *
 * @param[in] reader Reader to query.
 *
 * @return Number of bytes from the current cursor position to the end of the
 *         range.
 */
__dmi_api size_t dmi_reader_remaining(const dmi_reader_t *reader);

/**
 * @brief Check whether all data in the reader has been consumed.
 *
 * @param[in] reader Reader to check.
 *
 * @return `true` if there are no remaining bytes to read, `false` otherwise.
 */
__dmi_api bool dmi_reader_is_done(const dmi_reader_t *reader);

/**
 * @brief Check if the reader contains at least the given number of bytes.
 *
 * @param[in] reader Reader to check.
 * @param[in] length Number of bytes.
 *
 * @return `true` if at least @p length bytes remain from the current
 *         position, `false` otherwise.
 */
__dmi_api bool dmi_reader_has(const dmi_reader_t *reader, size_t length);

/**
 * @brief Reset the reader cursor to the beginning.
 *
 * Repositions the reader cursor back to the start of the range so that it
 * can be read again from the beginning.
 *
 * @param[in,out] reader Reader to reset.
 */
__dmi_api void dmi_reader_reset(dmi_reader_t *reader);

__END_DECLS

#endif // !OPENDMI_READER_H
