//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <limits.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/error.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/base64.h>

#include <opendmi/format/iter.h>
#include <opendmi/format/yaml/handlers.h>
#include <opendmi/format/yaml/helpers.h>

void *dmi_yaml_initialize(dmi_context_t *context, FILE *stream, const dmi_format_options_t *options)
{
    assert(context != nullptr);
    assert(stream != nullptr);

    bool success = false;
    bool initialized = false;
    dmi_yaml_session_t *session;

    session = dmi_alloc(context, sizeof(*session));
    if (session == nullptr) {
        dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
        return nullptr;
    }

    do {
        session->emitter = dmi_alloc(context, sizeof(*session->emitter));
        if (session->emitter == nullptr)
            break;

        if (not yaml_emitter_initialize(session->emitter)) {
            dmi_error_raise_ex(context, DMI_ERROR_INTERNAL, "Unable to initialize YAML emitter");
            break;
        }
        initialized = true;

        yaml_emitter_set_output_file(session->emitter, stream);
        yaml_emitter_set_encoding(session->emitter, YAML_UTF8_ENCODING);
        yaml_emitter_set_unicode(session->emitter, true);
        yaml_emitter_set_canonical(session->emitter, false);
        yaml_emitter_set_indent(session->emitter, 2);

        if (not yaml_emitter_open(session->emitter)) {
            dmi_error_raise_ex(context, DMI_ERROR_INTERNAL, "Unable to open YAML emitter");
            break;
        }

        success = true;
    } while (false);

    if (not success) {
        if (initialized)
            yaml_emitter_delete(session->emitter);
        dmi_free(session->emitter);
        dmi_free(session);

        return nullptr;
    }

    session->context = context;
    session->stream  = stream;

    // Default options are used if not specified
    if (options != nullptr)
        session->options = *options;

    return session;
}

bool dmi_yaml_dump_start(dmi_yaml_session_t *session)
{
    yaml_event_t event = {};

    assert(session != nullptr);

    bool result =
        yaml_document_start_event_initialize(&event, nullptr, nullptr, nullptr, true) and
        yaml_emitter_emit(session->emitter, &event) and
        dmi_yaml_mapping_start(session, YAML_BLOCK_MAPPING_STYLE);

    if (not result)
        dmi_error_raise_ex(session->context, DMI_ERROR_INTERNAL, "Unable to start YAML document");

    return result;
}

bool dmi_yaml_entry(dmi_yaml_session_t *session)
{
    char *smbios_version;

    assert(session != nullptr);

    smbios_version = dmi_version_format(session->context->state.smbios_version);
    if (smbios_version == nullptr)
        return false;

    bool result =
        dmi_yaml_label(session, "entry") and
        dmi_yaml_mapping_start(session, YAML_BLOCK_MAPPING_STYLE) and
        dmi_yaml_label(session, "smbios-version") and
        dmi_yaml_scalar(session, smbios_version, YAML_STR_TAG, YAML_SINGLE_QUOTED_SCALAR_STYLE) and
        dmi_yaml_mapping_end(session);

    dmi_free(smbios_version);

    return result;
}

bool dmi_yaml_table_start(dmi_yaml_session_t *session)
{
    assert(session != nullptr);

    return
        dmi_yaml_label(session, "table") and
        dmi_yaml_sequence_start(session, YAML_BLOCK_SEQUENCE_STYLE);
}

