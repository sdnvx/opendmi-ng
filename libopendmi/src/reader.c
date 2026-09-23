//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <memory.h>

#include <opendmi/entity.h>
#include <opendmi/reader.h>
#include <opendmi/internal.h>

/**
 * @brief @internal
 */
static bool dmi_reader_advance(dmi_reader_t *reader, size_t position, size_t length);

bool dmi_reader_initialize(dmi_reader_t *reader, const dmi_entity_t *entity)
{
    if ((reader == nullptr) or (entity == nullptr))
        return false;

    reader->entity    = entity;
    reader->position  = 0;
    reader->remaining = entity->body_length;

    if (entity->overlay_data != nullptr)
        reader->data = entity->overlay_data;
    else
        reader->data = entity->data;

    return true;
}

bool dmi_reader_seek(dmi_reader_t *reader, size_t position)
{
    if (reader == nullptr)
        return false;
    if (position >= reader->entity->body_length)
        return false;

    reader->position  = position;
    reader->remaining = reader->entity->body_length - position;

    return true;
}

dmi_reader_mark_t dmi_reader_mark(const dmi_reader_t *reader)
{
    if (reader == nullptr)
        return (dmi_reader_mark_t){};

    return (dmi_reader_mark_t){
        .entity   = reader->entity,
        .position = reader->position
    };
}

bool dmi_reader_rewind(dmi_reader_t *reader, dmi_reader_mark_t mark)
{
    // Coming back to a mark is advancing by nothing from it
    return dmi_reader_skip_ex(reader, mark, 0);
}

bool dmi_reader_get_bytes(dmi_reader_t *reader, void *ptr, size_t length)
{
    if (!dmi_reader_get_bytes_at(reader, ptr, reader->position, length))
        return false;

    reader->position  += length;
    reader->remaining -= length;

    return true;
}

bool dmi_reader_get_bytes_at(const dmi_reader_t *reader, void *ptr, size_t offset, size_t length)
{
    if ((reader == nullptr) or (ptr == nullptr))
        return false;
    if (offset + length > reader->entity->body_length)
        return false;

    memcpy(ptr, reader->data + offset, length);

    return true;
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

    if ((from.entity == nullptr) or (from.entity != reader->entity))
        return false;

    return dmi_reader_advance(reader, from.position, length);
}

bool dmi_reader_get_binary(dmi_reader_t *reader, size_t length, dmi_binary_t *value)
{
    if ((reader == nullptr) or (value == nullptr))
        return false;

    size_t position = reader->position;

    // Data is referenced in place, so the cursor is only advanced
    if (not dmi_reader_skip(reader, length))
        return false;

    value->data   = (length > 0) ? reader->data + position : nullptr;
    value->length = length;

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

    return reader->entity->body_length - reader->position;
}

bool dmi_reader_is_done(const dmi_reader_t *reader)
{
    if (reader == nullptr)
        return true;

    return reader->position >= reader->entity->body_length;
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

    reader->position  = 0;
    reader->remaining = reader->entity->body_length;
}

static bool dmi_reader_advance(dmi_reader_t *reader, size_t position, size_t length)
{
    // Written so that the sum of the position and the length cannot overflow
    size_t body_length = reader->entity->body_length;

    if ((length > body_length) or (position > (body_length - length)))
        return false;

    reader->position  = position + length;
    reader->remaining = body_length - reader->position;

    return true;
}
