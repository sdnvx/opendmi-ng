//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/writer.h>
#include <opendmi/entity.h>
#include <opendmi/error.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>

//
// Size the buffers start at, which fits the formatted areas of most of the
// structures without growing.
//
#define DMI_WRITER_INITIAL_CAPACITY 64

static bool dmi_writer_reserve(dmi_writer_t *writer, size_t length);
static bool dmi_writer_add_str(dmi_writer_t *writer, const char *value, size_t *index);
static bool dmi_writer_has_str(const dmi_writer_t *writer, size_t index);

bool dmi_writer_initialize(
        dmi_writer_t       *writer,
        const dmi_entity_t *entity,
        dmi_encode_mode_t   mode,
        dmi_version_t       version)
{
    if ((writer == nullptr) or (entity == nullptr)) {
        dmi_error_raise_ex(nullptr, DMI_ERROR_NULL_ARGUMENT, "writer or entity");
        return false;
    }

    *writer = (dmi_writer_t){
        .entity  = entity,
        .mode    = mode,
        .version = version
    };

    // Only the preserve mode reads the data the structure has been decoded
    // from, and the canonical one has nothing to read at all
    if (mode == DMI_ENCODE_MODE_PRESERVE) {
        dmi_reader_initialize(&writer->source, entity);

        // Strings keep their numbers, including the ones nothing refers to
        for (size_t i = 0; i < entity->string_count; i++) {
            size_t index = 0;

            if (not dmi_writer_add_str(writer, entity->strings[i].raw, &index)) {
                dmi_writer_destroy(writer);
                return false;
            }
        }
    }

    dmi_header_t header = {
        .type   = (dmi_byte_t)entity->type,
        .length = 0,
        .handle = dmi_encode((dmi_handle_t)entity->handle)
    };

    if (not dmi_writer_put_bytes(writer, &header, sizeof(header))) {
        dmi_writer_destroy(writer);
        return false;
    }

    return true;
}

void dmi_writer_destroy(dmi_writer_t *writer)
{
    if (writer == nullptr)
        return;

    for (size_t i = 0; i < writer->string_count; i++)
        dmi_free(writer->strings[i]);

    dmi_free(writer->data);
    dmi_free(writer->strings);

    writer->data     = nullptr;
    writer->strings  = nullptr;
    writer->length   = 0;
    writer->capacity = 0;

    writer->string_count    = 0;
    writer->string_capacity = 0;
}

bool dmi_writer_put_bytes(dmi_writer_t *writer, const void *data, size_t length)
{
    assert(writer != nullptr);

    if (length == 0)
        return true;

    if (not dmi_writer_reserve(writer, length))
        return false;

    memcpy(writer->data + writer->length, data, length);
    writer->length += length;

    // Source stays in step with what is written, for as long as it lasts,
    // and there is none in the canonical mode
    size_t remaining = dmi_writer_remaining(writer);

    if (remaining > 0)
        dmi_reader_skip(&writer->source, (length < remaining) ? length : remaining);

    return true;
}

bool dmi_writer_copy(dmi_writer_t *writer, size_t length)
{
    assert(writer != nullptr);

    for (size_t i = 0; i < length; i++) {
        dmi_byte_t data = 0;

        if (writer->mode == DMI_ENCODE_MODE_PRESERVE)
            dmi_writer_peek(writer, &data, sizeof(data));

        if (not dmi_writer_put_bytes(writer, &data, sizeof(data)))
            return false;
    }

    return true;
}

bool dmi_writer_put_string(dmi_writer_t *writer, const char *value)
{
    assert(writer != nullptr);

    const dmi_entity_t *entity = writer->entity;
    dmi_byte_t          number = 0;

    if (value == nullptr) {
        // A reference to a string the structure does not have is kept as it
        // is, since the model says nothing about the number it carried
        dmi_byte_t original = 0;

        if (dmi_writer_peek(writer, &original, sizeof(original)) and
            (original != 0) and not dmi_writer_has_str(writer, original))
            number = original;

        return dmi_writer_put_bytes(writer, &number, sizeof(number));
    }

    // Strings of the structure are known by their own pointers, so that a
    // field keeps the number of the string it has been decoded from even
    // when another string has the same text
    size_t index = 0;

    for (size_t i = 0; i < entity->string_count; i++) {
        const dmi_string_entry_t *entry = &entity->strings[i];

        if ((entry->raw == nullptr) or ((value != entry->raw) and (value != entry->pretty)))
            continue;

        if (writer->mode == DMI_ENCODE_MODE_PRESERVE) {
            index = i + 1;
            break;
        }

        // Strings are written as they were stored, rather than as they are
        // printed
        value = entry->raw;
        break;
    }

    if ((index == 0) and not dmi_writer_add_str(writer, value, &index))
        return false;

    if (index > UINT8_MAX) {
        dmi_error_raise_ex(dmi_entity_context(entity), DMI_ERROR_INVALID_ARGUMENT,
                           "0x%04x: string %zu cannot be referred to", entity->handle, index);
        return false;
    }

    number = (dmi_byte_t)index;

    return dmi_writer_put_bytes(writer, &number, sizeof(number));
}