bool dmi_yaml_entity_start(dmi_yaml_session_t *session, const dmi_entity_t *entity)
{
    char entity_handle[8];
    char entity_type[8];
    char *entity_level = nullptr;
    const char *entity_description;
    char entity_length[8];

    assert(session != nullptr);
    assert(entity != nullptr);

    snprintf(entity_handle, sizeof(entity_handle), "0x%04hx", entity->handle);
    snprintf(entity_type, sizeof(entity_type), "%d", entity->type);
    snprintf(entity_length, sizeof(entity_length), "%zu", entity->total_length);

    if (entity->level != DMI_VERSION_NONE) {
        entity_level = dmi_version_format(entity->level);
        if (entity_level == nullptr)
            return false;
    }

    entity_description = dmi_type_name(session->context, entity->type);

    bool result =
        dmi_yaml_mapping_start(session, YAML_BLOCK_MAPPING_STYLE) and
        dmi_yaml_label(session, "handle") and
        dmi_yaml_scalar(session, entity_handle, YAML_INT_TAG, YAML_PLAIN_SCALAR_STYLE) and
        dmi_yaml_label(session, "type") and
        dmi_yaml_scalar(session, entity_type, YAML_INT_TAG, YAML_PLAIN_SCALAR_STYLE) and
        dmi_yaml_label(session, "length") and
        dmi_yaml_scalar(session, entity_length, YAML_INT_TAG, YAML_PLAIN_SCALAR_STYLE) and
        dmi_yaml_label(session, "level") and
        (entity_level != nullptr ?
            dmi_yaml_scalar(session, entity_level, YAML_STR_TAG, YAML_SINGLE_QUOTED_SCALAR_STYLE) :
            dmi_yaml_scalar(session, "null", YAML_NULL_TAG, YAML_PLAIN_SCALAR_STYLE)) and
        dmi_yaml_label(session, "state") and
        dmi_yaml_sequence_start(session, YAML_FLOW_SEQUENCE_STYLE);

    dmi_free(entity_level);

    if (not result)
        return false;

    dmi_format_set_iter_t iter;
    const dmi_format_flag_t *flag;

    dmi_format_mask_iter_init(&iter, &dmi_entity_state_names, entity->state,
                              sizeof(entity->state) * CHAR_BIT);

    while ((flag = dmi_format_set_iter_next(&iter)) != nullptr) {
        if (flag->value and not dmi_yaml_scalar(session, flag->code, YAML_STR_TAG, YAML_PLAIN_SCALAR_STYLE))
            return false;
    }

    return
        dmi_yaml_sequence_end(session) and
        dmi_yaml_label(session, "description") and
        dmi_yaml_scalar(session, entity_description, YAML_STR_TAG, YAML_PLAIN_SCALAR_STYLE);
}

bool dmi_yaml_entity_attrs_start(dmi_yaml_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    dmi_unused(entity);

    return
        dmi_yaml_label(session, "attributes") and
        dmi_yaml_mapping_start(session, YAML_BLOCK_MAPPING_STYLE);
}

bool dmi_yaml_entity_attr(
        dmi_yaml_session_t    *session,
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

        if (not dmi_yaml_label(session, attr->params.code))
            break;

        if (not dmi_member_is_present(attr->counter)) {
            if (attr->type == DMI_ATTRIBUTE_TYPE_STRUCT)
                result = dmi_yaml_entity_attr_struct(session, attr, value);
            else
                result = dmi_yaml_entity_attr_value(session, attr, value);
        } else {
            result = dmi_yaml_entity_attr_array(session, attr, entity->info, value);
        }

        if (not result)
            break;

        success = true;
    } while (false);

    return success;
}

bool dmi_yaml_entity_attr_array(
        dmi_yaml_session_t    *session,
        const dmi_attribute_t *attr,
        const dmi_data_t      *info,
        const void            *value)
{
    assert(session != nullptr);
    assert(attr != nullptr);
    assert(info != nullptr);
    assert(value != nullptr);

    dmi_format_array_iter_t iter;
    const dmi_data_t *ptr;

    if (not dmi_yaml_sequence_start(session, YAML_BLOCK_SEQUENCE_STYLE))
        return false;

    dmi_format_array_iter_init(&iter, attr, info, value);

    while ((ptr = dmi_format_array_iter_next(&iter)) != nullptr) {
        bool result;

        if (attr->type == DMI_ATTRIBUTE_TYPE_STRUCT)
            result = dmi_yaml_entity_attr_struct(session, attr, ptr);
        else
            result = dmi_yaml_entity_attr_value(session, attr, ptr);

        if (not result)
            return false;
    }

    return dmi_yaml_sequence_end(session);
}

