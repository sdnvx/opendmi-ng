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

    memcpy(ptr, stream->entity->data + offset, length);

    return true;
}

bool dmi_stream_skip(dmi_stream_t *stream, size_t length)
{
    if (stream == nullptr)
        return false;
    if (stream->position + length > stream->entity->body_length)
        return false;

    stream->position  += length;
    stream->remaining -= length;

    return true;
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
