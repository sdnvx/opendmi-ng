//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/error.h>
#include <opendmi/utils.h>
#include <opendmi/utils/base64.h>

#include <opendmi/format/json/handlers.h>
#include <opendmi/format/json/helpers.h>

void *dmi_json_initialize(dmi_context_t *context, FILE *stream)
{
    assert(context != nullptr);
    assert(stream != nullptr);

    bool success = false;
    dmi_json_session_t *session;

    session = dmi_alloc(context, sizeof(*session));
    if (session == nullptr) {
        dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
        return nullptr;
    }

    do {
        session->generator = yajl_gen_alloc(nullptr);
        if (session->generator == nullptr) {
            dmi_error_raise_ex(context, DMI_ERROR_INTERNAL, "Unable to create JSON generator");
            break;
        }

        if (not yajl_gen_config(session->generator, yajl_gen_beautify, 1)) {
            dmi_error_raise_ex(context, DMI_ERROR_INTERNAL, "Unable to configure JSON generator");
            break;
        }

        success = true;
    } while (false);

    if (not success) {
        if (session->generator != nullptr)
            yajl_gen_free(session->generator);
        return nullptr;
    }

    session->context = context;
    session->stream  = stream;

    return session;
}

bool dmi_json_dump_start(dmi_json_session_t *session)
{
    bool result;
    assert(session != nullptr);

    result = dmi_json_mapping_start(session);
    if (not result)
        dmi_error_raise_ex(session->context, DMI_ERROR_INTERNAL, "Unable to start JSON document");

    return result;
}

bool dmi_json_entry(dmi_json_session_t *session)
{
    bool result;
    char *smbios_version;

    assert(session != nullptr);

    smbios_version = dmi_version_format(session->context->state.smbios_version);
    if (smbios_version == nullptr)
        return false;

    result =
        dmi_json_label(session, "entry") and
        dmi_json_mapping_start(session) and
        dmi_json_label(session, "smbios-version") and
        dmi_json_scalar_str(session, smbios_version) and
        dmi_json_mapping_end(session);

    dmi_free(smbios_version);

    return result;
}

bool dmi_json_table_start(dmi_json_session_t *session)
{
    bool result;

    assert(session != nullptr);

    result =
        dmi_json_label(session, "table") and
        dmi_json_sequence_start(session);

    return result;
}

bool dmi_json_entity_start(dmi_json_session_t *session, const dmi_entity_t *entity)
{
    bool result;
    char entity_handle[8];
    char *entity_level;
    const char *entity_description;

    assert(session != nullptr);
    assert(entity != nullptr);

    snprintf(entity_handle, sizeof(entity_handle), "0x%04hx", entity->handle);

    entity_level = dmi_version_format(entity->level);
    if (entity_level == nullptr)
        return false;

    entity_description = dmi_type_name(session->context, entity->type);

    result =
        dmi_json_mapping_start(session) and
        dmi_json_label(session, "handle") and
        dmi_json_scalar(session, entity_handle) and
        dmi_json_label(session, "type") and
        dmi_json_scalar(session, entity->type) and
        dmi_json_label(session, "length") and
        dmi_json_scalar(session, entity->total_length) and
        dmi_json_label(session, "level") and
        dmi_json_scalar(session, entity_level) and
        dmi_json_label(session, "description") and
        dmi_json_scalar(session, entity_description);

    dmi_free(entity_level);

    return result;
}

bool dmi_json_entity_attrs_start(dmi_json_session_t *session, const dmi_entity_t *entity)
{
    bool result;

    assert(session != nullptr);
    assert(entity != nullptr);

    dmi_unused(entity);

    result =
        dmi_json_label(session, "attributes") and
        dmi_json_mapping_start(session);

    return result;
}

bool dmi_json_entity_attr(
        dmi_json_session_t    *session,
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attr,
        const void            *value)
{
    assert(session != nullptr);
    assert(entity != nullptr);
    assert(attr != nullptr);
    assert(value != nullptr);

    bool success = false;

    do {
        bool result;

        if (not dmi_json_label(session, attr->params.code))
            break;

        if (not dmi_member_is_present(attr->counter)) {
            if (attr->type == DMI_ATTRIBUTE_TYPE_STRUCT)
                result = dmi_json_entity_attr_struct(session, attr, value);
            else
                result = dmi_json_entity_attr_value(session, attr, value);
        } else {
            result = dmi_json_entity_attr_array(session, attr, entity->info, value);
        }

        if (not result)
            break;

        success = true;
    } while (false);

    return success;
}

bool dmi_json_entity_attr_array(
        dmi_json_session_t    *session,
        const dmi_attribute_t *attr,
        const dmi_data_t      *info,
        const void            *value)
{
    assert(session != nullptr);
    assert(attr != nullptr);
    assert(info != nullptr);
    assert(value != nullptr);

    // TODO: Support other types of counters
    size_t count = dmi_member_value(info, attr->counter, size_t);
    const dmi_data_t *ptr = *(const dmi_data_t **)value;

    if (not dmi_json_sequence_start(session))
        return false;

    for (size_t i = 0; i < count; i++, ptr += attr->value.size) {
        bool result;

        if (attr->type == DMI_ATTRIBUTE_TYPE_STRUCT)
            result = dmi_json_entity_attr_struct(session, attr, ptr);
        else
            result = dmi_json_entity_attr_value(session, attr, ptr);

        if (not result)
            return false;
    }

    if (not dmi_json_sequence_end(session))
        return false;

    return true;
}

