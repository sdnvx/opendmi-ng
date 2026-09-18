//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_FORMAT_XML_HANDLERS_H
#define OPENDMI_FORMAT_XML_HANDLERS_H

#pragma once

#include <opendmi/format/xml/types.h>

__BEGIN_DECLS

void *dmi_xml_initialize(dmi_context_t *context, FILE *stream, const dmi_format_options_t *options);

bool dmi_xml_dump_start(dmi_xml_session_t *session);
bool dmi_xml_entry(dmi_xml_session_t *session);
bool dmi_xml_entity_start(dmi_xml_session_t *session, const dmi_entity_t *entity);
bool dmi_xml_entity_attrs_start(dmi_xml_session_t *session, const dmi_entity_t *entity);

bool dmi_xml_entity_attr(
        dmi_xml_session_t     *session,
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attr,
        const void            *value);

bool dmi_xml_entity_attr_array(
        dmi_xml_session_t     *session,
        const dmi_attribute_t *attr,
        const dmi_data_t      *info,
        const void            *value);

bool dmi_xml_entity_attr_struct(
        dmi_xml_session_t     *session,
        const dmi_attribute_t *attr,
        const void            *value);

bool dmi_xml_entity_attr_value(
        dmi_xml_session_t     *session,
        const dmi_attribute_t *attr,
        const void            *value);

bool dmi_xml_entity_attr_set(
        dmi_xml_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value);

bool dmi_xml_entity_attrs_end(dmi_xml_session_t *session, const dmi_entity_t *entity);
bool dmi_xml_entity_data(dmi_xml_session_t *session, const dmi_entity_t *entity);
bool dmi_xml_entity_properties(dmi_xml_session_t *session, const dmi_entity_t *entity);
bool dmi_xml_entity_strings(dmi_xml_session_t *session, const dmi_entity_t *entity);
bool dmi_xml_entity_end(dmi_xml_session_t *session, const dmi_entity_t *entity);
bool dmi_xml_dump_end(dmi_xml_session_t *session);

void dmi_xml_finalize(dmi_xml_session_t *session);

__END_DECLS

#endif // !OPENDMI_FORMAT_XML_HANDLERS_H
