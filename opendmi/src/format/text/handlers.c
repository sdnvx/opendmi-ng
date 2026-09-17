//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include "../../config.h"

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

bool dmi_text_entry(dmi_text_session_t *session)
{
    assert(session != nullptr);

    // Meta-data is hidden in quiet mode
    if (session->options.mode == DMI_FORMAT_MODE_QUIET)
        return true;

    dmi_context_t *context = session->context;

    char *version = dmi_version_format(context->state.smbios_version);
    if (version == nullptr) {
        dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
        return false;
    }

    size_t entity_count = context->state.entity_count;
    if (entity_count == 0)
        entity_count = context->state.registry->count;

    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "SMBIOS %s present\n", version);
    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "SMBIOS vendor: %s\n",
                    context->state.vendor_name ? context->state.vendor_name : "unknown");
    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "%zu structures occupying %zu bytes\n",
                    entity_count, context->state.table_size);
    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "Table at 0x%" PRIx64 "\n",
                    context->state.table_area_addr);
    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\n");

    dmi_free(version);

    return true;
}

bool dmi_text_entity_start(dmi_text_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    if (session->options.mode != DMI_FORMAT_MODE_QUIET) {
        dmi_text_printf(session, DMI_TTY_COLOR_YELLOW, "Handle 0x%04hX, DMI type %d, %zu bytes\n",
                        dmi_entity_handle(entity),
                        dmi_entity_type(entity),
                        entity->total_length);
    }

    dmi_text_printf(session, DMI_TTY_COLOR_YELLOW, "%s\n", dmi_entity_name(entity));

    if (session->options.mode != DMI_FORMAT_MODE_VERBOSE)
        return true;

    if (entity->level != DMI_VERSION_NONE) {
        char *level = dmi_version_format(entity->level);
        if (level == nullptr)
            return false;

        dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\tStructure version: %s\n", level);
        dmi_free(level);
    }

    dmi_format_set_iter_t iter;
    const dmi_format_flag_t *flag;
    const char *separator = "";

    dmi_format_mask_iter_init(&iter, &dmi_entity_state_names, entity->state,
                              sizeof(entity->state) * CHAR_BIT);

    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\tState: ");

    while ((flag = dmi_format_set_iter_next(&iter)) != nullptr) {
        if (not flag->value)
            continue;

        dmi_text_printf(session, DMI_TTY_COLOR_NONE, "%s%s", separator, flag->name);
        separator = ", ";
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

    if (dmi_text_is_hidden(session, attr))
        return true;

    // Print attribute name
    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\t%s: ", attr->params.name);

    // Print attribute value
    if (not dmi_member_is_present(attr->counter)) {
        if (attr->type == DMI_ATTRIBUTE_TYPE_STRUCT)
            dmi_text_entity_attr_struct(session, attr, value);
        else
            dmi_text_entity_attr_value(session, attr, value, nullptr);
    } else {
        dmi_text_entity_attr_array(session, attr, entity->info, value);
    }

    return true;
}

void dmi_text_entity_attr_array(
        dmi_text_session_t    *session,
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

    dmi_format_array_iter_init(&iter, attr, info, value);
    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "%zu items\n", iter.count);

    while ((ptr = dmi_format_array_iter_next(&iter)) != nullptr) {
        dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\t\t%zu: ", iter.index);

        if (attr->type == DMI_ATTRIBUTE_TYPE_STRUCT) {
            dmi_text_entity_attr_struct(session, attr, ptr);
        } else {
            const char *descr = nullptr;

            if (attr->type == DMI_ATTRIBUTE_TYPE_HANDLE) {
                dmi_handle_t handle = dmi_deref(dmi_handle_t, ptr);
                const dmi_entity_t *entity = dmi_registry_get(session->context->state.registry, handle, DMI_TYPE_INVALID, true);

                descr = dmi_entity_name(entity);
            }

            dmi_text_entity_attr_value(session, attr, ptr, descr);
        }
    }
}

void dmi_text_entity_attr_struct(
        dmi_text_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value)
{
    assert(session != nullptr);
    assert(attr != nullptr);
    assert(value != nullptr);

    const dmi_attribute_t *child_attr = nullptr;

    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\n");
    for (child_attr = attr->params.attrs; child_attr->params.name; child_attr++) {
        const dmi_data_t *ptr = dmi_member_ptr(value, child_attr->value, dmi_data_t);

        if (dmi_text_is_hidden(session, child_attr))
            continue;

        dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\t\t\t%s: ", child_attr->params.name);
        dmi_text_entity_attr_value(session, child_attr, ptr, nullptr);
    }
}

void dmi_text_entity_attr_value(
        dmi_text_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value,
        const char            *descr)
{
    assert(session != nullptr);
    assert(attr != nullptr);
    assert(value != nullptr);

    char *text;

    if (dmi_attribute_is_unspecified(attr, value)) {
        dmi_text_printf(session, DMI_TTY_COLOR_GREY, "<unspecified>\n");
        return;
    }
    if (dmi_attribute_is_unknown(attr, value)) {
        dmi_text_printf(session, DMI_TTY_COLOR_OLIVE, "<unknown>\n");
        return;
    }

    text = dmi_attribute_format(session->context, attr, value, true);
    if (text == nullptr) {
        dmi_text_printf(session, DMI_TTY_COLOR_RED, "<error>\n");
        return;
    }

    // Adjust color of boolean values
    dmi_tty_color_t color = DMI_TTY_COLOR_NONE;
    if (attr->type == DMI_ATTRIBUTE_TYPE_BOOL) {
        if (dmi_attribute_get_bool(attr, value))
            color = DMI_TTY_COLOR_LIME;
        else
            color = DMI_TTY_COLOR_RED;
    }

    if (attr->params.unit)
        dmi_text_printf(session, color, "%s %s", text, attr->params.unit);
    else
        dmi_text_printf(session, color, "%s", text);

    if (descr != nullptr)
        dmi_text_printf(session, DMI_TTY_COLOR_NONE, " - %s", descr);

    fputc('\n', session->stream);

    dmi_free(text);

    if (attr->type == DMI_ATTRIBUTE_TYPE_SET)
        dmi_text_entity_attr_set(session, attr, value);
}

void dmi_text_entity_attr_set(
        dmi_text_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value)
{
    assert(session != nullptr);
    assert(attr != nullptr);
    assert(value != nullptr);

    dmi_format_set_iter_t iter;
    const dmi_format_flag_t *flag;

    dmi_format_set_iter_init(&iter, attr, value);

    while ((flag = dmi_format_set_iter_next(&iter)) != nullptr) {
        dmi_tty_color_t color = flag->value ? DMI_TTY_COLOR_LIME : DMI_TTY_COLOR_RED;

        dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\t\t%s: ", flag->name);
        dmi_text_printf(session, color, "%s\n", flag->value ? "yes" : "no");
    }
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
