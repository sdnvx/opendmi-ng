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

/**
 * @internal
 * @brief Print the name of a member, unless it is hidden.
 *
 * @param[in] context Session, `dmi_text_session_t`.
 * @param[in] node    Node of the attribute walk.
 *
 * @return Walk status, which skips the member if it is hidden.
 */
static dmi_attribute_walk_t dmi_text_attr_member(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief Begin a nested structure, whose members are printed one level
 * deeper.
 *
 * @param[in] context Session, `dmi_text_session_t`.
 * @param[in] node    Node of the attribute walk.
 *
 * @return Walk status.
 */
static dmi_attribute_walk_t dmi_text_attr_struct_start(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief Print the number of the elements of an array, which are printed one
 * level deeper.
 *
 * @param[in] context Session, `dmi_text_session_t`.
 * @param[in] node    Node of the attribute walk.
 *
 * @return Walk status.
 */
static dmi_attribute_walk_t dmi_text_attr_array_start(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief End a nested structure or an array, going back to the level of
 * indentation of its member.
 *
 * @param[in] context Session, `dmi_text_session_t`.
 * @param[in] node    Node of the attribute walk.
 *
 * @return Always `true`.
 */
static bool dmi_text_attr_outdent(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief Print the index of an element of an array.
 *
 * @param[in] context Session, `dmi_text_session_t`.
 * @param[in] node    Node of the attribute walk.
 *
 * @return Walk status.
 */
static dmi_attribute_walk_t dmi_text_attr_item_start(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief Print the value of a member or of an element of an array.
 *
 * @details Elements of an array of handles are described by the structures
 * they refer to.
 *
 * @param[in] context Session, `dmi_text_session_t`.
 * @param[in] node    Node of the attribute walk.
 *
 * @return Always `true`.
 */
static bool dmi_text_attr_value(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief Print the flags of a set, each on a line of its own.
 *
 * @param[in] session Session.
 * @param[in] attr    Attribute of the set.
 * @param[in] value   Value of the set.
 * @param[in] depth   Level of indentation of the flags.
 */
static void dmi_text_entity_attr_set(
        dmi_text_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value,
        unsigned int           depth);

/**
 * @internal
 * @brief Check whether an attribute is hidden from the output.
 *
 * @details Handle references are hidden in quiet mode.
 *
 * @param[in] session Session.
 * @param[in] attr    Attribute to check.
 *
 * @return `true` if the attribute is hidden, `false` otherwise.
 */
static bool dmi_text_is_hidden(const dmi_text_session_t *session, const dmi_attribute_t *attr);

/**
 * @internal
 * @brief Print a message of the tool resources, which is translated to the
 * locale, and is taken from the fallback pattern if there is no translation.
 *
 * @param[in] session  Session.
 * @param[in] color    Color of the message.
 * @param[in] table    Resource table of the message.
 * @param[in] key      Key of the message in the table.
 * @param[in] fallback Pattern used if there is no translation.
 * @param[in] args     Arguments of the message.
 * @param[in] count    Number of the arguments.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Message cannot be formatted
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_text_message_color(
        dmi_text_session_t      *session,
        dmi_tty_color_t          color,
        const char              *table,
        const char              *key,
        const char              *fallback,
        const dmi_message_arg_t *args,
        size_t                   count);

/**
 * @internal
 * @brief Print a message of the tool resources on a line of its own.
 *
 * @param[in] session  Session.
 * @param[in] table    Resource table of the message.
 * @param[in] key      Key of the message in the table.
 * @param[in] fallback Pattern used if there is no translation.
 * @param[in] args     Arguments of the message.
 * @param[in] count    Number of the arguments.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_text_message(
        dmi_text_session_t      *session,
        const char              *table,
        const char              *key,
        const char              *fallback,
        const dmi_message_arg_t *args,
        size_t                   count);

/**
 * @internal
 * @brief Print a string of the data, which is not trusted, between prefix and
 * suffix.
 *
 * @details The string is escaped before it is printed.
 *
 * @param[in] session Session.
 * @param[in] prefix  Text printed before the string.
 * @param[in] str     String to print.
 * @param[in] suffix  Text printed after the string.
 */
static void dmi_text_print_string(
        dmi_text_session_t *session,
        const char         *prefix,
        const char         *str,
        const char         *suffix);

/**
 * @internal
 * @brief Callbacks printing the attributes of a structure.
 */
static const dmi_attribute_visitor_t dmi_text_attr_visitor = {
    .member_start = dmi_text_attr_member,
    .struct_start = dmi_text_attr_struct_start,
    .struct_end   = dmi_text_attr_outdent,
    .array_start  = dmi_text_attr_array_start,
    .array_end    = dmi_text_attr_outdent,
    .item_start   = dmi_text_attr_item_start,
    .value        = dmi_text_attr_value
};

void *dmi_text_initialize(dmi_context_t *context, FILE *stream, const dmi_format_options_t *options)
{
    assert(context != nullptr);
    assert(stream != nullptr);

    dmi_text_session_t *session;

    session = dmi_alloc(context, sizeof(*session));
    if (session == nullptr)
        return nullptr;

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

bool dmi_text_entry(dmi_text_session_t *session)
{
    assert(session != nullptr);

    // Meta-data is hidden in quiet mode
    if (session->options.mode == DMI_FORMAT_MODE_QUIET)
        return true;

    dmi_context_t        *context  = session->context;
    const dmi_registry_t *registry = dmi_get_registry(context);

    char *version = dmi_version_format(context->state.smbios_version);
    if (version == nullptr)
        return dmi_trace_out_of_memory(context);

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
                             DMI_MESSAGE_NUMBER(context->state.table->length)
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
                                           DMI_MESSAGE_NUMBER(dmi_entity_type_id(entity)),
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
            return dmi_trace_out_of_memory(session->context);

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

    dmi_unused(value);

    // Attributes are named after the structure they belong to, so that the
    // names which are common to structures can be translated once
    session->owner = (entity->spec != nullptr) ? entity->spec->code : nullptr;
    session->depth = 1;

    dmi_attribute_walk(attr, entity->info, &dmi_text_attr_visitor, session);

    return true;
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

    // Values may hold strings of the data, which are not trusted
    char *escaped;
    const char *safe = dmi_text_escape(session, text, &escaped);

    // Adjust color of boolean values
    dmi_tty_color_t color = DMI_TTY_COLOR_NONE;
    if (attr->type == DMI_ATTRIBUTE_TYPE_BOOL) {
        if (dmi_attribute_get_bool(attr, value))
            color = DMI_TTY_COLOR_LIME;
        else
            color = DMI_TTY_COLOR_RED;
    }

    if (attr->params.unit != DMI_UNIT_NONE)
        dmi_text_printf(session, color, "%s %s", safe, dmi_unit_name(attr->params.unit));
    else
        dmi_text_printf(session, color, "%s", safe);

    if (descr != nullptr)
        dmi_text_printf(session, DMI_TTY_COLOR_NONE, " - %s", descr);

    fputc('\n', session->stream);

    dmi_free(escaped);
    dmi_free(text);

    // Flags are indented one level deeper than the attribute
    if (attr->type == DMI_ATTRIBUTE_TYPE_SET)
        dmi_text_entity_attr_set(session, attr, value, depth + 1);
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
            dmi_text_print_string(session, " ", property->value, "\n");
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
            dmi_text_print_string(session, " - \"", overlay->entry->string, "\"");

        dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\n");
    }

    return true;
}

bool dmi_text_entity_data(dmi_text_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\tHeader and data:\n");
    dmi_text_hex_data(session, dmi_entity_data(entity, DMI_TYPE_ANY), entity->body_length);

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

    while ((str = dmi_format_string_iter_next(&iter)) != nullptr) {
        char *escaped;

        dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\t\t%zu: \"%s\"\n", iter.index,
                        dmi_text_escape(session, str, &escaped));
        dmi_free(escaped);
    }

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

static bool dmi_text_is_hidden(const dmi_text_session_t *session, const dmi_attribute_t *attr)
{
    return (session->options.mode == DMI_FORMAT_MODE_QUIET) and (attr->type == DMI_ATTRIBUTE_TYPE_HANDLE);
}

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
    if (text == nullptr)
        return dmi_trace_out_of_memory(session->context);

    dmi_text_printf(session, color, "%s", text);
    dmi_free(text);

    return true;
}

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

static dmi_attribute_walk_t dmi_text_attr_member(void *context, const dmi_attribute_node_t *node)
{
    dmi_text_session_t *session = context;

    if (dmi_text_is_hidden(session, node->attr))
        return DMI_ATTRIBUTE_WALK_SKIP;

    // Values are preceded by spaces themselves
    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "%.*s%s:", (int)session->depth, "\t\t\t\t\t\t\t\t",
                    dmi_attribute_name(node->member, session->owner));

    return DMI_ATTRIBUTE_WALK_CONTINUE;
}

static dmi_attribute_walk_t dmi_text_attr_struct_start(void *context, const dmi_attribute_node_t *node)
{
    dmi_text_session_t *session = context;

    dmi_unused(node);

    // Fields are indented one level deeper than the structure
    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\n");
    session->depth++;

    return DMI_ATTRIBUTE_WALK_CONTINUE;
}

static dmi_attribute_walk_t dmi_text_attr_array_start(void *context, const dmi_attribute_node_t *node)
{
    dmi_text_session_t *session = context;

    // Elements are indented one level deeper than the array
    dmi_text_printf(session, DMI_TTY_COLOR_NONE, " %zu items\n", node->count);
    session->depth++;

    return DMI_ATTRIBUTE_WALK_CONTINUE;
}

static bool dmi_text_attr_outdent(void *context, const dmi_attribute_node_t *node)
{
    dmi_text_session_t *session = context;

    dmi_unused(node);

    session->depth--;

    return true;
}

static dmi_attribute_walk_t dmi_text_attr_item_start(void *context, const dmi_attribute_node_t *node)
{
    dmi_text_session_t *session = context;

    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "%.*s%zu:", (int)session->depth, "\t\t\t\t\t\t\t\t", node->index);

    return DMI_ATTRIBUTE_WALK_CONTINUE;
}

static bool dmi_text_attr_value(void *context, const dmi_attribute_node_t *node)
{
    dmi_text_session_t *session = context;
    const char *descr = nullptr;

    // Elements of an array of handles are described by the structures they
    // refer to
    if ((node->index != SIZE_MAX) and (node->attr->type == DMI_ATTRIBUTE_TYPE_HANDLE)) {
        dmi_registry_t *registry = dmi_get_registry(session->context);
        dmi_handle_t    handle   = dmi_deref(dmi_handle_t, node->value);

        descr = dmi_entity_name(dmi_registry_lookup(registry, handle, DMI_TYPE_ANY, true));
    }

    dmi_text_entity_attr_value(session, node->attr, node->value, descr, session->depth);

    return true;
}

static void dmi_text_entity_attr_set(
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

static void dmi_text_print_string(
        dmi_text_session_t *session,
        const char         *prefix,
        const char         *str,
        const char         *suffix)
{
    char *escaped;

    dmi_text_printf(session, DMI_TTY_COLOR_NONE, "%s%s%s",
                    prefix, dmi_text_escape(session, str, &escaped), suffix);
    dmi_free(escaped);
}
