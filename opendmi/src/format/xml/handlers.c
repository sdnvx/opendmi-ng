//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <time.h>
#include <inttypes.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/error.h>
#include <opendmi/utils.h>

#include <opendmi/format/iter.h>
#include <opendmi/format/xml/handlers.h>
#include <opendmi/format/xml/helpers.h>

#ifdef _WIN32
static inline struct tm *gmtime_r(const time_t *timep, struct tm *result)
{
    errno_t err = gmtime_s(result, timep);
    if (err != 0)
        return nullptr;
    return result;
}
#endif

void *dmi_xml_initialize(dmi_context_t *context, FILE *stream)
{
    assert(context != nullptr);
    assert(stream != nullptr);

    bool success = false;
    dmi_xml_session_t *session;

    session = dmi_alloc(context, sizeof(*session));
    if (session == nullptr) {
        dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
        return nullptr;
    }

    do {
        session->buffer = xmlOutputBufferCreateFile(stream, nullptr);
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

    session->context = context;
    session->stream  = stream;

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

        if (xmlTextWriterStartDocument(session->writer, "1.0", "UTF-8", NULL) < 0)
            break;

        if (xmlTextWriterStartElementNS(
                    session->writer,
                    dmi_xml_string(DMI_XML_PREFIX),
                    dmi_xml_string("dump"),
                    dmi_xml_string(DMI_XML_NAMESPACE)) < 0)
            break;
        if (xmlTextWriterWriteFormatAttribute(
                    session->writer,
                    dmi_xml_string("created-at"),
                    "%04u-%02u-%02uT%02u:%02u:%02u",
                    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                    tm.tm_hour, tm.tm_min, tm.tm_sec) < 0)
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

        if (xmlTextWriterStartElementNS(
                    session->writer,
                    dmi_xml_string(DMI_XML_PREFIX),
                    dmi_xml_string("entry"),
                    nullptr) < 0)
            break;

        if (xmlTextWriterWriteAttribute(
                    session->writer,
                    dmi_xml_string("smbios-version"),
                    dmi_xml_string(smbios_version)) < 0)
            break;
        if (xmlTextWriterWriteFormatAttribute(
                    session->writer,
                    dmi_xml_string("table-area-address"),
                    "0x%" PRIx64, context->state.table_area_addr) < 0)
            break;
        if (xmlTextWriterWriteFormatAttribute(
                    session->writer,
                    dmi_xml_string("table-area-size"),
                    "%zu", context->state.table_area_size) < 0)
            break;

        if (xmlTextWriterEndElement(session->writer) < 0)
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
        if (xmlTextWriterStartElementNS(
                    session->writer,
                    dmi_xml_string(DMI_XML_PREFIX),
                    dmi_xml_string("entity"),
                    nullptr) < 0)
            break;

        if (xmlTextWriterWriteFormatAttribute(
                    session->writer,
                    dmi_xml_string("handle"),
                    "0x%04hx", entity->handle) < 0)
            break;
        if (xmlTextWriterWriteFormatAttribute(
                    session->writer,
                    dmi_xml_string("type"),
                    "%u", entity->type) < 0)
            break;
        if (xmlTextWriterWriteFormatAttribute(
                    session->writer,
                    dmi_xml_string("length"),
                    "%zu", entity->total_length) < 0)
            break;

        success = true;
    } while (false);

    return success;
}

bool dmi_xml_entity_attrs_start(dmi_xml_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    if (xmlTextWriterStartElementNS(
                session->writer,
                dmi_xml_string(DMI_XML_PREFIX),
                dmi_xml_string(entity->spec->code),
                nullptr) < 0)
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
    assert(value != nullptr);

    bool success = false;

    do {
        bool rv;

        if (xmlTextWriterStartElement(session->writer, dmi_xml_string(attr->params.code)) < 0)
            break;

        if (not dmi_member_is_present(attr->counter)) {
            if (attr->type == DMI_ATTRIBUTE_TYPE_STRUCT)
                rv = dmi_xml_entity_attr_struct(session, attr, value);
            else
                rv = dmi_xml_entity_attr_value(session, attr, value);
        } else {
            rv = dmi_xml_entity_attr_array(session, attr, entity->info, value);
        }
        if (not rv)
            break;

        if (xmlTextWriterFullEndElement(session->writer) < 0)
           break;

        success = true;
    } while (false);

    return success;
}

bool dmi_xml_entity_attr_array(
        dmi_xml_session_t     *session,
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

    while ((ptr = dmi_format_array_iter_next(&iter)) != nullptr) {
        if (xmlTextWriterStartElement(session->writer, dmi_xml_string("item")) < 0)
            return false;

        if (attr->type == DMI_ATTRIBUTE_TYPE_STRUCT) {
            if (not dmi_xml_entity_attr_struct(session, attr, ptr))
                return false;
        } else {
            if (not dmi_xml_entity_attr_value(session, attr, ptr))
                return false;
        }

        if (xmlTextWriterFullEndElement(session->writer) < 0)
            return false;
    }

    return true;
}

