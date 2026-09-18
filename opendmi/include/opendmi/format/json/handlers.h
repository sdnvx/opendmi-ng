//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_FORMAT_JSON_HANDLERS_H
#define OPENDMI_FORMAT_JSON_HANDLERS_H

#pragma once

#include <stdio.h>

#include <opendmi/format/json/types.h>

__BEGIN_DECLS

void *dmi_json_initialize(dmi_context_t *context, FILE *stream, const dmi_format_options_t *options);

bool dmi_json_dump_start(dmi_json_session_t *session);
bool dmi_json_entry(dmi_json_session_t *session);
bool dmi_json_table_start(dmi_json_session_t *session);
bool dmi_json_entity_start(dmi_json_session_t *session, const dmi_entity_t *entity);
bool dmi_json_entity_attrs_start(dmi_json_session_t *session, const dmi_entity_t *entity);

bool dmi_json_entity_attr(
        dmi_json_session_t    *session,
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attr,
        const void            *value);

bool dmi_json_entity_attr_array(
        dmi_json_session_t    *session,
        const dmi_attribute_t *attr,
        const dmi_data_t      *info,
        const void            *value);

bool dmi_json_entity_attr_struct(
        dmi_json_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value);

bool dmi_json_entity_attr_value(
        dmi_json_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value);

bool dmi_json_entity_attr_set(
        dmi_json_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value);

bool dmi_json_entity_attrs_end(dmi_json_session_t *session, const dmi_entity_t *entity);
bool dmi_json_entity_data(dmi_json_session_t *session, const dmi_entity_t *entity);
bool dmi_json_entity_properties(dmi_json_session_t *session, const dmi_entity_t *entity);
bool dmi_json_entity_strings(dmi_json_session_t *session, const dmi_entity_t *entity);
bool dmi_json_entity_end(dmi_json_session_t *session, const dmi_entity_t *entity);
bool dmi_json_table_end(dmi_json_session_t *session);
bool dmi_json_dump_end(dmi_json_session_t *session);

void dmi_json_finalize(dmi_json_session_t *session);

__END_DECLS

#endif // !OPENDMI_FORMAT_JSON_HANDLERS_H
