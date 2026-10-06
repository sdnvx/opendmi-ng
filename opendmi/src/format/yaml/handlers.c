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

/**
 * @internal
 * @brief Write the label of a member, which is the code of its attribute.
 */
static dmi_attribute_walk_t dmi_yaml_attr_member(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief Begin the mapping a nested structure is written as.
 */
static dmi_attribute_walk_t dmi_yaml_attr_struct_start(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief End the mapping a nested structure is written as.
 */
static bool dmi_yaml_attr_struct_end(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief Begin the sequence an array is written as.
 */
static dmi_attribute_walk_t dmi_yaml_attr_array_start(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief End the sequence an array is written as.
 */
static bool dmi_yaml_attr_array_end(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief Write the value of a member or of an element of an array.
 */
static bool dmi_yaml_attr_value(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief Write a value as a scalar: null if it is unspecified, the word
 * `unknown` if it is unknown, a mapping of the flags for a set, and the
 * formatted value otherwise.
 */
static bool dmi_yaml_entity_attr_value(
        dmi_yaml_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value);

/**
 * @internal
 * @brief Write the flags of a set as a mapping of their codes to booleans.
 */
static bool dmi_yaml_entity_attr_set(
        dmi_yaml_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value);

/**
 * @internal
 * @brief Callbacks writing the attributes of a structure.
 */
static const dmi_attribute_visitor_t dmi_yaml_attr_visitor = {
    .member_start = dmi_yaml_attr_member,
    .struct_start = dmi_yaml_attr_struct_start,
    .struct_end   = dmi_yaml_attr_struct_end,
    .array_start  = dmi_yaml_attr_array_start,
    .array_end    = dmi_yaml_attr_array_end,
    .value        = dmi_yaml_attr_value
};

void *dmi_yaml_initialize(dmi_context_t *context, FILE *stream, const dmi_format_options_t *options)
{
    assert(context != nullptr);
    assert(stream != nullptr);

    bool success = false;
    bool initialized = false;
    dmi_yaml_session_t *session;

    session = dmi_alloc(context, sizeof(*session));
    if (session == nullptr)
        return nullptr;

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

    if (not yaml_document_start_event_initialize(&event, nullptr, nullptr, nullptr, true)) {
        dmi_error_raise_ex(session->context, DMI_ERROR_INTERNAL,
                           "Unable to initialize YAML document start event");
        return false;
    }

    return
        dmi_yaml_emit(session, &event) and
        dmi_yaml_mapping_start(session, YAML_BLOCK_MAPPING_STYLE);
}

bool dmi_yaml_entry(dmi_yaml_session_t *session)
{
    char *smbios_version;

    assert(session != nullptr);

    smbios_version = dmi_version_format(session->context->state.smbios_version);
    if (smbios_version == nullptr)
        return dmi_trace_out_of_memory(session->context);

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
    snprintf(entity_type, sizeof(entity_type), "%d", entity->type_id);
    snprintf(entity_length, sizeof(entity_length), "%zu", entity->total_length);

    if (entity->level != DMI_VERSION_NONE) {
        entity_level = dmi_version_format(entity->level);
        if (entity_level == nullptr)
            return dmi_trace_out_of_memory(session->context);
    }

    entity_description = dmi_entity_name(entity);

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

    dmi_format_mask_iter_initialize(&iter, &dmi_entity_state_names, entity->state,
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

    dmi_unused(value);

    // Nested structures are written as nested mappings, and arrays as
    // sequences, whose counters are members of the same structure
    return dmi_attribute_walk(attr, entity->info, &dmi_yaml_attr_visitor, session);
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

    dmi_format_property_iter_initialize(&iter, entity);

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

bool dmi_yaml_entity_overlays(dmi_yaml_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    bool result =
        dmi_yaml_label(session, "overlays") and
        dmi_yaml_sequence_start(session, YAML_BLOCK_SEQUENCE_STYLE);
    if (not result)
        return false;

    for (const dmi_entity_overlay_t *overlay = entity->overlays; overlay != nullptr; overlay = overlay->next) {
        const char *string = overlay->entry->string;
        char source[8];
        char index[24];
        char offset[8];

        snprintf(source, sizeof(source), "0x%04hx", overlay->source->handle);
        snprintf(index, sizeof(index), "%zu", overlay->index);
        snprintf(offset, sizeof(offset), "0x%02x", overlay->entry->referenced_offset);

        char *value = dmi_format_overlay_value(entity, overlay, session->options.pretty);
        if (value == nullptr)
            return false;

        // Handle, index and offset are written as plain numbers
        result =
            dmi_yaml_mapping_start(session, YAML_BLOCK_MAPPING_STYLE) and
            dmi_yaml_label(session, "source") and
            dmi_yaml_scalar(session, source, nullptr, YAML_PLAIN_SCALAR_STYLE) and
            dmi_yaml_label(session, "index") and
            dmi_yaml_scalar(session, index, nullptr, YAML_PLAIN_SCALAR_STYLE) and
            dmi_yaml_label(session, "offset") and
            dmi_yaml_scalar(session, offset, nullptr, YAML_PLAIN_SCALAR_STYLE) and
            dmi_yaml_label(session, "value") and
            dmi_yaml_scalar(session, value, YAML_STR_TAG, YAML_DOUBLE_QUOTED_SCALAR_STYLE) and
            dmi_yaml_label(session, "string") and
            ((string != nullptr) ?
                dmi_yaml_scalar(session, string, YAML_STR_TAG, YAML_DOUBLE_QUOTED_SCALAR_STYLE) :
                dmi_yaml_scalar(session, "null", YAML_NULL_TAG, YAML_PLAIN_SCALAR_STYLE)) and
            dmi_yaml_mapping_end(session);

        dmi_free(value);

        if (not result)
            return false;
    }

    return dmi_yaml_sequence_end(session);
}

bool dmi_yaml_entity_data(dmi_yaml_session_t *session, const dmi_entity_t *entity)
{
    bool result;
    char *data;

    data = dmi_base64_encode(dmi_entity_data(entity, DMI_TYPE_ANY), entity->body_length, nullptr);
    if (data == nullptr)
        return dmi_trace_out_of_memory(session->context);

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

    dmi_format_string_iter_initialize(&iter, entity);

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
    yaml_event_t event = {};

    assert(session != nullptr);

    if (not dmi_yaml_mapping_end(session))
        return false;

    if (not yaml_document_end_event_initialize(&event, true)) {
        dmi_error_raise_ex(session->context, DMI_ERROR_INTERNAL,
                           "Unable to initialize YAML document end event");
        return false;
    }

    return
        dmi_yaml_emit(session, &event) and
        dmi_yaml_flush(session);
}

void dmi_yaml_finalize(dmi_yaml_session_t *session)
{
    assert(session != nullptr);

    yaml_emitter_close(session->emitter);
    yaml_emitter_delete(session->emitter);

    dmi_free(session->emitter);
    dmi_free(session);
}

static dmi_attribute_walk_t dmi_yaml_attr_member(void *context, const dmi_attribute_node_t *node)
{
    return dmi_format_walk(dmi_yaml_label(context, node->member->params.code));
}

static dmi_attribute_walk_t dmi_yaml_attr_struct_start(void *context, const dmi_attribute_node_t *node)
{
    dmi_unused(node);

    return dmi_format_walk(dmi_yaml_mapping_start(context, YAML_BLOCK_MAPPING_STYLE));
}

static bool dmi_yaml_attr_struct_end(void *context, const dmi_attribute_node_t *node)
{
    dmi_unused(node);

    return dmi_yaml_mapping_end(context);
}

static dmi_attribute_walk_t dmi_yaml_attr_array_start(void *context, const dmi_attribute_node_t *node)
{
    dmi_unused(node);

    return dmi_format_walk(dmi_yaml_sequence_start(context, YAML_BLOCK_SEQUENCE_STYLE));
}

static bool dmi_yaml_attr_array_end(void *context, const dmi_attribute_node_t *node)
{
    dmi_unused(node);

    return dmi_yaml_sequence_end(context);
}

static bool dmi_yaml_attr_value(void *context, const dmi_attribute_node_t *node)
{
    return dmi_yaml_entity_attr_value(context, node->attr, node->value);
}

static bool dmi_yaml_entity_attr_value(
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

        text = dmi_format_attribute_value(session->context, attr, value,
                                          session->options.pretty);
        if (text == nullptr)
            break;

        // Values formatted for a person are text, whatever they hold: a size
        // carries its unit, and a boolean reads as a word a reader would
        // resolve as a value of its own type
        if (session->options.pretty) {
            if (not dmi_yaml_scalar(session, text, YAML_STR_TAG,
                                    YAML_DOUBLE_QUOTED_SCALAR_STYLE))
                break;

            success = true;
            break;
        }

        // Only canonical numbers and booleans are written as plain scalars,
        // since other values (e.g. dates, versions, enumeration codes, or
        // booleans named by codes like "no") could be resolved by readers as
        // values of different types
        if (dmi_format_scalar_classify(attr, text) != DMI_FORMAT_SCALAR_STRING) {
            tag   = nullptr;
            style = YAML_PLAIN_SCALAR_STYLE;
        } else {
            tag   = YAML_STR_TAG;
            style = YAML_DOUBLE_QUOTED_SCALAR_STYLE;
        }

        if (not dmi_yaml_scalar(session, text, tag, style))
            break;

        success = true;
    } while (false);

    dmi_free(text);

    return success;
}

static bool dmi_yaml_entity_attr_set(
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

    dmi_format_set_iter_initialize(&iter, attr, value);

    while ((flag = dmi_format_set_iter_next(&iter)) != nullptr) {
        bool result =
            dmi_yaml_label(session, flag->code) and
            dmi_yaml_scalar(session, flag->value ? "true" : "false", YAML_BOOL_TAG, YAML_PLAIN_SCALAR_STYLE);

        if (not result)
            return false;
    }

    return dmi_yaml_mapping_end(session);
}
