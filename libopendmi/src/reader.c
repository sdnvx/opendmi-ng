//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>

#include <opendmi/reader.h>
#include <opendmi/internal.h>

/**
 * @brief @internal
 */
static bool dmi_reader_advance(dmi_reader_t *reader, size_t position, size_t length);

bool dmi_reader_initialize(
        dmi_reader_t       *reader,
        const dmi_buffer_t *buffer,
        size_t              offset,
        size_t              length)
{
    if ((reader == nullptr) or (buffer == nullptr))
        return false;

    // Range is the reader's own view of the data, and what it reads past is
    // of no concern to it, so it is required to be there in the first place
    if (dmi_buffer_at(buffer, offset, length) == nullptr)
        return false;

    *reader = (dmi_reader_t){
        .buffer = buffer,
        .base   = offset,
        .length = length
    };

    return true;
}

bool dmi_reader_seek(dmi_reader_t *reader, size_t position)
{
    if (reader == nullptr)
        return false;
    // End of the range is a position too, which the cursor stands at once
    // everything has been read
    if (position > reader->length)
        return false;

    reader->position = position;

    return true;
}

dmi_reader_mark_t dmi_reader_mark(const dmi_reader_t *reader)
{
    if (reader == nullptr)
        return (dmi_reader_mark_t){};

    return (dmi_reader_mark_t){
        .buffer   = reader->buffer,
        .base     = reader->base,
        .position = reader->position
    };
}

bool dmi_reader_rewind(dmi_reader_t *reader, dmi_reader_mark_t mark)
{
    if (reader == nullptr)
        return false;

    return dmi_reader_skip_ex(reader, mark, 0);
}

bool dmi_reader_get_bytes(dmi_reader_t *reader, void *ptr, size_t length)
{
    if (reader == nullptr)
        return false;

    if (not dmi_reader_get_bytes_at(reader, ptr, reader->position, length))
        return false;

    reader->position += length;

    return true;
}

bool dmi_reader_get_bytes_at(const dmi_reader_t *reader, void *ptr, size_t offset, size_t length)
{
    if ((reader == nullptr) or (ptr == nullptr))
        return false;

    // Written so that the sum of the offset and the length cannot overflow
    if ((length > reader->length) or (offset > (reader->length - length)))
        return false;

    return dmi_buffer_read(reader->buffer, ptr, reader->base + offset, length);
}

bool dmi_reader_skip(dmi_reader_t *reader, size_t length)
{
    if (reader == nullptr)
        return false;

    return dmi_reader_advance(reader, reader->position, length);
}

bool dmi_reader_skip_ex(dmi_reader_t *reader, dmi_reader_mark_t from, size_t length)
{
    if (reader == nullptr)
        return false;

    if ((from.buffer != reader->buffer) or (from.base != reader->base))
        return false;

    return dmi_reader_advance(reader, from.position, length);
}

bool dmi_reader_ref_bytes(dmi_reader_t *reader, size_t length, const dmi_data_t **ptr)
{
    if ((reader == nullptr) or (ptr == nullptr))
        return false;

    size_t position = reader->position;

    // Bytes are referred to in place, so the cursor is only advanced
    if (not dmi_reader_skip(reader, length))
        return false;

    if (length > 0)
        *ptr = dmi_buffer_at(reader->buffer, reader->base + position, length);
    else
        *ptr = nullptr;

    return true;
}

size_t dmi_reader_tell(const dmi_reader_t *reader)
{
    if (reader == nullptr)
        return 0;

    return reader->position;
}

size_t dmi_reader_remaining(const dmi_reader_t *reader)
{
    if (reader == nullptr)
        return 0;

    return reader->length - reader->position;
}

bool dmi_reader_is_done(const dmi_reader_t *reader)
{
    if (reader == nullptr)
        return true;

    return reader->position >= reader->length;
}

bool dmi_reader_has(const dmi_reader_t *reader, size_t length)
{
    if (reader == nullptr)
        return false;

    return dmi_reader_remaining(reader) >= length;
}

void dmi_reader_reset(dmi_reader_t *reader)
{
    if (reader == nullptr)
        return;

    reader->position = 0;
}

static bool dmi_reader_advance(dmi_reader_t *reader, size_t position, size_t length)
{
    // Written so that the sum of the position and the length cannot overflow
    if ((length > reader->length) or (position > (reader->length - length)))
        return false;

    reader->position = position + length;

    return true;
}
