//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_WRITER_H
#define OPENDMI_WRITER_H

#pragma once

#include <opendmi/buffer.h>
#include <opendmi/utils/codec.h>

#ifndef DMI_WRITER_T
#   define DMI_WRITER_T
    typedef struct dmi_writer dmi_writer_t;
#endif // !DMI_WRITER_T

#ifndef DMI_WRITER_MARK_T
#   define DMI_WRITER_MARK_T
    typedef struct dmi_writer_mark dmi_writer_mark_t;
#endif // !DMI_WRITER_MARK_T

/**
 * @brief Sequential writer of SMBIOS data.
 *
 * Writes bytes into a range of a `dmi_buffer_t`, which holds them, the way
 * `dmi_reader_t` reads them out of one. What the bytes stand for is of no
 * concern to the writer: the caller writes the structures, the entry points
 * and whatever else the data is made of.
 *
 * The cursor may be moved back over the bytes written already, so that a
 * value the rest of the data decides, such as a length or a checksum, is
 * written once it is known.
 *
 * @note All fields are maintained internally. Do not modify them directly;
 *       use the writer API instead.
 */
struct dmi_writer
{
    /**
     * @brief Buffer holding the data being written.
     */
    dmi_buffer_t *buffer;

    /**
     * @brief Offset the range being written begins at within the buffer.
     */
    size_t base;

    /**
     * @brief Number of the bytes the range holds at most, or a negative
     * number when the range is bounded by nothing but the data itself.
     *
     * A range of a length of its own is one which something else follows,
     * such as a record of a declared length or an area of a fixed size, and
     * the writer refuses to write past it rather than writing over whatever
     * comes next.
     */
    ssize_t length;

    /**
     * @brief Offset the next byte is written at, counted from the beginning
     * of the range.
     *
     * Advanced by sequential writes, and repositioned over the bytes written
     * already with `dmi_writer_seek`(3).
     */
    size_t position;
};

/**
 * @brief Position of a writer cursor, taken with `dmi_writer_mark`(3).
 *
 * Marks are values: they are copied rather than allocated, nest without any
 * bookkeeping, and need no release. A mark stays valid for as long as the
 * writer holds the data it was taken over.
 *
 * @note All fields are maintained internally. Do not read or modify them
 *       directly; pass the mark to `dmi_writer_rewind`(3) instead.
 */
struct dmi_writer_mark
{
    /**
     * @brief Buffer the writer was writing into when the mark was taken.
     */
    const dmi_buffer_t *buffer;

    /**
     * @brief Offset the range being written begins at, which tells the marks
     * of the writers of different ranges apart.
     */
    size_t base;

    /**
     * @brief Cursor position the mark was taken at.
     */
    size_t position;
};

__BEGIN_DECLS

/**
 * @brief Initialize a writer of a range of a buffer.
 *
 * The cursor is positioned at the beginning of the range, which the data is
 * written into and past: a range beginning at the end of the buffer appends
 * to it, and one beginning within the data written already writes over it.
 *
 * The writer holds no memory of its own, so there is nothing to destroy: the
 * data belongs to the buffer and outlives the writers of it.
 *
 * @param[out] writer Writer to initialize.
 * @param[in]  buffer Buffer to write the data into.
 * @param[in]  offset Offset the range begins at within the buffer.
 * @param[in]  length Number of the bytes the range holds at most, or a
 *                    negative number for a range of no length of its own.
 *
 * @return `true` on success, `false` if @p writer or @p buffer is @c nullptr,
 *         or if the buffer holds fewer bytes than @p offset.
 */
__dmi_api bool dmi_writer_initialize(
        dmi_writer_t *writer,
        dmi_buffer_t *buffer,
        size_t        offset,
        ssize_t       length);

/**
 * @brief Seek to a byte position within the range written so far.
 *
 * The bytes at the position are written over, and the ones after them are
 * left as they are, which is how a value the rest of the data decides is
 * filled in once it is known.
 *
 * @param[in,out] writer   Writer to reposition.
 * @param[in]     position Byte offset from the beginning of the data.
 *
 * @return `true` on success, `false` if @p position is past the bytes of the
 *         range written so far.
 */
__dmi_api bool dmi_writer_seek(dmi_writer_t *writer, size_t position);

