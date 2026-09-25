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
#include <opendmi/encoder.h>
#include <opendmi/entity.h>
#include <opendmi/error.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>

static bool dmi_encoder_add_string(dmi_encoder_t *encoder, const char *value, size_t *index);
static bool dmi_encoder_has_string(const dmi_encoder_t *encoder, size_t index);

bool dmi_encoder_initialize(
        dmi_encoder_t      *encoder,
        dmi_buffer_t       *buffer,
        const dmi_entity_t *entity,
        dmi_encode_mode_t   mode,
        dmi_version_t       version)
{
    if ((encoder == nullptr) or (buffer == nullptr) or (entity == nullptr)) {
        dmi_error_raise_ex(dmi_buffer_context(buffer), DMI_ERROR_NULL_ARGUMENT,
                           "encoder, buffer or entity");
        return false;
    }

    *encoder = (dmi_encoder_t){
        .entity  = entity,
        .mode    = mode,
        .version = version
    };

    // Structure is written at the end of the data, so that the structures of
    // a table follow one another
    if (not dmi_writer_initialize(&encoder->writer, buffer, buffer->length, -1))
        return false;

    // Only the preserve mode reads the data the structure has been decoded
    // from, and the canonical one has nothing to read at all
    if (mode == DMI_ENCODE_MODE_PRESERVE) {
        dmi_reader_initialize(&encoder->source, dmi_entity_buffer(entity),
                              dmi_entity_offset(entity), entity->body_length);

        // Strings keep their numbers, including the ones nothing refers to
        for (size_t i = 0; i < entity->string_count; i++) {
            size_t index = 0;

            if (not dmi_encoder_add_string(encoder, entity->strings[i].raw, &index)) {
                dmi_encoder_finalize(encoder);
                return false;
            }
        }
    }

    dmi_header_t header = {
        .type   = (dmi_byte_t)entity->type,
        .length = 0,
        .handle = dmi_encode((dmi_handle_t)entity->handle)
    };

    if (not dmi_encoder_put_bytes(encoder, &header, sizeof(header))) {
        dmi_encoder_finalize(encoder);
        return false;
    }

    return true;
}

void dmi_encoder_finalize(dmi_encoder_t *encoder)
{
    if (encoder == nullptr)
        return;

    for (size_t i = 0; i < encoder->string_count; i++)
        dmi_free(encoder->strings[i]);

    dmi_free(encoder->strings);

    encoder->strings         = nullptr;
    encoder->string_count    = 0;
    encoder->string_capacity = 0;
}

const dmi_entity_t *dmi_encoder_entity(const dmi_encoder_t *encoder)
{
    if (encoder == nullptr)
        return nullptr;

    return encoder->entity;
}

dmi_writer_t *dmi_encoder_writer(dmi_encoder_t *encoder)
{
    if (encoder == nullptr)
        return nullptr;

    return &encoder->writer;
}

bool dmi_encoder_put_bytes(dmi_encoder_t *encoder, const void *ptr, size_t length)
{
    assert(encoder != nullptr);

    if (length == 0)
        return true;

    if (not dmi_writer_put_bytes(&encoder->writer, ptr, length))
        return false;

    // Source stays in step with what is written, for as long as it lasts,
    // and there is none in the canonical mode
    size_t remaining = dmi_encoder_remaining(encoder);

    if (remaining > 0)
        dmi_reader_skip(&encoder->source, (length < remaining) ? length : remaining);

    return true;
}

bool dmi_encoder_put_bytes_at(
        dmi_encoder_t *encoder,
        const void    *ptr,
        size_t         offset,
        size_t         length)
{
    assert(encoder != nullptr);

    return dmi_writer_put_bytes_at(&encoder->writer, ptr, offset, length);
}

bool dmi_encoder_copy(dmi_encoder_t *encoder, size_t length)
{
    assert(encoder != nullptr);

    for (size_t i = 0; i < length; i++) {
        dmi_byte_t data = 0;

        if (encoder->mode == DMI_ENCODE_MODE_PRESERVE)
            dmi_encoder_peek(encoder, &data, sizeof(data));

        if (not dmi_encoder_put_bytes(encoder, &data, sizeof(data)))
            return false;
    }

    return true;
}

