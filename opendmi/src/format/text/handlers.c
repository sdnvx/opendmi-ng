//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <config.h>

#if __has_include(<unistd.h>)
#   include <unistd.h>
#endif

#include <string.h>
#include <limits.h>
#include <inttypes.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/error.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/tty.h>
#include <opendmi/utils/locale.h>

#include <opendmi/format.h>
#include <opendmi/format/iter.h>
#include <opendmi/format/text/handlers.h>
#include <opendmi/format/text/helpers.h>

void *dmi_text_initialize(dmi_context_t *context, FILE *stream, const dmi_format_options_t *options)
{
    assert(context != nullptr);
    assert(stream != nullptr);

    dmi_text_session_t *session;

    session = dmi_alloc(context, sizeof(*session));
    if (session == nullptr) {
        dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
        return nullptr;
    }

    session->context = context;
    session->stream  = stream;

    // Default options are used if not specified
    if (options != nullptr)
        session->options = *options;

    // Colors are available only if terminal has been initialized. Standard
    // output may be already redirected to pager, which shows colors, so its
    // state before redirection is used.
    bool is_terminal = (stream == stdout) ? dmi_tty_is_stdout() : isatty(fileno(stream));

    session->is_tty = dmi_has_tty() and is_terminal;

    return session;
}

// Handle references are hidden in quiet mode
static bool dmi_text_is_hidden(const dmi_text_session_t *session, const dmi_attribute_t *attr)
{
    return (session->options.mode == DMI_FORMAT_MODE_QUIET) and (attr->type == DMI_ATTRIBUTE_TYPE_HANDLE);
}

//
// Print a message of the tool resources, which is translated to the locale,
// and is taken from the fallback pattern if there is no translation.
//
static bool dmi_text_message_color(
        dmi_text_session_t      *session,
        dmi_tty_color_t          color,
        const char              *table,
        const char              *key,
        const char              *fallback,
        const dmi_message_arg_t *args,
        size_t                   count)
{
    char *text = dmi_tool_message(table, key, fallback, args, count);
    if (text == nullptr) {
        dmi_error_raise(session->context, DMI_ERROR_OUT_OF_MEMORY);
        return false;
    }

    dmi_text_printf(session, color, "%s", text);
    dmi_free(text);

    return true;
}

//
// Print a message of the tool resources on a line of its own.
//
static bool dmi_text_message(
        dmi_text_session_t      *session,
        const char              *table,
        const char              *key,
        const char              *fallback,
        const dmi_message_arg_t *args,
        size_t                   count)
{
    if (not dmi_text_message_color(session, DMI_TTY_COLOR_NONE, table, key, fallback, args, count))
        return false;

    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\n");

    return true;
}

bool dmi_text_entry(dmi_text_session_t *session)
{
    assert(session != nullptr);

    // Meta-data is hidden in quiet mode
    if (session->options.mode == DMI_FORMAT_MODE_QUIET)
        return true;

    dmi_context_t        *context  = session->context;
    const dmi_registry_t *registry = dmi_get_registry(context);

    char *version = dmi_version_format(context->state.smbios_version);
    if (version == nullptr) {
        dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
        return false;
    }

    size_t entity_count = context->state.entity_count;
    if (entity_count == 0)
        entity_count = registry->count;

    const char *vendor = context->state.vendor_name;

    char address[sizeof("0xffffffffffffffff")];
    snprintf(address, sizeof(address), "0x%" PRIx64, context->state.table_area_addr);

    bool success =
        dmi_text_message(session, "entry", "version", "SMBIOS {0} present",
                         (const dmi_message_arg_t[]){ DMI_MESSAGE_TEXT(version) }, 1) and
        dmi_text_message(session, "entry", (vendor != nullptr) ? "vendor" : "vendor-unknown",
                         (vendor != nullptr) ? "SMBIOS vendor: {0}" : "SMBIOS vendor: unknown",
                         (const dmi_message_arg_t[]){ DMI_MESSAGE_TEXT(vendor) }, 1) and
        dmi_text_message(session, "entry", "size", "{0} structures occupying {1} bytes",
                         (const dmi_message_arg_t[]){
                             DMI_MESSAGE_NUMBER(entity_count),
                             DMI_MESSAGE_NUMBER(context->state.table_size)
                         }, 2) and
        dmi_text_message(session, "entry", "address", "Table at {0}",
                         (const dmi_message_arg_t[]){ DMI_MESSAGE_TEXT(address) }, 1);

    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\n");

    dmi_free(version);

    return success;
}