bool dmi_yaml_entity_attr_struct(
        dmi_yaml_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value)
{
    assert(session != nullptr);
    assert(attr != nullptr);
    assert(value != nullptr);

    const dmi_attribute_t *child_attr = nullptr;

    if (not dmi_yaml_mapping_start(session, YAML_BLOCK_MAPPING_STYLE))
        return false;

    for (child_attr = attr->params.attrs; child_attr->params.name; child_attr++) {
        const dmi_data_t *ptr = dmi_member_ptr(value, child_attr->value, dmi_data_t);

        bool result =
            dmi_yaml_label(session, child_attr->params.code) and
            dmi_yaml_entity_attr_value(session, child_attr, ptr);

        if (not result)
            return false;
    }

    return dmi_yaml_mapping_end(session);
}

bool dmi_yaml_entity_attr_value(
        dmi_yaml_session_t    *session,
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
        return dmi_yaml_scalar(session, "null", YAML_NULL_TAG, YAML_PLAIN_SCALAR_STYLE);

    // Handle unknown values
    if (dmi_attribute_is_unknown(attr, value))
        return dmi_yaml_scalar(session, "unknown", YAML_STR_TAG, YAML_PLAIN_SCALAR_STYLE);

    // Handle value sets
    if (attr->type == DMI_ATTRIBUTE_TYPE_SET)
        return dmi_yaml_entity_attr_set(session, attr, value);

    do {
        const char *tag;
        yaml_scalar_style_t style;

        text = dmi_attribute_format(session->context, attr, value, false);
        if (text == nullptr)
            break;

        // Only numbers and booleans are written as plain scalars, since
        // other values (e.g. dates, versions or enumeration codes) could be
        // resolved by readers as values of different types
        switch (attr->type) {
        case DMI_ATTRIBUTE_TYPE_HANDLE:
        case DMI_ATTRIBUTE_TYPE_BOOL:
        case DMI_ATTRIBUTE_TYPE_INTEGER:
        case DMI_ATTRIBUTE_TYPE_DECIMAL:
        case DMI_ATTRIBUTE_TYPE_SIZE:
        case DMI_ATTRIBUTE_TYPE_ADDRESS:
            tag   = nullptr;
            style = YAML_PLAIN_SCALAR_STYLE;
            break;

        default:
            tag   = YAML_STR_TAG;
            style = YAML_DOUBLE_QUOTED_SCALAR_STYLE;
            break;
        }

        if (not dmi_yaml_scalar(session, text, tag, style))
            break;

        success = true;
    } while (false);

    dmi_free(text);

    return success;
}

bool dmi_yaml_entity_attr_set(
        dmi_yaml_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value)
{
    assert(session != nullptr);
    assert(attr != nullptr);
    assert(value != nullptr);

    dmi_format_set_iter_t iter;
    const dmi_format_flag_t *flag;

    if (not dmi_yaml_mapping_start(session, YAML_BLOCK_MAPPING_STYLE))
        return false;

    dmi_format_set_iter_init(&iter, attr, value);

    while ((flag = dmi_format_set_iter_next(&iter)) != nullptr) {
        bool result =
            dmi_yaml_label(session, flag->code) and
            dmi_yaml_scalar(session, flag->value ? "true" : "false", YAML_BOOL_TAG, YAML_PLAIN_SCALAR_STYLE);

        if (not result)
            return false;
    }

    return dmi_yaml_mapping_end(session);
}

bool dmi_yaml_entity_attrs_end(dmi_yaml_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);

    dmi_unused(entity);

    return dmi_yaml_mapping_end(session);
}

