//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>

#include <opendmi/entity.h>
#include <opendmi/error.h>
#include <opendmi/field.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>

#include "field-internal.h"

/**
 * @internal
 * @brief Free the elements of an array, after whatever the decoded ones hold.
 *
 * @details
 * Only the counted elements have been decoded in full, and the one which has
 * failed has been released as it failed, so the others hold nothing.
 */
static void dmi_field_release_array(const dmi_field_t *field, dmi_data_t *info);

uintmax_t dmi_field_get(const dmi_field_t *field, const void *value)
{
    if (field == nullptr)
        return dmi_trace_argument_null(nullptr, field, 0);
    if (value == nullptr)
        return dmi_trace_argument_null(nullptr, value, 0);

    return dmi_field_load_member(field->member, value);
}

bool dmi_field_set(const dmi_field_t *field, void *value, uintmax_t number)
{
    if (field == nullptr)
        return dmi_trace_argument_null(nullptr, field);

    return dmi_field_store(field, value, number);
}

bool dmi_field_decode_kilobytes(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    return dmi_field_set(field, value, data->number << 10);
}

bool dmi_field_decode_kilobytes_last(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    return dmi_field_set(field, value, (data->number << 10) | 0x3FFu);
}

bool dmi_field_encode_kilobytes(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    data->number = dmi_field_get(field, value) >> 10;

    return true;
}

void dmi_field_choices_collect(const dmi_field_t *fields, dmi_field_choices_t *choices)
{
    for (const dmi_field_t *field = fields; field->type != DMI_FIELD_TYPE_NONE; field++) {
        if (not field->params.extended)
            continue;

        size_t offset = dmi_field_when_offset(field);

        if (dmi_field_choice_find(choices, offset) != nullptr)
            continue;

        assert(choices->count < DMI_FIELD_CHOICE_MAX);

        if (choices->count < DMI_FIELD_CHOICE_MAX) {
            choices->items[choices->count++] = (dmi_field_choice_t){
                .offset   = offset,
                .when_raw = field->params.when_raw
            };
        }
    }
}

dmi_field_choice_t *dmi_field_choice_find(dmi_field_choices_t *choices, size_t offset)
{
    for (unsigned i = 0; i < choices->count; i++) {
        if (choices->items[i].offset == offset)
            return &choices->items[i];
    }

    return nullptr;
}

bool dmi_field_check_offset(const dmi_entity_t *entity, size_t position, const dmi_field_t *field)
{
    if (position == field->params.offset)
        return true;

    dmi_error_raise_ex(dmi_entity_context(entity), DMI_ERROR_INTERNAL,
                       "0x%04hx (%s): fields reach offset 0x%02zX, 0x%02zX declared",
                       dmi_entity_handle(entity), entity->spec->code,
                       position, field->params.offset);

    return false;
}

bool dmi_field_apply(const dmi_field_t *field, const dmi_field_data_t *data, void *value)
{
    assert(field != nullptr);
    assert(data != nullptr);

    if (value == nullptr)
        return true;

    // Unknown is the largest value of the width of the member, which is wider
    // than the field whenever the field has a value standing for it
    if ((field->params.unknown_raw != 0) and (data->number == field->params.unknown_raw) and
        dmi_field_is_numeric(field))
        return dmi_field_store(field, value, UINTMAX_MAX);

    if (field->params.decode != nullptr)
        return field->params.decode(field, data, value);

    switch (field->type) {
    case DMI_FIELD_TYPE_STRING:
        *(const char **)value = data->string;
        return true;

    case DMI_FIELD_TYPE_BINARY:
        *(dmi_binary_t *)value = data->binary;
        return true;

    default:
        return dmi_field_store(field, value, data->number);
    }
}

bool dmi_field_store(const dmi_field_t *field, void *value, uintmax_t raw)
{
    assert(field != nullptr);

    return dmi_field_store_member(field->member, value, raw);
}

bool dmi_field_store_member(dmi_member_ref_t member, void *value, uintmax_t raw)
{
    if (value == nullptr)
        return true;

    switch (member.size) {
    case sizeof(uint8_t):
        *(uint8_t *)value = (uint8_t)raw;
        break;

    case sizeof(uint16_t):
        *(uint16_t *)value = (uint16_t)raw;
        break;

    case sizeof(uint32_t):
        *(uint32_t *)value = (uint32_t)raw;
        break;

    case sizeof(uint64_t):
        *(uint64_t *)value = (uint64_t)raw;
        break;

    default:
        return dmi_trace_argument_invalid(nullptr, member);
    }

    return true;
}

void dmi_fields_release(dmi_entity_t *entity)
{
    if ((entity == nullptr) or (entity->info == nullptr))
        return;

    const dmi_entity_spec_t *spec = entity->spec;

    if ((spec == nullptr) or (spec->fields == nullptr))
        return;

    dmi_field_release_list(spec->fields, entity->info);
}

void dmi_field_release_list(const dmi_field_t *fields, dmi_data_t *info)
{
    if ((fields == nullptr) or (info == nullptr))
        return;

    for (const dmi_field_t *field = fields; field->type != DMI_FIELD_TYPE_NONE; field++) {
        switch (field->type) {
        case DMI_FIELD_TYPE_ARRAY:
            dmi_field_release_array(field, info);
            break;

        case DMI_FIELD_TYPE_VECTOR:
            for (size_t i = 0; i < field->params.count; i++)
                dmi_field_release_list(field->params.fields,
                                       info + field->member.offset + (i * field->member.size));
            break;

        case DMI_FIELD_TYPE_STRUCT:
            dmi_field_release_list(field->params.fields, info + field->member.offset);
            break;

        default:
            break;
        }
    }
}

uintmax_t dmi_field_load_member(dmi_member_ref_t member, const void *value)
{
    switch (member.size) {
    case sizeof(uint8_t):
        return dmi_deref(uint8_t,  value);
    case sizeof(uint16_t):
        return dmi_deref(uint16_t, value);
    case sizeof(uint32_t):
        return dmi_deref(uint32_t, value);
    case sizeof(uint64_t):
        return dmi_deref(uint64_t, value);
    default:
        return dmi_trace_argument_invalid(nullptr, member, 0);
    }
}

static void dmi_field_release_array(const dmi_field_t *field, dmi_data_t *info)
{
    dmi_data_t **elements = (dmi_data_t **)(info + field->member.offset);

    if (*elements == nullptr)
        return;

    size_t *counter = (size_t *)(info + field->params.counter.offset);

    for (size_t i = 0; i < *counter; i++)
        dmi_field_release_list(field->params.fields, *elements + (i * field->member.size));

    dmi_free(*elements);

    *elements = nullptr;
    *counter  = 0;
}