bool dmi_encoder_put_string(dmi_encoder_t *encoder, const char *value)
{
    assert(encoder != nullptr);

    const dmi_entity_t *entity = encoder->entity;
    dmi_byte_t          number = 0;

    if (value == nullptr) {
        // A reference to a string the structure does not have is kept as it
        // is, since the model says nothing about the number it carried
        dmi_byte_t original = 0;

        if (dmi_encoder_peek(encoder, &original, sizeof(original)) and
            (original != 0) and not dmi_encoder_has_string(encoder, original))
            number = original;

        return dmi_encoder_put_bytes(encoder, &number, sizeof(number));
    }

    // Strings of the structure are known by their own pointers, so that a
    // field keeps the number of the string it has been decoded from even
    // when another string has the same text
    size_t index = 0;

    for (size_t i = 0; i < entity->string_count; i++) {
        const dmi_string_entry_t *entry = &entity->strings[i];

        if ((entry->raw == nullptr) or ((value != entry->raw) and (value != entry->pretty)))
            continue;

        if (encoder->mode == DMI_ENCODE_MODE_PRESERVE) {
            index = i + 1;
            break;
        }

        // Strings are written as they were stored, rather than as they are
        // printed
        value = entry->raw;
        break;
    }

    if ((index == 0) and not dmi_encoder_add_string(encoder, value, &index))
        return false;

    if (index > UINT8_MAX) {
        dmi_error_raise_ex(dmi_entity_context(entity), DMI_ERROR_INVALID_ARGUMENT,
                           "0x%04x: string %zu cannot be referred to", entity->handle, index);
        return false;
    }

    number = (dmi_byte_t)index;

    return dmi_encoder_put_bytes(encoder, &number, sizeof(number));
}

bool dmi_encoder_peek(const dmi_encoder_t *encoder, void *ptr, size_t length)
{
    assert(encoder != nullptr);

    if (encoder->source.buffer == nullptr)
        return false;

    return dmi_reader_get_bytes_at(&encoder->source, ptr, encoder->source.position, length);
}

size_t dmi_encoder_remaining(const dmi_encoder_t *encoder)
{
    assert(encoder != nullptr);

    if (encoder->source.buffer == nullptr)
        return 0;

    return dmi_reader_remaining(&encoder->source);
}

size_t dmi_encoder_tell(const dmi_encoder_t *encoder)
{
    assert(encoder != nullptr);

    return dmi_writer_tell(&encoder->writer);
}

bool dmi_encoder_finish(dmi_encoder_t *encoder)
{
    assert(encoder != nullptr);

    size_t length = dmi_encoder_tell(encoder);

    assert(length >= sizeof(dmi_header_t));

    if (length > UINT8_MAX) {
        dmi_error_raise_ex(dmi_entity_context(encoder->entity), DMI_ERROR_INVALID_ARGUMENT,
                           "0x%04x: formatted area of %zu bytes", encoder->entity->handle, length);
        return false;
    }

    dmi_header_t header = {
        .type   = (dmi_byte_t)encoder->entity->type,
        .length = (dmi_byte_t)length,
        .handle = dmi_encode((dmi_handle_t)encoder->entity->handle)
    };

    // Header the source data has is kept whenever it reads as the one
    // written, the way the structures are read: a header of no length stands
    // for the end of the table, whatever type it names
    if (encoder->mode == DMI_ENCODE_MODE_PRESERVE) {
        dmi_header_t original;

        if (dmi_reader_get_bytes_at(&encoder->source, &original, 0, sizeof(original))) {
            bool same_handle = (original.handle == header.handle);
            bool same_type   = (original.length == 0)
                             ? ((header.type == DMI_TYPE(END_OF_TABLE)) and
                                (header.length == sizeof(dmi_header_t)))
                             : ((original.type == header.type) and (original.length == header.length));

            if (same_handle and same_type)
                header = original;
        }
    }

    return dmi_writer_put_bytes_at(&encoder->writer, &header, 0, sizeof(header));
}

//
// Strings written from scratch are shared between the fields referring to
// the same text, which is how the specification numbers them.
//
static bool dmi_encoder_add_string(dmi_encoder_t *encoder, const char *value, size_t *index)
{
    if (encoder->mode == DMI_ENCODE_MODE_CANONICAL) {
        for (size_t i = 0; i < encoder->string_count; i++) {
            if ((encoder->strings[i] != nullptr) and (strcmp(encoder->strings[i], value) == 0)) {
                *index = i + 1;
                return true;
            }
        }
    }

    if (encoder->string_count == encoder->string_capacity) {
        size_t capacity = (encoder->string_capacity != 0) ? encoder->string_capacity * 2 : 8;

        char **strings = realloc(encoder->strings, capacity * sizeof(*strings));
        if (strings == nullptr) {
            dmi_error_raise(dmi_entity_context(encoder->entity), DMI_ERROR_OUT_OF_MEMORY);
            return false;
        }

        encoder->strings         = strings;
        encoder->string_capacity = capacity;
    }

    // Strings are copied, so that the ones made up while writing, e.g. dates,
    // live as long as the encoder does. A string the structure has lost is
    // kept as a gap, so that the ones after it keep their numbers
    char *copy = nullptr;

    if (value != nullptr) {
        copy = strdup(value);
        if (copy == nullptr) {
            dmi_error_raise(dmi_entity_context(encoder->entity), DMI_ERROR_OUT_OF_MEMORY);
            return false;
        }
    }

    encoder->strings[encoder->string_count++] = copy;
    *index = encoder->string_count;

    return true;
}

static bool dmi_encoder_has_string(const dmi_encoder_t *encoder, size_t index)
{
    return (index > 0) and (index <= encoder->string_count) and
           (encoder->strings[index - 1] != nullptr);
}