bool dmi_yaml_entity_properties(dmi_yaml_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    dmi_format_property_iter_t iter;
    const dmi_string_property_t *property;

    bool result =
        dmi_yaml_label(session, "properties") and
        dmi_yaml_sequence_start(session, YAML_BLOCK_SEQUENCE_STYLE);
    if (not result)
        return false;

    dmi_format_property_iter_init(&iter, entity);

    while ((property = dmi_format_property_iter_next(&iter)) != nullptr) {
        char id[8];
        const char *code = dmi_code_lookup(&dmi_property_names, property->ident);

        snprintf(id, sizeof(id), "0x%04x", (unsigned)property->ident);

        // Identifier is written as a plain number, like handles
        result =
            dmi_yaml_mapping_start(session, YAML_BLOCK_MAPPING_STYLE) and
            dmi_yaml_label(session, "id") and
            dmi_yaml_scalar(session, id, nullptr, YAML_PLAIN_SCALAR_STYLE) and
            dmi_yaml_label(session, "code") and
            ((code != nullptr) ?
                dmi_yaml_scalar(session, code, YAML_STR_TAG, YAML_DOUBLE_QUOTED_SCALAR_STYLE) :
                dmi_yaml_scalar(session, "null", YAML_NULL_TAG, YAML_PLAIN_SCALAR_STYLE)) and
            dmi_yaml_label(session, "value") and
            ((property->value != nullptr) ?
                dmi_yaml_scalar(session, property->value, YAML_STR_TAG, YAML_DOUBLE_QUOTED_SCALAR_STYLE) :
                dmi_yaml_scalar(session, "null", YAML_NULL_TAG, YAML_PLAIN_SCALAR_STYLE)) and
            dmi_yaml_mapping_end(session);

        if (not result)
            return false;
    }

    return dmi_yaml_sequence_end(session);
}

bool dmi_yaml_entity_data(dmi_yaml_session_t *session, const dmi_entity_t *entity)
{
    bool result;
    char *data;

    data = dmi_base64_encode(entity->data, entity->body_length, nullptr);
    if (data == nullptr)
        return false;

    result =
        dmi_yaml_label(session, "data") and
        dmi_yaml_scalar(session, data, YAML_BINARY_TAG, YAML_LITERAL_SCALAR_STYLE);

    dmi_free(data);

    return result;
}

bool dmi_yaml_entity_strings(dmi_yaml_session_t *session, const dmi_entity_t *entity)
{
    bool result;

    if (entity->string_count == 0)
        return true;

    result =
        dmi_yaml_label(session, "strings") and
        dmi_yaml_sequence_start(session, YAML_BLOCK_SEQUENCE_STYLE);
    if (not result)
        return false;

    dmi_format_string_iter_t iter;
    const char *str;

    dmi_format_string_iter_init(&iter, entity);

    while ((str = dmi_format_string_iter_next(&iter)) != nullptr) {
        if (not dmi_yaml_scalar(session, str, YAML_STR_TAG, YAML_DOUBLE_QUOTED_SCALAR_STYLE))
            return false;
    }

    return dmi_yaml_sequence_end(session);
}

bool dmi_yaml_entity_end(dmi_yaml_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);

    dmi_unused(entity);

    return dmi_yaml_mapping_end(session);
}

bool dmi_yaml_table_end(dmi_yaml_session_t *session)
{
    assert(session != nullptr);

    return dmi_yaml_sequence_end(session);
}

bool dmi_yaml_dump_end(dmi_yaml_session_t *session)
{
    bool result;
    yaml_event_t event = {};

    assert(session != nullptr);

    result =
        dmi_yaml_mapping_end(session) and
        yaml_document_end_event_initialize(&event, true) and
        yaml_emitter_emit(session->emitter, &event) and
        yaml_emitter_flush(session->emitter);

    if (not result)
        dmi_error_raise_ex(session->context, DMI_ERROR_INTERNAL, "Unable to end YAML document");

    return result;
}

void dmi_yaml_finalize(dmi_yaml_session_t *session)
{
    assert(session != nullptr);

    yaml_emitter_close(session->emitter);
    yaml_emitter_delete(session->emitter);

    dmi_free(session->emitter);
    dmi_free(session);
}