bool dmi_text_entity_start(dmi_text_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    bool verbose = (session->options.mode == DMI_FORMAT_MODE_VERBOSE);

    if (session->options.mode != DMI_FORMAT_MODE_QUIET) {
        char handle[sizeof("0xffff")];
        snprintf(handle, sizeof(handle), "0x%04hX", dmi_entity_handle(entity));

        if (not dmi_text_message_color(session, DMI_TTY_COLOR_YELLOW, "entity", "header",
                                       "Handle {0}, DMI type {1}, {2} bytes",
                                       (const dmi_message_arg_t[]){
                                           DMI_MESSAGE_TEXT(handle),
                                           DMI_MESSAGE_NUMBER(dmi_entity_type(entity)),
                                           DMI_MESSAGE_NUMBER(entity->total_length)
                                       }, 3))
            return false;

        // Structure states follow the header in verbose mode
        if (verbose) {
            dmi_format_set_iter_t iter;
            const dmi_format_flag_t *flag;

            dmi_format_mask_iter_init(&iter, &dmi_entity_state_names, entity->state,
                                      sizeof(entity->state) * CHAR_BIT);

            while ((flag = dmi_format_set_iter_next(&iter)) != nullptr) {
                if (flag->value)
                    dmi_text_printf(session, DMI_TTY_COLOR_YELLOW, ", %s", flag->name);
            }
        }

        dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\n");
    }

    dmi_text_printf(session, DMI_TTY_COLOR_YELLOW, "%s", dmi_entity_name(entity));

    // Structure version follows the name in verbose mode
    if (verbose and (entity->level != DMI_VERSION_NONE)) {
        char *level = dmi_version_format(entity->level);
        if (level == nullptr)
            return false;

        dmi_text_printf(session, DMI_TTY_COLOR_YELLOW, " (%s)", level);
        dmi_free(level);
    }

    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\n");

    return true;
}

bool dmi_text_entity_attr(
        dmi_text_session_t    *session,
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attr,
        const void            *value)
{
    assert(session != nullptr);
    assert(entity != nullptr);
    assert(attr != nullptr);
    assert(value != nullptr);

    // Value of variant attribute is described by the variant
    const dmi_attribute_t *variant = dmi_attribute_resolve(attr, entity->info);
    if (variant == nullptr)
        return true;

    value = dmi_member_ptr(entity->info, variant->value, dmi_data_t);

    if (dmi_text_is_hidden(session, variant))
        return true;

    // Attributes are named after the structure they belong to, so that the
    // names which are common to structures can be translated once
    const char *owner = (entity->spec != nullptr) ? entity->spec->code : nullptr;

    // Print attribute name, values are preceded by spaces themselves
    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\t%s:", dmi_attribute_name(attr, owner));

    // Print attribute value
    if (not dmi_member_is_present(variant->counter)) {
        if (variant->type == DMI_ATTRIBUTE_TYPE_STRUCT)
            dmi_text_entity_attr_struct(session, variant, value, 2, owner);
        else
            dmi_text_entity_attr_value(session, variant, value, nullptr, 1);
    } else {
        dmi_text_entity_attr_array(session, variant, entity->info, value, owner);
    }

    return true;
}