bool dmi_writer_peek(const dmi_writer_t *writer, void *data, size_t length)
{
    assert(writer != nullptr);

    if (writer->source.entity == nullptr)
        return false;

    return dmi_reader_get_bytes_at(&writer->source, data, writer->source.position, length);
}

size_t dmi_writer_remaining(const dmi_writer_t *writer)
{
    assert(writer != nullptr);

    if (writer->source.entity == nullptr)
        return 0;

    return dmi_reader_remaining(&writer->source);
}

size_t dmi_writer_tell(const dmi_writer_t *writer)
{
    assert(writer != nullptr);

    return writer->length;
}

bool dmi_writer_finish(dmi_writer_t *writer)
{
    assert(writer != nullptr);
    assert(writer->length >= sizeof(dmi_header_t));

    if (writer->length > UINT8_MAX) {
        dmi_error_raise_ex(dmi_entity_context(writer->entity), DMI_ERROR_INVALID_ARGUMENT,
                           "0x%04x: formatted area of %zu bytes", writer->entity->handle,
                           writer->length);
        return false;
    }

    dmi_header_t *header = (dmi_header_t *)writer->data;

    header->length = (dmi_byte_t)writer->length;

    // Header the source data has is kept whenever it reads as the one
    // written, the way the structures are read: a header of no length stands
    // for the end of the table, whatever type it names
    if (writer->mode == DMI_ENCODE_MODE_PRESERVE) {
        dmi_header_t original;

        if (dmi_reader_get_bytes_at(&writer->source, &original, 0, sizeof(original))) {
            bool same_handle = (original.handle == header->handle);
            bool same_type   = (original.length == 0)
                             ? ((header->type == DMI_TYPE(END_OF_TABLE)) and
                                (header->length == sizeof(dmi_header_t)))
                             : ((original.type == header->type) and (original.length == header->length));

            if (same_handle and same_type)
                *header = original;
        }
    }

    return true;
}

static bool dmi_writer_reserve(dmi_writer_t *writer, size_t length)
{
    if (writer->capacity - writer->length >= length)
        return true;

    size_t capacity = (writer->capacity != 0) ? writer->capacity : DMI_WRITER_INITIAL_CAPACITY;

    while (capacity - writer->length < length)
        capacity *= 2;

    dmi_byte_t *data = realloc(writer->data, capacity);
    if (data == nullptr) {
        dmi_error_raise(dmi_entity_context(writer->entity), DMI_ERROR_OUT_OF_MEMORY);
        return false;
    }

    writer->data     = data;
    writer->capacity = capacity;

    return true;
}

//
// Strings written from scratch are shared between the fields referring to
// the same text, which is how the specification numbers them.
//
static bool dmi_writer_add_str(dmi_writer_t *writer, const char *value, size_t *index)
{
    if (writer->mode == DMI_ENCODE_MODE_CANONICAL) {
        for (size_t i = 0; i < writer->string_count; i++) {
            if ((writer->strings[i] != nullptr) and (strcmp(writer->strings[i], value) == 0)) {
                *index = i + 1;
                return true;
            }
        }
    }

    if (writer->string_count == writer->string_capacity) {
        size_t capacity = (writer->string_capacity != 0) ? writer->string_capacity * 2 : 8;

        char **strings = realloc(writer->strings, capacity * sizeof(*strings));
        if (strings == nullptr) {
            dmi_error_raise(dmi_entity_context(writer->entity), DMI_ERROR_OUT_OF_MEMORY);
            return false;
        }

        writer->strings         = strings;
        writer->string_capacity = capacity;
    }

    // Strings are copied, so that the ones made up while writing, e.g. dates,
    // live as long as the writer does. A string the structure has lost is
    // kept as a gap, so that the ones after it keep their numbers
    char *copy = nullptr;

    if (value != nullptr) {
        copy = strdup(value);
        if (copy == nullptr) {
            dmi_error_raise(dmi_entity_context(writer->entity), DMI_ERROR_OUT_OF_MEMORY);
            return false;
        }
    }

    writer->strings[writer->string_count++] = copy;
    *index = writer->string_count;

    return true;
}

static bool dmi_writer_has_str(const dmi_writer_t *writer, size_t index)
{
    return (index > 0) and (index <= writer->string_count) and
           (writer->strings[index - 1] != nullptr);
}