bool dmi_xml_entity_attr_struct(
        dmi_xml_session_t     *session,
        const dmi_attribute_t *attr,
        const void            *value)
{
    assert(session != nullptr);
    assert(attr != nullptr);
    assert(value != nullptr);

    const dmi_attribute_t *child_attr = nullptr;

    for (child_attr = attr->params.attrs; child_attr->params.name; child_attr++) {
        const dmi_data_t *ptr = dmi_member_ptr(value, child_attr->value, dmi_data_t);

        if (xmlTextWriterStartElement(session->writer, dmi_xml_string(child_attr->params.code)) < 0)
            return false;

        if (not dmi_xml_entity_attr_value(session, child_attr, ptr))
            return false;

        if (xmlTextWriterFullEndElement(session->writer) < 0)
            return false;
    }

    return true;
}

bool dmi_xml_entity_attr_value(
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
        if (xmlTextWriterWriteString(session->writer, dmi_xml_string("unknown")) < 0)
            return false;
        return true;
    }

    // Handle value sets
    if (attr->type == DMI_ATTRIBUTE_TYPE_SET)
        return dmi_xml_entity_attr_set(session, attr, value);

    do {
        text = dmi_attribute_format(session->context, attr, value, false);
        if (text == nullptr)
            break;

        if (attr->params.unit) {
            if (xmlTextWriterWriteAttribute(
                        session->writer,
                        dmi_xml_string("units"),
                        dmi_xml_string(attr->params.unit)) < 0)
                break;
        }

        if (not dmi_xml_text(session, text))
            break;

        success = true;
    } while (false);

    dmi_free(text);

    return success;
}

bool dmi_xml_entity_attr_set(
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

    if (xmlTextWriterWriteFormatAttribute(
                session->writer,
                dmi_xml_string("value"),
                "0x%" PRIxMAX, iter.mask) < 0)
        return false;

    // Flag codes are not always valid element names (e.g. "5v"), so they are
    // written as attributes
    while ((flag = dmi_format_set_iter_next(&iter)) != nullptr) {
        bool result =
            (xmlTextWriterStartElement(session->writer, dmi_xml_string("flag")) >= 0) and
            (xmlTextWriterWriteAttribute(session->writer, dmi_xml_string("name"),
                                         dmi_xml_string(flag->code)) >= 0) and
            (xmlTextWriterWriteString(session->writer,
                                      dmi_xml_string(flag->value ? "true" : "false")) >= 0) and
            (xmlTextWriterEndElement(session->writer) >= 0);

        if (not result)
            return false;
    }

    return true;
}

bool dmi_xml_entity_attrs_end(dmi_xml_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    dmi_unused(entity);

    if (xmlTextWriterFullEndElement(session->writer) < 0)
        return false;

    return true;
}

bool dmi_xml_entity_data(dmi_xml_session_t *session, const dmi_entity_t *entity)
{
    assert(session != nullptr);
    assert(entity != nullptr);

    bool success = false;

    do {
        if (xmlTextWriterStartElementNS(
                    session->writer,
                    dmi_xml_string(DMI_XML_PREFIX),
                    dmi_xml_string("data"),
                    nullptr) < 0)
            break;

        if (not dmi_xml_data(session, entity->data, entity->body_length))
            break;

        if (xmlTextWriterFullEndElement(session->writer) < 0)
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
        if (xmlTextWriterStartElementNS(
                    session->writer,
                    dmi_xml_string(DMI_XML_PREFIX),
                    dmi_xml_string("strings"),
                    nullptr) < 0)
            break;

        dmi_format_string_iter_t iter;
        const char *str;

        dmi_format_string_iter_init(&iter, entity);

        while ((str = dmi_format_string_iter_next(&iter)) != nullptr) {
            if (xmlTextWriterStartElementNS(
                        session->writer,
                        dmi_xml_string(DMI_XML_PREFIX),
                        dmi_xml_string("string"),
                        nullptr) < 0)
                break;
            if (xmlTextWriterWriteFormatAttribute(
                        session->writer,
                        dmi_xml_string("index"),
                        "%zu", iter.index) < 0)
                break;

            if (not dmi_xml_text(session, str))
                break;

            if (xmlTextWriterFullEndElement(session->writer) < 0)
                break;
        }

        // Loop is interrupted on errors
        if (str != nullptr)
            break;

        if (xmlTextWriterFullEndElement(session->writer) < 0)
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
        if (xmlTextWriterFullEndElement(session->writer) < 0)
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
        if (xmlTextWriterEndDocument(session->writer) < 0)
            break;
        if (xmlTextWriterFlush(session->writer) < 0)
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
