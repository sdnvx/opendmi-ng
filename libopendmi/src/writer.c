//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>

#include <opendmi/writer.h>
#include <opendmi/internal.h>

/**
 * @internal
 * @brief Number of the bytes of the range written so far, which is what the
 * data of the buffer holds past the beginning of the range.
 */
static size_t dmi_writer_length(const dmi_writer_t *writer);

/**
 * @internal
 * @brief Check whether the given part of the range is within the length the
 * range declares, which a range of no length of its own always is.
 */
static bool dmi_writer_fits(const dmi_writer_t *writer, size_t offset, size_t length);

bool dmi_writer_initialize(
        dmi_writer_t *writer,
        dmi_buffer_t *buffer,
        size_t        offset,
        ssize_t       length)
{
    if ((writer == nullptr) or (buffer == nullptr))
        return false;

    // Range begins within the data or right at its end, so that the data is
    // written or appended to rather than written past
    if (offset > buffer->length)
        return false;

    *writer = (dmi_writer_t){
        .buffer = buffer,
        .base   = offset,
        .length = length
    };

    return true;
}

bool dmi_writer_seek(dmi_writer_t *writer, size_t position)
{
    assert(writer != nullptr);

    if (position > dmi_writer_length(writer))
        return false;

    writer->position = position;

    return true;
}

dmi_writer_mark_t dmi_writer_mark(const dmi_writer_t *writer)
{
    if (writer == nullptr)
        return (dmi_writer_mark_t){};

    return (dmi_writer_mark_t){
        .buffer   = writer->buffer,
        .base     = writer->base,
        .position = writer->position
    };
}

bool dmi_writer_rewind(dmi_writer_t *writer, dmi_writer_mark_t mark)
{
    assert(writer != nullptr);

    if ((mark.buffer != writer->buffer) or (mark.base != writer->base))
        return false;

    return dmi_writer_seek(writer, mark.position);
}

bool dmi_writer_put_bytes(dmi_writer_t *writer, const void *ptr, size_t length)
{
    assert(writer != nullptr);

    if (not dmi_writer_put_bytes_at(writer, ptr, writer->position, length))
        return false;

    writer->position += length;

    return true;
}

bool dmi_writer_put_bytes_at(
        dmi_writer_t *writer,
        const void   *ptr,
        size_t        offset,
        size_t        length)
{
    assert(writer != nullptr);

    if (not dmi_writer_fits(writer, offset, length))
        return false;

    return dmi_buffer_write(writer->buffer, ptr, writer->base + offset, length);
}

bool dmi_writer_skip(dmi_writer_t *writer, size_t length)
{
    assert(writer != nullptr);

    if (not dmi_writer_fits(writer, writer->position, length))
        return false;

    size_t end = writer->position + length;

    // Bytes which are there are stepped over rather than written over, and
    // only the ones past the data are made up, as zeros
    if ((end > dmi_writer_length(writer)) and
        not dmi_buffer_resize(writer->buffer, writer->base + end))
        return false;

    writer->position = end;

    return true;
}

bool dmi_writer_skip_ex(
        dmi_writer_t      *writer,
        dmi_writer_mark_t  from,
        size_t             length)
{
    assert(writer != nullptr);

    if ((from.buffer != writer->buffer) or (from.base != writer->base))
        return false;

    size_t end = from.position + length;

    // A record longer than its own length is one the caller has miscounted,
    // and padding it out cannot undo that
    if (end < writer->position)
        return false;

    return dmi_writer_skip(writer, end - writer->position);
}

size_t dmi_writer_tell(const dmi_writer_t *writer)
{
    if (writer == nullptr)
        return 0;

    return writer->position;
}

void dmi_writer_reset(dmi_writer_t *writer)
{
    assert(writer != nullptr);

    writer->position = 0;
}

static size_t dmi_writer_length(const dmi_writer_t *writer)
{
    size_t length = writer->buffer->length;

    length = (length > writer->base) ? length - writer->base : 0;

    // Data past the end of a range of a length of its own belongs to whatever
    // comes next, and is none of the writer's business
    if ((writer->length >= 0) and (length > (size_t)writer->length))
        length = (size_t)writer->length;

    return length;
}

static bool dmi_writer_fits(const dmi_writer_t *writer, size_t offset, size_t length)
{
    if (writer->length < 0)
        return true;

    size_t limit = (size_t)writer->length;

    // Written so that the sum of the offset and the length cannot overflow
    return (length <= limit) and (offset <= limit - length);
}
