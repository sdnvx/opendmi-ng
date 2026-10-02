//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>

#include <opendmi/decoder.h>
#include <opendmi/entity.h>
#include <opendmi/context.h>
#include <opendmi/error.h>
#include <opendmi/internal.h>

bool dmi_decoder_initialize(dmi_decoder_t *decoder, dmi_entity_t *entity)
{
    dmi_context_t *context = dmi_entity_context(entity);

    if (decoder == nullptr)
        return dmi_trace_argument_null(context, decoder);
    if (entity == nullptr)
        return dmi_trace_argument_null(context, entity);

    *decoder = (dmi_decoder_t){
        .entity = entity
    };

    if (not dmi_reader_initialize(&decoder->reader, dmi_entity_buffer(entity),
                                  dmi_entity_offset(entity), entity->body_length))
        return false;

    // Header has been read before the structure was decoded at all
    return dmi_reader_seek(&decoder->reader, sizeof(dmi_header_t));
}

dmi_entity_t *dmi_decoder_entity(const dmi_decoder_t *decoder)
{
    if (decoder == nullptr)
        return dmi_trace_argument_null(nullptr, decoder, nullptr);

    return decoder->entity;
}

dmi_context_t *dmi_decoder_context(const dmi_decoder_t *decoder)
{
    if (decoder == nullptr)
        return dmi_trace_argument_null(nullptr, decoder, nullptr);

    return dmi_entity_context(decoder->entity);
}

dmi_reader_t *dmi_decoder_reader(dmi_decoder_t *decoder)
{
    if (decoder == nullptr)
        return dmi_trace_argument_null(nullptr, decoder, nullptr);

    return &decoder->reader;
}

bool dmi_decoder_get_string(dmi_decoder_t *decoder, const char **value)
{
    if (decoder == nullptr)
        return dmi_trace_argument_null(nullptr, decoder);
    if (value == nullptr)
        return dmi_trace_argument_null(nullptr, value);

    dmi_string_t number = 0;

    if (not dmi_decoder_get(decoder, dmi_string_t, &number))
        return false;

    // A number the structure carries no string for is an error of the data,
    // which dmi_entity_string() reports, and leaves the value unset
    *value = dmi_entity_string(decoder->entity, number);

    return true;
}

bool dmi_decoder_get_bytes(dmi_decoder_t *decoder, void *ptr, size_t length)
{
    if (decoder == nullptr)
        return dmi_trace_argument_null(nullptr, decoder);

    return dmi_reader_get_bytes(&decoder->reader, ptr, length);
}

bool dmi_decoder_get_bytes_at(
        const dmi_decoder_t *decoder,
        void                *ptr,
        size_t               offset,
        size_t               length)
{
    if (decoder == nullptr)
        return dmi_trace_argument_null(nullptr, decoder);

    return dmi_reader_get_bytes_at(&decoder->reader, ptr, offset, length);
}

bool dmi_decoder_get_binary(dmi_decoder_t *decoder, size_t length, dmi_binary_t *value)
{
    if (decoder == nullptr)
        return dmi_trace_argument_null(nullptr, decoder);
    if (value == nullptr)
        return dmi_trace_argument_null(nullptr, value);

    // Binary data refers to the bytes of the structure in place
    if (not dmi_reader_ref_bytes(&decoder->reader, length, &value->data))
        return false;

    value->length = length;

    return true;
}

bool dmi_decoder_skip(dmi_decoder_t *decoder, size_t length)
{
    if (decoder == nullptr)
        return dmi_trace_argument_null(nullptr, decoder);

    return dmi_reader_skip(&decoder->reader, length);
}

size_t dmi_decoder_remaining(const dmi_decoder_t *decoder)
{
    if (decoder == nullptr)
        return dmi_trace_argument_null(nullptr, decoder, 0);

    return dmi_reader_remaining(&decoder->reader);
}

bool dmi_decoder_stop(dmi_decoder_t *decoder)
{
    if (decoder == nullptr)
        return dmi_trace_argument_null(nullptr, decoder);

    // Structure which ends where it is allowed to carries what it has, and
    // anything else is a structure which ends in the middle of its fields
    if (not dmi_reader_is_done(&decoder->reader))
        return dmi_decoder_incomplete(decoder);

    decoder->entity->state |= DMI_ENTITY_STATE_PARTIAL;

    return true;
}

bool dmi_decoder_incomplete(dmi_decoder_t *decoder)
{
    if (decoder == nullptr)
        return dmi_trace_argument_null(nullptr, decoder);

    dmi_entity_t  *entity  = decoder->entity;
    dmi_context_t *context = dmi_entity_context(entity);

    entity->state |= DMI_ENTITY_STATE_INCOMPLETE;

    size_t position  = dmi_reader_tell(&decoder->reader);
    size_t remaining = dmi_reader_remaining(&decoder->reader);

    if (remaining > 0) {
        dmi_log_notice(context,
                       "Handle 0x%04hx (%s): Incomplete fields at offset 0x%02zx, %zu byte%s ignored",
                       entity->handle, dmi_type_name(context, entity->type_id),
                       position, remaining, (remaining == 1) ? "" : "s");
    } else {
        dmi_log_notice(context,
                       "Handle 0x%04hx (%s): Incomplete fields at offset 0x%02zx",
                       entity->handle, dmi_type_name(context, entity->type_id), position);
    }

    return true;
}