void dmi_text_entity_attr_array(
        dmi_text_session_t    *session,
        const dmi_attribute_t *attr,
        const dmi_data_t      *info,
        const void            *value,
        const char            *owner)
{
    assert(session != nullptr);
    assert(attr != nullptr);
    assert(info != nullptr);
    assert(value != nullptr);

    dmi_registry_t *registry = dmi_get_registry(session->context);

    dmi_format_array_iter_t iter;
    const dmi_data_t *ptr;

    dmi_format_array_iter_init(&iter, attr, info, value);
    dmi_text_printf(session, DMI_TTY_COLOR_NONE, " %zu items\n", iter.count);

    while ((ptr = dmi_format_array_iter_next(&iter)) != nullptr) {
        dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\t\t%zu:", iter.index);

        if (attr->type == DMI_ATTRIBUTE_TYPE_STRUCT) {
            dmi_text_entity_attr_struct(session, attr, ptr, 3, owner);
        } else {
            const char *descr = nullptr;

            if (attr->type == DMI_ATTRIBUTE_TYPE_HANDLE) {
                dmi_handle_t handle = dmi_deref(dmi_handle_t, ptr);
                const dmi_entity_t *entity = dmi_registry_lookup(registry, handle, DMI_TYPE_ANY, true);

                descr = dmi_entity_name(entity);
            }

            dmi_text_entity_attr_value(session, attr, ptr, descr, 2);
        }
    }
}

void dmi_text_entity_attr_struct(
        dmi_text_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value,
        unsigned int           depth,
        const char            *owner)
{
    assert(session != nullptr);
    assert(attr != nullptr);
    assert(value != nullptr);

    const dmi_attribute_t *child_attr = nullptr;

    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\n");
    for (child_attr = attr->params.attrs; child_attr->params.name; child_attr++) {
        // Value of variant attribute is described by the variant
        const dmi_attribute_t *child = dmi_attribute_resolve(child_attr, value);
        if (child == nullptr)
            continue;

        const dmi_data_t *ptr = dmi_member_ptr(value, child->value, dmi_data_t);

        if (dmi_text_is_hidden(session, child))
            continue;

        // Fields are indented one level deeper than the structure
        dmi_text_printf(session, DMI_TTY_COLOR_NONE, "%.*s%s:", (int)depth, "\t\t\t\t\t\t\t\t", dmi_attribute_name(child_attr, owner));

        if (child->type == DMI_ATTRIBUTE_TYPE_STRUCT)
            dmi_text_entity_attr_struct(session, child, ptr, depth + 1, owner);
        else
            dmi_text_entity_attr_value(session, child, ptr, nullptr, depth);
    }
}

void dmi_text_entity_attr_value(
        dmi_text_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value,
        const char            *descr,
        unsigned int           depth)
{
    assert(session != nullptr);
    assert(attr != nullptr);
    assert(value != nullptr);

    char *text;

    // Values are preceded by a space, which is omitted for empty values, so
    // that there are no trailing spaces
    if (dmi_attribute_is_unspecified(attr, value)) {
        dmi_text_printf(session, DMI_TTY_COLOR_GREY, " %s\n",
                        dmi_tool_text("value", "unspecified", "<unspecified>"));
        return;
    }
    if (dmi_attribute_is_unknown(attr, value)) {
        dmi_text_printf(session, DMI_TTY_COLOR_OLIVE, " %s\n",
                        dmi_tool_text("value", "unknown", "<unknown>"));
        return;
    }

    text = dmi_attribute_format(session->context, attr, value, true);
    if (text == nullptr) {
        dmi_text_printf(session, DMI_TTY_COLOR_RED, " %s\n",
                        dmi_tool_text("value", "error", "<error>"));
        return;
    }

    if ((*text != 0) or (attr->params.unit != DMI_UNIT_NONE))
        fputc(' ', session->stream);

    // Adjust color of boolean values
    dmi_tty_color_t color = DMI_TTY_COLOR_NONE;
    if (attr->type == DMI_ATTRIBUTE_TYPE_BOOL) {
        if (dmi_attribute_get_bool(attr, value))
            color = DMI_TTY_COLOR_LIME;
        else
            color = DMI_TTY_COLOR_RED;
    }

    if (attr->params.unit != DMI_UNIT_NONE)
        dmi_text_printf(session, color, "%s %s", text, dmi_unit_name(attr->params.unit));
    else
        dmi_text_printf(session, color, "%s", text);

    if (descr != nullptr)
        dmi_text_printf(session, DMI_TTY_COLOR_NONE, " - %s", descr);

    fputc('\n', session->stream);

    dmi_free(text);

    // Flags are indented one level deeper than the attribute
    if (attr->type == DMI_ATTRIBUTE_TYPE_SET)
        dmi_text_entity_attr_set(session, attr, value, depth + 1);
}

