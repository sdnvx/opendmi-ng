//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <memory.h>

#include <opendmi/entity.h>
#include <opendmi/stream.h>
#include <opendmi/internal.h>

bool dmi_stream_initialize(dmi_stream_t *stream, const dmi_entity_t *entity)
{
    if ((stream == nullptr) or (entity == nullptr))
        return false;

    stream->entity    = entity;
    stream->position  = 0;
    stream->remaining = entity->body_length;

    if (entity->overlay_data != nullptr)
        stream->data = entity->overlay_data;
    else
        stream->data = entity->data;

    return true;
}

bool dmi_stream_seek(dmi_stream_t *stream, size_t position)
{
    if (stream == nullptr)
        return false;
    if (position >= stream->entity->body_length)
        return false;

    stream->position  = position;
    stream->remaining = stream->entity->body_length - position;

    return true;
}

dmi_stream_mark_t dmi_stream_mark(const dmi_stream_t *stream)
{
    if (stream == nullptr)
        return (dmi_stream_mark_t){};

    return (dmi_stream_mark_t){
        .entity   = stream->entity,
        .position = stream->position
    };
}

bool dmi_stream_rewind(dmi_stream_t *stream, dmi_stream_mark_t mark)
{
    // Coming back to a mark is advancing by nothing from it
    return dmi_stream_skip_ex(stream, mark, 0);
}

bool dmi_stream_read_data(dmi_stream_t *stream, void *ptr, size_t length)
{
    if (!dmi_stream_read_data_at(stream, ptr, stream->position, length))
        return false;

    stream->position  += length;
    stream->remaining -= length;

    return true;
}

bool dmi_stream_read_data_at(const dmi_stream_t *stream, void *ptr, size_t offset, size_t length)
{
    if ((stream == nullptr) or (ptr == nullptr))
        return false;
    if (offset + length > stream->entity->body_length)
        return false;

    memcpy(ptr, stream->data + offset, length);

    return true;
}

//
// Place the cursor at the given number of bytes past the given position,
// which is what skipping is in either of its forms.
//
static bool dmi_stream_advance(dmi_stream_t *stream, size_t position, size_t length)
{
    // Written so that the sum of the position and the length cannot overflow
    size_t body_length = stream->entity->body_length;

    if ((length > body_length) or (position > (body_length - length)))
        return false;

    stream->position  = position + length;
    stream->remaining = body_length - stream->position;

    return true;
}

bool dmi_stream_skip(dmi_stream_t *stream, size_t length)
{
    if (stream == nullptr)
        return false;

    return dmi_stream_advance(stream, stream->position, length);
}

bool dmi_stream_skip_ex(dmi_stream_t *stream, dmi_stream_mark_t from, size_t length)
{
    if (stream == nullptr)
        return false;

    // Marks belong to the stream they were taken from
    if ((from.entity == nullptr) or (from.entity != stream->entity))
        return false;

    return dmi_stream_advance(stream, from.position, length);
}

bool dmi_stream_decode_bin(dmi_stream_t *stream, size_t length, dmi_binary_t *value)
{
    if ((stream == nullptr) or (value == nullptr))
        return false;

    size_t position = stream->position;

    // Data is referenced in place, so the cursor is only advanced
    if (not dmi_stream_skip(stream, length))
        return false;

    value->data   = (length > 0) ? stream->data + position : nullptr;
    value->length = length;

    return true;
}

size_t dmi_stream_tell(const dmi_stream_t *stream)
{
    if (stream == nullptr)
        return 0;

    return stream->position;
}

size_t dmi_stream_remaining(const dmi_stream_t *stream)
{
    if (stream == nullptr)
        return 0;

    return stream->entity->body_length - stream->position;
}

bool dmi_stream_is_done(const dmi_stream_t *stream)
{
    if (stream == nullptr)
        return true;

    return stream->position >= stream->entity->body_length;
}

bool dmi_stream_has(const dmi_stream_t *stream, size_t length)
{
    if (stream == nullptr)
        return false;

    return dmi_stream_remaining(stream) >= length;
}

void dmi_stream_reset(dmi_stream_t *stream)
{
    if (stream == nullptr)
        return;

    stream->position  = 0;
    stream->remaining = stream->entity->body_length;
}
