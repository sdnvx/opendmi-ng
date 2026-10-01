//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/error.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/utils/base64.h>

#include <opendmi/format/iter.h>
#include <opendmi/format/json/handlers.h>
#include <opendmi/format/json/helpers.h>

/**
 * @internal
 * @brief Write the label of a member, which is the code of its attribute.
 */
static dmi_attribute_walk_t dmi_json_attr_member(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief Begin the mapping a nested structure is written as.
 */
static dmi_attribute_walk_t dmi_json_attr_struct_start(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief End the mapping a nested structure is written as.
 */
static bool dmi_json_attr_struct_end(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief Begin the sequence an array is written as.
 */
static dmi_attribute_walk_t dmi_json_attr_array_start(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief End the sequence an array is written as.
 */
static bool dmi_json_attr_array_end(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief Write the value of a member or of an element of an array.
 */
static bool dmi_json_attr_value(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief Write a value as a scalar: null if it is unspecified, the word
 * `unknown` if it is unknown, a mapping of the flags for a set, and the
 * formatted value otherwise.
 */
static bool dmi_json_entity_attr_value(
        dmi_json_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value);

/**
 * @internal
 * @brief Write the flags of a set as a mapping of their codes to booleans.
 */
static bool dmi_json_entity_attr_set(
        dmi_json_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value);

/**
 * @internal
 * @brief Callbacks writing the attributes of a structure.
 */
static const dmi_attribute_visitor_t dmi_json_attr_visitor = {
    .member_start = dmi_json_attr_member,
    .struct_start = dmi_json_attr_struct_start,
    .struct_end   = dmi_json_attr_struct_end,
    .array_start  = dmi_json_attr_array_start,
    .array_end    = dmi_json_attr_array_end,
    .value        = dmi_json_attr_value
};

void *dmi_json_initialize(dmi_context_t *context, FILE *stream, const dmi_format_options_t *options)
{
    assert(context != nullptr);
    assert(stream != nullptr);

    bool success = false;
    dmi_json_session_t *session;

    session = dmi_alloc(context, sizeof(*session));
    if (session == nullptr)
        return nullptr;

    do {
        session->generator = yajl_gen_alloc(nullptr);
        if (session->generator == nullptr) {
            dmi_error_raise_ex(context, DMI_ERROR_INTERNAL, "Unable to create JSON generator");
            break;
        }

        // Strings are repaired before output, and validation guarantees that
        // no invalid UTF-8 is written anyway
        bool configured =
            yajl_gen_config(session->generator, yajl_gen_beautify, 1) and
            yajl_gen_config(session->generator, yajl_gen_validate_utf8, 1);

        if (not configured) {
            dmi_error_raise_ex(context, DMI_ERROR_INTERNAL, "Unable to configure JSON generator");
            break;
        }

        success = true;
    } while (false);

    if (not success) {
        if (session->generator != nullptr)
            yajl_gen_free(session->generator);
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

bool dmi_json_dump_start(dmi_json_session_t *session)
{
    assert(session != nullptr);

    return dmi_json_mapping_start(session);
}

bool dmi_json_entry(dmi_json_session_t *session)
{
    bool result;
    char *smbios_version;

    assert(session != nullptr);

    smbios_version = dmi_version_format(session->context->state.smbios_version);
    if (smbios_version == nullptr) {
        dmi_error_raise(session->context, DMI_ERROR_OUT_OF_MEMORY);
        return false;
    }

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
    char *entity_level = nullptr;
    const char *entity_description;

    assert(session != nullptr);
    assert(entity != nullptr);

    if (entity->level != DMI_VERSION_NONE) {
        entity_level = dmi_version_format(entity->level);
        if (entity_level == nullptr) {
            dmi_error_raise(session->context, DMI_ERROR_OUT_OF_MEMORY);
            return false;
        }
    }

    entity_description = dmi_entity_name(entity);

    result =
        dmi_json_mapping_start(session) and
        dmi_json_label(session, "handle") and
        dmi_json_scalar(session, (int)entity->handle) and
        dmi_json_label(session, "type") and
        dmi_json_scalar(session, entity->type_id) and
        dmi_json_label(session, "length") and
        dmi_json_scalar(session, entity->total_length) and
        dmi_json_label(session, "level") and
        ((entity_level != nullptr) ?
            dmi_json_scalar(session, entity_level) :
            dmi_json_scalar_null(session)) and
        dmi_json_label(session, "state") and
        dmi_json_sequence_start(session);

    dmi_free(entity_level);

    if (not result)
        return false;

    dmi_format_set_iter_t iter;
    const dmi_format_flag_t *flag;

    dmi_format_mask_iter_init(&iter, &dmi_entity_state_names, entity->state,
                              sizeof(entity->state) * CHAR_BIT);

    while ((flag = dmi_format_set_iter_next(&iter)) != nullptr) {
        if (flag->value and not dmi_json_scalar(session, flag->code))
            return false;
    }

    return
        dmi_json_sequence_end(session) and
        dmi_json_label(session, "description") and
        dmi_json_scalar(session, entity_description);
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

    dmi_unused(value);

    // Nested structures are written as nested mappings, and arrays as
    // sequences, whose counters are members of the same structure
    return dmi_attribute_walk(attr, entity->info, &dmi_json_attr_visitor, session);
}

static dmi_attribute_walk_t dmi_json_attr_member(void *context, const dmi_attribute_node_t *node)
{
    return dmi_format_walk(dmi_json_label(context, node->member->params.code));
}

static dmi_attribute_walk_t dmi_json_attr_struct_start(void *context, const dmi_attribute_node_t *node)
{
    dmi_unused(node);

    return dmi_format_walk(dmi_json_mapping_start(context));
}

static bool dmi_json_attr_struct_end(void *context, const dmi_attribute_node_t *node)
{
    dmi_unused(node);

    return dmi_json_mapping_end(context);
}

static dmi_attribute_walk_t dmi_json_attr_array_start(void *context, const dmi_attribute_node_t *node)
{
    dmi_unused(node);

    return dmi_format_walk(dmi_json_sequence_start(context));
}

static bool dmi_json_attr_array_end(void *context, const dmi_attribute_node_t *node)
{
    dmi_unused(node);

    return dmi_json_sequence_end(context);
}

static bool dmi_json_attr_value(void *context, const dmi_attribute_node_t *node)
{
    return dmi_json_entity_attr_value(context, node->attr, node->value);
}

static bool dmi_json_entity_attr_value(
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
        text = dmi_format_attribute_value(session->context, attr, value,
                                          session->options.pretty);
        if (text == nullptr)
            break;

        // Values formatted for a person are text, whatever they hold, and so
        // are the values, which are not canonical numbers or booleans
        dmi_format_scalar_t scalar = DMI_FORMAT_SCALAR_STRING;
        if (not session->options.pretty)
            scalar = dmi_format_scalar_classify(attr, text);

        bool result;

        switch (scalar) {
        case DMI_FORMAT_SCALAR_BOOL:
            result = dmi_json_scalar_bool(session, strcmp(text, "true") == 0);
            break;

        case DMI_FORMAT_SCALAR_NUMBER:
            result = dmi_json_scalar_number(session, text);
            break;

        default:
            result = dmi_json_scalar_str(session, text);
            break;
        }

        if (not result)
            break;

        success = true;
    } while (false);

    dmi_free(text);

    return success;
}

static bool dmi_json_entity_attr_set(
        dmi_json_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value)
{
    assert(session != nullptr);
    assert(attr != nullptr);
    assert(value != nullptr);

    dmi_format_set_iter_t iter;
    const dmi_format_flag_t *flag;

    if (not dmi_json_mapping_start(session))
        return false;

    dmi_format_set_iter_init(&iter, attr, value);

    while ((flag = dmi_format_set_iter_next(&iter)) != nullptr) {
        bool result =
            dmi_json_label(session, flag->code) and
            dmi_json_scalar(session, flag->value);

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

bool dmi_json_entity_properties(dmi_json_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    dmi_format_property_iter_t iter;
    const dmi_string_property_t *property;

    bool result =
        dmi_json_label(session, "properties") and
        dmi_json_sequence_start(session);
    if (not result)
        return false;

    dmi_format_property_iter_init(&iter, entity);

    while ((property = dmi_format_property_iter_next(&iter)) != nullptr) {
        const char *code = dmi_code_lookup(&dmi_property_names, property->ident);

        result =
            dmi_json_mapping_start(session) and
            dmi_json_label(session, "id") and
            dmi_json_scalar(session, (int)property->ident) and
            dmi_json_label(session, "code") and
            ((code != nullptr) ?
                dmi_json_scalar(session, code) :
                dmi_json_scalar_null(session)) and
            dmi_json_label(session, "value") and
            ((property->value != nullptr) ?
                dmi_json_scalar(session, property->value) :
                dmi_json_scalar_null(session)) and
            dmi_json_mapping_end(session);

        if (not result)
            return false;
    }

    return dmi_json_sequence_end(session);
}

bool dmi_json_entity_overlays(dmi_json_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    bool result =
        dmi_json_label(session, "overlays") and
        dmi_json_sequence_start(session);
    if (not result)
        return false;

    for (const dmi_entity_overlay_t *overlay = entity->overlays; overlay != nullptr; overlay = overlay->next) {
        const char *string = overlay->entry->string;

        char *value = dmi_format_overlay_value(entity, overlay, session->options.pretty);
        if (value == nullptr)
            return false;

        result =
            dmi_json_mapping_start(session) and
            dmi_json_label(session, "source") and
            dmi_json_scalar(session, (int)overlay->source->handle) and
            dmi_json_label(session, "index") and
            dmi_json_scalar(session, overlay->index) and
            dmi_json_label(session, "offset") and
            dmi_json_scalar(session, (int)overlay->entry->ref_offset) and
            dmi_json_label(session, "value") and
            dmi_json_scalar(session, value) and
            dmi_json_label(session, "string") and
            ((string != nullptr) ?
                dmi_json_scalar(session, string) :
                dmi_json_scalar_null(session)) and
            dmi_json_mapping_end(session);

        dmi_free(value);

        if (not result)
            return false;
    }

    return dmi_json_sequence_end(session);
}

bool dmi_json_entity_data(dmi_json_session_t *session, const dmi_entity_t *entity)
{
    bool result;
    char *data;

    data = dmi_base64_encode(dmi_entity_data(entity, DMI_TYPE_ANY), entity->body_length, nullptr);
    if (data == nullptr) {
        dmi_error_raise(session->context, DMI_ERROR_OUT_OF_MEMORY);
        return false;
    }

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

    dmi_format_string_iter_t iter;
    const char *str;

    dmi_format_string_iter_init(&iter, entity);

    while ((str = dmi_format_string_iter_next(&iter)) != nullptr) {
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

    if (not dmi_json_mapping_end(session))
        return false;

    if (yajl_gen_get_buf(session->generator, (const unsigned char **)&buffer, &length) != yajl_gen_status_ok) {
        dmi_error_raise_ex(session->context, DMI_ERROR_INTERNAL, "Unable to get JSON document");
        return false;
    }

    if ((fwrite(buffer, 1, length, session->stream) < length) or (fflush(session->stream) != 0)) {
        dmi_error_raise_ex(session->context, DMI_ERROR_FILE_WRITE, "%s", strerror(errno));
        return false;
    }

    return true;
}

void dmi_json_finalize(dmi_json_session_t *session)
{
    assert(session != nullptr);

    yajl_gen_free(session->generator);
    dmi_free(session);
}