void dmi_text_entity_attr_set(
        dmi_text_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value,
        unsigned int           depth)
{
    assert(session != nullptr);
    assert(attr != nullptr);
    assert(value != nullptr);

    dmi_format_set_iter_t iter;
    const dmi_format_flag_t *flag;

    dmi_format_set_iter_init(&iter, attr, value);

    while ((flag = dmi_format_set_iter_next(&iter)) != nullptr) {
        dmi_tty_color_t color = flag->value ? DMI_TTY_COLOR_LIME : DMI_TTY_COLOR_RED;

        dmi_text_printf(session, DMI_TTY_COLOR_NONE, "%.*s%s: ", (int)depth, "\t\t\t\t\t\t\t\t", flag->name);
        dmi_text_printf(session, color, "%s\n", dmi_bool_name(flag->value));
    }
}

bool dmi_text_entity_properties(dmi_text_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    dmi_format_property_iter_t iter;
    const dmi_string_property_t *property;

    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\tProperties:\n");

    dmi_format_property_iter_init(&iter, entity);

    while ((property = dmi_format_property_iter_next(&iter)) != nullptr) {
        dmi_name_type_t type;
        const char *name = dmi_name_lookup_ex(&dmi_property_names, property->ident, &type);

        // Identifiers named by their range only are told apart by value
        if (type == DMI_NAME_TYPE_EXACT) {
            dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\t\t%s:", name);
        } else {
            dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\t\t%s (0x%04X):",
                            name ? name : dmi_tool_text("value", "invalid", "<invalid>"),
                            (unsigned)property->ident);
        }

        // Empty values are not preceded by a space
        if (property->value == nullptr)
            dmi_text_printf(session, DMI_TTY_COLOR_GREY, " %s\n",
                            dmi_tool_text("value", "unspecified", "<unspecified>"));
        else if (*property->value != 0)
            dmi_text_printf(session, DMI_TTY_COLOR_NONE, " %s\n", property->value);
        else
            dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\n");
    }

    return true;
}

bool dmi_text_entity_overlays(dmi_text_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\tAdditional information:\n");

    for (const dmi_entity_overlay_t *overlay = entity->overlays; overlay != nullptr; overlay = overlay->next) {
        char *value = dmi_format_overlay_value(entity, overlay, true);
        if (value == nullptr)
            return false;

        dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\t\t0x%04hX[%zu]: %s at offset 0x%02X",
                        overlay->source->handle, overlay->index, value, overlay->entry->ref_offset);

        dmi_free(value);

        if (overlay->entry->string != nullptr)
            dmi_text_printf(session, DMI_TTY_COLOR_NONE, " - \"%s\"", overlay->entry->string);

        dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\n");
    }

    return true;
}

bool dmi_text_entity_data(dmi_text_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\tHeader and data:\n");
    dmi_text_hex_data(session, entity->data, entity->body_length);

    return true;
}

bool dmi_text_entity_strings(dmi_text_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    if (entity->string_count == 0)
        return true;

    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\tStrings:\n");

    dmi_format_string_iter_t iter;
    const char *str;

    dmi_format_string_iter_init(&iter, entity);

    while ((str = dmi_format_string_iter_next(&iter)) != nullptr)
        dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\t\t%zu: \"%s\"\n", iter.index, str);

    return true;
}

bool dmi_text_entity_end(dmi_text_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    dmi_unused(entity);

    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\n");

    return true;
}

void dmi_text_finalize(dmi_text_session_t *session)
{
    assert(session != nullptr);

    dmi_free(session);
}
