//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <opendmi/buffer.h>
#include <opendmi/error.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>

//
// Size the memory of a buffer grows by, which fits the data of most of the
// structures at once.
//
#define DMI_BUFFER_CAPACITY_STEP 128

static bool dmi_buffer_reserve(dmi_buffer_t *buffer, size_t capacity);

dmi_buffer_t *dmi_buffer_create(dmi_context_t *context)
{
    dmi_buffer_t *buffer = dmi_alloc(context, sizeof(*buffer));

    if (buffer == nullptr)
        return nullptr;

    buffer->context = context;

    return buffer;
}

dmi_context_t *dmi_buffer_context(const dmi_buffer_t *buffer)
{
    if (buffer == nullptr)
        return nullptr;

    return buffer->context;
}

void dmi_buffer_destroy(dmi_buffer_t *buffer)
{
    if (buffer == nullptr)
        return;

    dmi_free(buffer->data);
    dmi_free(buffer);
}

bool dmi_buffer_is_empty(const dmi_buffer_t *buffer)
{
    if (buffer == nullptr)
        return true;

    return buffer->length == 0;
}

bool dmi_buffer_resize(dmi_buffer_t *buffer, size_t length)
{
    assert(buffer != nullptr);

    if (length > buffer->length) {
        if (not dmi_buffer_reserve(buffer, length))
            return false;

        // Bytes the buffer gains are its own rather than whatever the memory
        // held before
        memset(buffer->data + buffer->length, 0, length - buffer->length);
    }

    buffer->length = length;

    return true;
}

bool dmi_buffer_assign(dmi_buffer_t *buffer, const void *ptr, size_t length)
{
    assert(buffer != nullptr);
    assert((ptr != nullptr) or (length == 0));

    if (not dmi_buffer_reserve(buffer, length))
        return false;

    if (length > 0)
        memcpy(buffer->data, ptr, length);

    buffer->length = length;

    return true;
}

bool dmi_buffer_read(
        const dmi_buffer_t *buffer,
        void               *ptr,
        size_t              offset,
        size_t              length)
{
    assert(buffer != nullptr);
    assert((ptr != nullptr) or (length == 0));

    if ((offset > buffer->length) or (length > buffer->length - offset))
        return false;

    if (length > 0)
        memcpy(ptr, buffer->data + offset, length);

    return true;
}

bool dmi_buffer_write(
        dmi_buffer_t *buffer,
        const void   *ptr,
        size_t        offset,
        size_t        length)
{
    assert(buffer != nullptr);
    assert((ptr != nullptr) or (length == 0));

    if (length == 0)
        return true;

    // Data past the end of the buffer makes it longer, and the bytes between
    // the two are the buffer's own rather than whatever the memory held
    if ((offset + length > buffer->length) and
        not dmi_buffer_resize(buffer, offset + length))
        return false;

    memcpy(buffer->data + offset, ptr, length);

    return true;
}

const dmi_data_t *dmi_buffer_at(
        const dmi_buffer_t *buffer,
        size_t              offset,
        size_t              length)
{
    assert(buffer != nullptr);

    if ((offset > buffer->length) or (length > buffer->length - offset))
        return nullptr;

    return buffer->data + offset;
}

void dmi_buffer_clear(dmi_buffer_t *buffer)
{
    assert(buffer != nullptr);

    buffer->length = 0;
}

//
// Make room for the given number of the bytes, which is the only thing the
// memory of a buffer is grown by.
//
static bool dmi_buffer_reserve(dmi_buffer_t *buffer, size_t capacity)
{
    if (capacity <= buffer->capacity)
        return true;

    // Memory grows by whole steps, so that the data written byte by byte does
    // not reallocate at every write
    size_t reserved = buffer->capacity;

    while (reserved < capacity)
        reserved += DMI_BUFFER_CAPACITY_STEP;

    dmi_byte_t *data = realloc(buffer->data, reserved);
    if (data == nullptr) {
        dmi_error_raise(buffer->context, DMI_ERROR_OUT_OF_MEMORY);
        return false;
    }

    buffer->data     = data;
    buffer->capacity = reserved;

    return true;
}