/**
 * @brief Take a mark of the current position of the writer cursor.
 *
 * Marks name a place to come back to without counting the bytes written in
 * between, which is what the values written last need: mark the place, write
 * whatever the value is counted over, then rewind and write it.
 *
 * @param[in] writer Writer to mark.
 *
 * @return Mark of the current position, which is rejected by
 *         `dmi_writer_rewind`(3) if @p writer is @c nullptr.
 */
__dmi_api dmi_writer_mark_t dmi_writer_mark(const dmi_writer_t *writer);

/**
 * @brief Reposition the writer cursor to a mark taken earlier.
 *
 * @param[in,out] writer Writer to reposition.
 * @param[in]     mark   Mark taken from the same writer with
 *                       `dmi_writer_mark`(3).
 *
 * @return `true` on success, `false` if the mark was taken of another writer,
 *         or over data which has been discarded since.
 */
__dmi_api bool dmi_writer_rewind(dmi_writer_t *writer, dmi_writer_mark_t mark);

/**
 * @brief Write data at the current position.
 *
 * @param[in,out] writer Writer to write to.
 * @param[in]     ptr    Data to write.
 * @param[in]     length Number of bytes to write.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Buffer of the writer cannot grow
 *
 * @return `true` on success, `false` if the bytes do not fit into the range.
 */
__dmi_api bool dmi_writer_put_bytes(dmi_writer_t *writer, const void *ptr, size_t length);

/**
 * @brief Write data at a given offset without moving the cursor.
 *
 * The data grows if the bytes are written past its end, and the ones between
 * the end and @p offset are made up as zeros, the way the ones which are
 * padded over are.
 *
 * @param[in,out] writer Writer to write to.
 * @param[in]     ptr    Data to write.
 * @param[in]     offset Byte offset from the beginning of the range.
 * @param[in]     length Number of bytes to write.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Buffer of the writer cannot grow
 *
 * @return `true` on success, `false` if the bytes do not fit into the range.
 */
__dmi_api bool dmi_writer_put_bytes_at(
        dmi_writer_t *writer,
        const void   *ptr,
        size_t        offset,
        size_t        length);

/**
 * @brief Pad the data up to @p length bytes from the current position.
 *
 * Only the bytes past the end of the data are made up, as zeros, which is
 * what the bytes a structure holds and nothing reads are left as. The ones
 * written already are stepped over rather than written over, so that padding
 * over a part of the data which is being written again keeps it.
 *
 * @param[in,out] writer Writer to write to.
 * @param[in]     length Number of bytes to pad with.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Buffer of the writer cannot grow
 *
 * @return `true` on success, `false` if the padding does not fit into the
 *         range.
 */
__dmi_api bool dmi_writer_skip(dmi_writer_t *writer, size_t length);

/**
 * @brief Write padding up to a length counted from a mark taken earlier.
 *
 * Records of a declared length are padded out to that length rather than to
 * the number of bytes written into them: mark the beginning of the record,
 * write whatever fields it holds, then pad it out to the length of the record
 * from the mark. The bytes written already are stepped over, the way
 * `dmi_writer_skip`(3) steps over them.
 *
 * Counting from the current position is what `dmi_writer_skip`(3) does.
 *
 * @param[in,out] writer Writer to write to.
 * @param[in]     from   Mark to count @p length from, taken from the same
 *                       writer with `dmi_writer_mark`(3).
 * @param[in]     length Number of bytes the record takes.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Buffer of the writer cannot grow
 *
 * @return `true` on success, `false` if the record is written past its own
 *         length already, if the padding does not fit into the range, or if
 *         @p from was taken of another writer.
 */
__dmi_api bool dmi_writer_skip_ex(
        dmi_writer_t      *writer,
        dmi_writer_mark_t  from,
        size_t             length);

/**
 * @brief Return the current position of the writer cursor.
 *
 * @param[in] writer Writer to query.
 *
 * @return Offset the next byte is written at, counted from the beginning of
 *         the range, or zero if @p writer is @c nullptr.
 */
__dmi_api size_t dmi_writer_tell(const dmi_writer_t *writer);

/**
 * @brief Reposition the writer cursor to the beginning of its range, so that
 * the data is written again from there.
 *
 * The data written already is kept until it is written over, the way the data
 * of a buffer is kept until it is.
 *
 * @param[in,out] writer Writer to reset.
 */
__dmi_api void dmi_writer_reset(dmi_writer_t *writer);

__END_DECLS

#endif // !OPENDMI_WRITER_H