bool dmi_json_entity_attr_struct(
        dmi_json_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value)
{
    assert(session != nullptr);
    assert(attr != nullptr);
    assert(value != nullptr);

    const dmi_attribute_t *child_attr = nullptr;

    if (not dmi_json_mapping_start(session))
        return false;

    for (child_attr = attr->params.attrs; child_attr->params.name; child_attr++) {
        const dmi_data_t *ptr = dmi_member_ptr(value, child_attr->value, dmi_data_t);

        bool result =
            dmi_json_label(session, child_attr->params.code) and
            dmi_json_entity_attr_value(session, child_attr, ptr);

        if (not result)
            return false;
    }

    if (not dmi_json_mapping_end(session))
        return false;

    return true;
}

bool dmi_json_entity_attr_value(
        dmi_json_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value)
{
    assert(session != nullptr);
    assert(attr != nullptr);
    assert(value != nullptr);

    bool success = false;
    char *text = nullptr;

    // Write empty tag if the value is unspecified
    if (dmi_attribute_is_unspecified(attr, value))
        return dmi_json_scalar_null(session);

    // Handle unknown values
    if (dmi_attribute_is_unknown(attr, value))
        return dmi_json_scalar(session, "unknown");

    // Handle value sets
    if (attr->type == DMI_ATTRIBUTE_TYPE_SET)
        return dmi_json_entity_attr_set(session, attr, value);

    do {
        text = dmi_attribute_format(session->context, attr, value, false);
        if (text == nullptr)
            break;

        if (not dmi_json_scalar(session, text))
            break;

        success = true;
    } while (false);

    dmi_free(text);

    return success;
}

bool dmi_json_entity_attr_set(
        dmi_json_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value)
{
    assert(session != nullptr);
    assert(attr != nullptr);
    assert(value != nullptr);

    uintmax_t mask = dmi_attribute_get_uint(attr, value);

    if (not dmi_json_mapping_start(session))
        return false;

    for (size_t i = 0; i < attr->value.size * CHAR_BIT; i++) {
        const char *name = dmi_code_lookup(attr->params.values, i);
        if (name == nullptr)
            continue;

        bool flag = mask & (1 << i);
        bool result =
            dmi_json_label(session, name) and
            dmi_json_scalar(session, flag);

        if (not result)
            return false;
    }

    if (not dmi_json_mapping_end(session))
        return false;

    return true;
}

bool dmi_json_entity_attrs_end(dmi_json_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);

    dmi_unused(entity);

    return dmi_json_mapping_end(session);
}

bool dmi_json_entity_data(dmi_json_session_t *session, const dmi_entity_t *entity)
{
    bool result;
    char *data;

    data = dmi_base64_encode(entity->data, entity->body_length, nullptr);
    if (data == nullptr)
        return false;

    result =
        dmi_json_label(session, "data") and
        dmi_json_scalar(session, data);

    dmi_free(data);

    return result;
}

bool dmi_json_entity_strings(dmi_json_session_t *session, const dmi_entity_t *entity)
{
    bool result;

    if (entity->string_count == 0)
        return true;

    result =
        dmi_json_label(session, "strings") and
        dmi_json_sequence_start(session);
    if (not result)
        return false;

    for (dmi_string_t i = 1; i <= entity->string_count; i++) {
        const char *str = dmi_entity_string_ex(entity, i, true);
        if (not dmi_json_scalar(session, str))
            return false;
    }

    if (not dmi_json_sequence_end(session))
        return false;

    return true;
}

bool dmi_json_entity_end(dmi_json_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);

    dmi_unused(entity);

    return dmi_json_mapping_end(session);
}

bool dmi_json_table_end(dmi_json_session_t *session)
{
    assert(session != nullptr);

    return dmi_json_sequence_end(session);
}

bool dmi_json_dump_end(dmi_json_session_t *session)
{
    const char *buffer;
    size_t length;

    assert(session != nullptr);

    if (not dmi_json_mapping_end(session)) {
        dmi_error_raise_ex(session->context, DMI_ERROR_INTERNAL, "Unable to end JSON document");
        return false;
    }

    yajl_gen_get_buf(session->generator, (const unsigned char **)&buffer, &length);
    if (fwrite(buffer, length, 1, session->stream) < 1) {
        dmi_error_raise_ex(session->context, DMI_ERROR_INTERNAL, "Unable to write JSON document");
        return false;
    }

    fflush(session->stream);

    return true;
}

void dmi_json_finalize(dmi_json_session_t *session)
{
    assert(session != nullptr);

    yajl_gen_free(session->generator);
}
