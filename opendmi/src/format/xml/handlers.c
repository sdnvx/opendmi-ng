//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <time.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/error.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>

#include <opendmi/format/iter.h>
#include <opendmi/format/xml/handlers.h>
#include <opendmi/format/xml/helpers.h>

/**
 * @internal
 * @brief Begin the element a member is written as, named by the code of its
 * attribute.
 *
 * @param[in] context Session, `dmi_xml_session_t`.
 * @param[in] node    Node of the attribute walk.
 *
 * @return Walk status, which stops the walk on failure.
 */
static dmi_attribute_walk_t dmi_xml_attr_member_start(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief Begin the `item` element an element of an array is written as.
 *
 * @param[in] context Session, `dmi_xml_session_t`.
 * @param[in] node    Node of the attribute walk.
 *
 * @return Walk status, which stops the walk on failure.
 */
static dmi_attribute_walk_t dmi_xml_attr_item_start(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief End the element a member or an element of an array is written as.
 *
 * @param[in] context Session, `dmi_xml_session_t`.
 * @param[in] node    Node of the attribute walk.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_xml_attr_end(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief Write the value of a member or of an element of an array.
 *
 * @param[in] context Session, `dmi_xml_session_t`.
 * @param[in] node    Node of the attribute walk.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_xml_attr_value(void *context, const dmi_attribute_node_t *node);

/**
 * @internal
 * @brief Write a value as the text of its element: nothing if it is
 * unspecified, the word `unknown` if it is unknown, the flags for a set, and
 * the formatted value otherwise, whose unit is an attribute of the element.
 *
 * @param[in] session Session.
 * @param[in] attr    Attribute of the value.
 * @param[in] value   Value to write.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_xml_entity_attr_value(
        dmi_xml_session_t     *session,
        const dmi_attribute_t *attr,
        const void            *value);

/**
 * @internal
 * @brief Write the flags of a set as `flag` elements, along with the value of
 * the set as an attribute.
 *
 * @details Flag codes are not always valid element names (e.g. "5v"), so
 * they are written as attributes of the `flag` elements.
 *
 * @param[in] session Session.
 * @param[in] attr    Attribute of the set.
 * @param[in] value   Value of the set.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_xml_entity_attr_set(
        dmi_xml_session_t     *session,
        const dmi_attribute_t *attr,
        const void            *value);

/**
 * @internal
 * @brief Callbacks writing the attributes of a structure.
 */
static const dmi_attribute_visitor_t dmi_xml_attr_visitor = {
    .member_start = dmi_xml_attr_member_start,
    .member_end   = dmi_xml_attr_end,
    .item_start   = dmi_xml_attr_item_start,
    .item_end     = dmi_xml_attr_end,
    .value        = dmi_xml_attr_value
};

/**
 * @internal
 * @brief Write callback of the XML output buffer, which writes to the output
 * stream of the session and keeps the reason of a failure.
 *
 * @param[in] context Session, `dmi_xml_session_t`.
 * @param[in] buffer  Data to write.
 * @param[in] length  Length of the data.
 *
 * @return Number of the bytes written, or -1 on failure.
 */
static int dmi_xml_write(void *context, const char *buffer, int length);

/**
 * @internal
 * @brief Close callback of the XML output buffer, which flushes the output
 * stream of the session and leaves closing it to the caller, who owns it.
 *
 * @param[in] context Session, `dmi_xml_session_t`.
 *
 * @return 0 on success, or -1 on failure.
 */
static int dmi_xml_close(void *context);

/**
 * @internal
 * @brief Write an `overlay` element describing an additional information
 * entry applied to a structure.
 *
 * @details The element holds the value the entry gives and, if the entry has
 * one, its string.
 *
 * @param[in] session Session.
 * @param[in] entity  Structure the entry is applied to.
 * @param[in] overlay Entry applied to the structure.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_xml_entity_overlay(
        dmi_xml_session_t          *session,
        const dmi_entity_t         *entity,
        const dmi_entity_overlay_t *overlay);

#ifdef _WIN32

/**
 * @internal
 * @brief Convert a time to the broken-down UTC time, the way POSIX
 * `gmtime_r()` does, which Windows provides as `gmtime_s()`.
 *
 * @param[in]  timep  Time to convert.
 * @param[out] result Variable to store the broken-down time in.
 *
 * @return @p result on success, or `nullptr` on failure.
 */
static inline struct tm *gmtime_r(const time_t *timep, struct tm *result);

#endif

#ifdef _WIN32
static inline struct tm *gmtime_r(const time_t *timep, struct tm *result)
{
    errno_t err = gmtime_s(result, timep);
    if (err != 0)
        return nullptr;
    return result;
}
#endif

void *dmi_xml_initialize(dmi_context_t *context, FILE *stream, const dmi_format_options_t *options)
{
    assert(context != nullptr);
    assert(stream != nullptr);

    bool success = false;
    dmi_xml_session_t *session;

    session = dmi_alloc(context, sizeof(*session));
    if (session == nullptr)
        return nullptr;

    // Output buffer writes through the session, so the stream is set first
    session->context = context;
    session->stream  = stream;

    do {
        session->buffer = xmlOutputBufferCreateIO(dmi_xml_write, dmi_xml_close, session, nullptr);
        if (session->buffer == nullptr) {
            dmi_error_raise_ex(context, DMI_ERROR_INTERNAL, "Unable to create xmlOutputBuffer");
            break;
        }

        session->writer = xmlNewTextWriter(session->buffer);
        if (session->writer == nullptr) {
            dmi_error_raise_ex(context, DMI_ERROR_INTERNAL, "Unable to create xmlWriter");
            break;
        }

        if (xmlTextWriterSetIndent(session->writer, 1) < 0) {
            dmi_error_raise_ex(context, DMI_ERROR_INTERNAL, "Unable to configure xmlWriter indentation");
            break;
        }
        if (xmlTextWriterSetIndentString(session->writer, dmi_xml_string("  ")) < 0) {
            dmi_error_raise_ex(context, DMI_ERROR_INTERNAL, "Unable to configure xmlWriter indentation");
            break;
        }

        success = true;
    } while (false);

    if (not success) {
        if (session->writer != nullptr)
            xmlFreeTextWriter(session->writer);
        else if (session->buffer != nullptr)
            xmlOutputBufferClose(session->buffer);

        dmi_free(session);
        return nullptr;
    }

    // Default options are used if not specified
    if (options != nullptr)
        session->options = *options;

    return session;
}

bool dmi_xml_dump_start(dmi_xml_session_t *session)
{
    assert(session != nullptr);

    bool success = false;

    do {
        time_t t;
        struct tm tm;

        t = time(nullptr);
        if (t == (time_t)-1) {
            dmi_error_raise_ex(session->context, DMI_ERROR_SYSTEM, "Unable to get current time");
            break;
        }
        if (gmtime_r(&t, &tm) == nullptr) {
            dmi_error_raise_ex(session->context, DMI_ERROR_SYSTEM, "Unable to convert current time");
            break;
        }

        if (not dmi_xml_check(session, xmlTextWriterStartDocument(session->writer, "1.0", "UTF-8", NULL)))
            break;

        if (not dmi_xml_check(session, xmlTextWriterStartElementNS(
                    session->writer,
                    dmi_xml_string(DMI_XML_PREFIX),
                    dmi_xml_string("dump"),
                    dmi_xml_string(DMI_XML_NAMESPACE))))
            break;
        if (not dmi_xml_check(session, xmlTextWriterWriteFormatAttribute(
                    session->writer,
                    dmi_xml_string("created-at"),
                    "%04u-%02u-%02uT%02u:%02u:%02uZ",
                    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                    tm.tm_hour, tm.tm_min, tm.tm_sec)))
            break;
        if (not dmi_xml_check(session, xmlTextWriterWriteAttributeNS(
                    session->writer,
                    dmi_xml_string(DMI_XSI_PREFIX),
                    dmi_xml_string("schemaLocation"),
                    dmi_xml_string(DMI_XSI_NAMESPACE),
                    dmi_xml_string(DMI_XML_SCHEMA_LOCATION))))
            break;

        success = true;
    } while (false);

    return success;
}

bool dmi_xml_entry(dmi_xml_session_t *session)
{
    assert(session != nullptr);

    bool success = false;
    dmi_context_t *context = session->context;
    char *smbios_version = nullptr;

    do {
        smbios_version = dmi_version_format(context->state.smbios_version);
        if (smbios_version == nullptr) {
            dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
            break;
        }

        if (not dmi_xml_check(session, xmlTextWriterStartElementNS(
                    session->writer,
                    dmi_xml_string(DMI_XML_PREFIX),
                    dmi_xml_string("entry"),
                    nullptr)))
            break;

        if (not dmi_xml_check(session, xmlTextWriterWriteAttribute(
                    session->writer,
                    dmi_xml_string("smbios-version"),
                    dmi_xml_string(smbios_version))))
            break;
        if (not dmi_xml_check(session, xmlTextWriterWriteFormatAttribute(
                    session->writer,
                    dmi_xml_string("table-area-address"),
                    "0x%" PRIx64, context->state.table_area_addr)))
            break;
        if (not dmi_xml_check(session, xmlTextWriterWriteFormatAttribute(
                    session->writer,
                    dmi_xml_string("table-area-size"),
                    "%zu", context->state.table->length)))
            break;

        if (not dmi_xml_check(session, xmlTextWriterEndElement(session->writer)))
            break;

        success = true;
    } while (false);

    dmi_free(smbios_version);

    return success;
}

bool dmi_xml_entity_start(dmi_xml_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    bool success = false;

    do {
        if (not dmi_xml_check(session, xmlTextWriterStartElementNS(
                    session->writer,
                    dmi_xml_string(DMI_XML_PREFIX),
                    dmi_xml_string("entity"),
                    nullptr)))
            break;

        if (not dmi_xml_check(session, xmlTextWriterWriteFormatAttribute(
                    session->writer,
                    dmi_xml_string("handle"),
                    "0x%04hx", entity->handle)))
            break;
        if (not dmi_xml_check(session, xmlTextWriterWriteFormatAttribute(
                    session->writer,
                    dmi_xml_string("type"),
                    "%u", entity->type_id)))
            break;
        if (not dmi_xml_check(session, xmlTextWriterWriteFormatAttribute(
                    session->writer,
                    dmi_xml_string("length"),
                    "%zu", entity->total_length)))
            break;

        // State flags are written as a space-separated list
        if (not dmi_xml_check(session, xmlTextWriterStartAttribute(session->writer, dmi_xml_string("state"))))
            break;

        dmi_format_set_iter_t iter;
        const dmi_format_flag_t *flag;
        const char *separator = "";
        bool written = true;

        dmi_format_mask_iter_init(&iter, &dmi_entity_state_names, entity->state,
                                  sizeof(entity->state) * CHAR_BIT);

        while ((flag = dmi_format_set_iter_next(&iter)) != nullptr) {
            if (not flag->value)
                continue;

            if (not dmi_xml_check(session, xmlTextWriterWriteFormatString(session->writer, "%s%s", separator, flag->code))) {
                written = false;
                break;
            }

            separator = " ";
        }

        if (not written)
            break;
        if (not dmi_xml_check(session, xmlTextWriterEndAttribute(session->writer)))
            break;

        success = true;
    } while (false);

    return success;
}

bool dmi_xml_entity_attrs_start(dmi_xml_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    if (not dmi_xml_check(session, xmlTextWriterStartElementNS(
                session->writer,
                dmi_xml_string(DMI_XML_PREFIX),
                dmi_xml_string(entity->spec->code),
                nullptr)))
        return false;

    return true;
}

bool dmi_xml_entity_attr(
        dmi_xml_session_t     *session,
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attr,
        const void            *value)
{
    assert(session != nullptr);
    assert(entity != nullptr);
    assert(attr != nullptr);

    dmi_unused(value);

    // Nested structures are written as nested elements, and arrays as lists
    // of items, whose counters are members of the same structure
    return dmi_attribute_walk(attr, entity->info, &dmi_xml_attr_visitor, session);
}

bool dmi_xml_entity_attrs_end(dmi_xml_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    dmi_unused(entity);

    if (not dmi_xml_check(session, xmlTextWriterFullEndElement(session->writer)))
        return false;

    return true;
}

bool dmi_xml_entity_properties(dmi_xml_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    bool success = false;

    do {
        if (not dmi_xml_check(session, xmlTextWriterStartElementNS(
                    session->writer,
                    dmi_xml_string(DMI_XML_PREFIX),
                    dmi_xml_string("properties"),
                    nullptr)))
            break;

        dmi_format_property_iter_t iter;
        const dmi_string_property_t *property;

        dmi_format_property_iter_init(&iter, entity);

        while ((property = dmi_format_property_iter_next(&iter)) != nullptr) {
            const char *code = dmi_code_lookup(&dmi_property_names, property->ident);

            if (not dmi_xml_check(session, xmlTextWriterStartElementNS(
                        session->writer,
                        dmi_xml_string(DMI_XML_PREFIX),
                        dmi_xml_string("property"),
                        nullptr)))
                break;
            if (not dmi_xml_check(session, xmlTextWriterWriteFormatAttribute(
                        session->writer,
                        dmi_xml_string("id"),
                        "0x%04x", (unsigned)property->ident)))
                break;
            if ((code != nullptr) and
                (not dmi_xml_check(session, xmlTextWriterWriteAttribute(
                        session->writer,
                        dmi_xml_string("code"),
                        dmi_xml_string(code)))))
                break;

            // Element is left empty if the value is not specified
            if ((property->value != nullptr) and not dmi_xml_text(session, property->value))
                break;

            if (not dmi_xml_check(session, xmlTextWriterFullEndElement(session->writer)))
                break;
        }

        // Loop is interrupted on errors
        if (property != nullptr)
            break;

        if (not dmi_xml_check(session, xmlTextWriterFullEndElement(session->writer)))
            break;

        success = true;
    } while (false);

    return success;
}

bool dmi_xml_entity_overlays(dmi_xml_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    if (not dmi_xml_check(session, xmlTextWriterStartElementNS(
                session->writer,
                dmi_xml_string(DMI_XML_PREFIX),
                dmi_xml_string("overlays"),
                nullptr)))
        return false;

    for (const dmi_entity_overlay_t *overlay = entity->overlays; overlay != nullptr; overlay = overlay->next) {
        if (not dmi_xml_entity_overlay(session, entity, overlay))
            return false;
    }

    return dmi_xml_check(session, xmlTextWriterFullEndElement(session->writer));
}

bool dmi_xml_entity_data(dmi_xml_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    bool success = false;

    do {
        if (not dmi_xml_check(session, xmlTextWriterStartElementNS(
                    session->writer,
                    dmi_xml_string(DMI_XML_PREFIX),
                    dmi_xml_string("data"),
                    nullptr)))
            break;

        if (not dmi_xml_data(session, dmi_entity_data(entity, DMI_TYPE_ANY), entity->body_length))
            break;

        if (not dmi_xml_check(session, xmlTextWriterFullEndElement(session->writer)))
            break;

        success = true;
    } while (false);

    return success;
}

bool dmi_xml_entity_strings(dmi_xml_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    bool success = false;

    if (entity->string_count == 0)
        return true;

    do {
        if (not dmi_xml_check(session, xmlTextWriterStartElementNS(
                    session->writer,
                    dmi_xml_string(DMI_XML_PREFIX),
                    dmi_xml_string("strings"),
                    nullptr)))
            break;

        dmi_format_string_iter_t iter;
        const char *str;

        dmi_format_string_iter_init(&iter, entity);

        while ((str = dmi_format_string_iter_next(&iter)) != nullptr) {
            if (not dmi_xml_check(session, xmlTextWriterStartElementNS(
                        session->writer,
                        dmi_xml_string(DMI_XML_PREFIX),
                        dmi_xml_string("string"),
                        nullptr)))
                break;
            if (not dmi_xml_check(session, xmlTextWriterWriteFormatAttribute(
                        session->writer,
                        dmi_xml_string("index"),
                        "%zu", iter.index)))
                break;

            if (not dmi_xml_text(session, str))
                break;

            if (not dmi_xml_check(session, xmlTextWriterFullEndElement(session->writer)))
                break;
        }

        // Loop is interrupted on errors
        if (str != nullptr)
            break;

        if (not dmi_xml_check(session, xmlTextWriterFullEndElement(session->writer)))
            break;

        success = true;
    } while (false);

    return success;
}

bool dmi_xml_entity_end(dmi_xml_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    dmi_unused(entity);

    bool success = false;

    do {
        if (not dmi_xml_check(session, xmlTextWriterFullEndElement(session->writer)))
            break;

        success = true;
    } while (false);

    return success;
}

bool dmi_xml_dump_end(dmi_xml_session_t *session)
{
    assert(session != nullptr);

    bool success = false;

    do {
        if (not dmi_xml_check(session, xmlTextWriterEndDocument(session->writer)))
            break;
        if (not dmi_xml_check(session, xmlTextWriterFlush(session->writer)))
            break;

        success = true;
    } while (false);

    return success;
}

void dmi_xml_finalize(dmi_xml_session_t *session)
{
    assert(session != nullptr);

    xmlFreeTextWriter(session->writer);
    dmi_free(session);
}

static int dmi_xml_write(void *context, const char *buffer, int length)
{
    dmi_xml_session_t *session = context;

    assert(session != nullptr);

    if (length <= 0)
        return 0;

    errno = 0;

    if (fwrite(buffer, 1, (size_t)length, session->stream) < (size_t)length) {
        if (session->write_error == 0)
            session->write_error = (errno != 0) ? errno : EIO;

        return -1;
    }

    return length;
}

static int dmi_xml_close(void *context)
{
    dmi_xml_session_t *session = context;

    assert(session != nullptr);

    errno = 0;

    if (fflush(session->stream) != 0) {
        if (session->write_error == 0)
            session->write_error = (errno != 0) ? errno : EIO;

        return -1;
    }

    return 0;
}

static dmi_attribute_walk_t dmi_xml_attr_member_start(void *context, const dmi_attribute_node_t *node)
{
    dmi_xml_session_t *session = context;

    return dmi_format_walk(dmi_xml_check(session, xmlTextWriterStartElement(
            session->writer, dmi_xml_string(node->member->params.code))));
}

static dmi_attribute_walk_t dmi_xml_attr_item_start(void *context, const dmi_attribute_node_t *node)
{
    dmi_xml_session_t *session = context;

    dmi_unused(node);

    return dmi_format_walk(dmi_xml_check(session, xmlTextWriterStartElement(
            session->writer, dmi_xml_string("item"))));
}

static bool dmi_xml_attr_end(void *context, const dmi_attribute_node_t *node)
{
    dmi_xml_session_t *session = context;

    dmi_unused(node);

    return dmi_xml_check(session, xmlTextWriterFullEndElement(session->writer));
}

static bool dmi_xml_attr_value(void *context, const dmi_attribute_node_t *node)
{
    return dmi_xml_entity_attr_value(context, node->attr, node->value);
}

static bool dmi_xml_entity_attr_value(
        dmi_xml_session_t     *session,
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
        return true;

    // Handle unknown values
    if (dmi_attribute_is_unknown(attr, value)) {
        if (not dmi_xml_check(session, xmlTextWriterWriteString(session->writer, dmi_xml_string("unknown"))))
            return false;
        return true;
    }

    // Handle value sets
    if (attr->type == DMI_ATTRIBUTE_TYPE_SET)
        return dmi_xml_entity_attr_set(session, attr, value);

    do {
        text = dmi_attribute_format(session->context, attr, value, session->options.pretty);
        if (text == nullptr)
            break;

        // Units have an attribute of their own here, so they stay out of the
        // value even when it is formatted for a person. They are serialized
        // by their code names, which do not change with the locale, unless
        // the output is meant to be read rather than parsed
        if (attr->params.unit != DMI_UNIT_NONE) {
            const char *unit = session->options.pretty
                    ? dmi_unit_name(attr->params.unit)
                    : dmi_unit_code(attr->params.unit);

            if (not dmi_xml_check(session, xmlTextWriterWriteAttribute(
                        session->writer,
                        dmi_xml_string("units"),
                        dmi_xml_string(unit))))
                break;
        }

        if (not dmi_xml_text(session, text))
            break;

        success = true;
    } while (false);

    dmi_free(text);

    return success;
}

static bool dmi_xml_entity_attr_set(
        dmi_xml_session_t     *session,
        const dmi_attribute_t *attr,
        const void            *value)
{

    assert(session != nullptr);
    assert(attr != nullptr);
    assert(value != nullptr);

    dmi_format_set_iter_t iter;
    const dmi_format_flag_t *flag;

    dmi_format_set_iter_init(&iter, attr, value);

    if (not dmi_xml_check(session, xmlTextWriterWriteFormatAttribute(
                session->writer,
                dmi_xml_string("value"),
                "0x%" PRIxMAX, iter.mask)))
        return false;

    // Flag codes are not always valid element names (e.g. "5v"), so they are
    // written as attributes
    while ((flag = dmi_format_set_iter_next(&iter)) != nullptr) {
        bool result =
            (dmi_xml_check(session, xmlTextWriterStartElement(session->writer, dmi_xml_string("flag")))) and
            (dmi_xml_check(session, xmlTextWriterWriteAttribute(session->writer, dmi_xml_string("name"),
                                         dmi_xml_string(flag->code)))) and
            (dmi_xml_check(session, xmlTextWriterWriteString(session->writer,
                                      dmi_xml_string(flag->value ? "true" : "false")))) and
            (dmi_xml_check(session, xmlTextWriterEndElement(session->writer)));

        if (not result)
            return false;
    }

    return true;
}

static bool dmi_xml_entity_overlay(
        dmi_xml_session_t          *session,
        const dmi_entity_t         *entity,
        const dmi_entity_overlay_t *overlay)
{
    bool success = false;

    char *value = dmi_format_overlay_value(entity, overlay, session->options.pretty);
    if (value == nullptr)
        return false;

    do {
        if (not dmi_xml_check(session, xmlTextWriterStartElementNS(
                    session->writer,
                    dmi_xml_string(DMI_XML_PREFIX),
                    dmi_xml_string("overlay"),
                    nullptr)))
            break;
        if (not dmi_xml_check(session, xmlTextWriterWriteFormatAttribute(
                    session->writer,
                    dmi_xml_string("source"),
                    "0x%04hx", overlay->source->handle)))
            break;
        if (not dmi_xml_check(session, xmlTextWriterWriteFormatAttribute(
                    session->writer,
                    dmi_xml_string("index"),
                    "%zu", overlay->index)))
            break;
        if (not dmi_xml_check(session, xmlTextWriterWriteFormatAttribute(
                    session->writer,
                    dmi_xml_string("offset"),
                    "0x%02x", overlay->entry->ref_offset)))
            break;
        if (not dmi_xml_check(session, xmlTextWriterStartElementNS(
                    session->writer,
                    dmi_xml_string(DMI_XML_PREFIX),
                    dmi_xml_string("value"),
                    nullptr)))
            break;
        if (not dmi_xml_text(session, value))
            break;
        if (not dmi_xml_check(session, xmlTextWriterFullEndElement(session->writer)))
            break;

        // Element is omitted if the entry has no string
        if (overlay->entry->string != nullptr) {
            if (not dmi_xml_check(session, xmlTextWriterStartElementNS(
                        session->writer,
                        dmi_xml_string(DMI_XML_PREFIX),
                        dmi_xml_string("string"),
                        nullptr)))
                break;
            if (not dmi_xml_text(session, overlay->entry->string))
                break;
            if (not dmi_xml_check(session, xmlTextWriterFullEndElement(session->writer)))
                break;
        }

        if (not dmi_xml_check(session, xmlTextWriterFullEndElement(session->writer)))
            break;

        success = true;
    } while (false);

    dmi_free(value);

    return success;
}
