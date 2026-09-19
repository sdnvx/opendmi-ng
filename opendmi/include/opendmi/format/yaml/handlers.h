//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_FORMAT_YAML_HANDLERS_H
#define OPENDMI_FORMAT_YAML_HANDLERS_H

#pragma once

#include <opendmi/format/yaml/types.h>

__BEGIN_DECLS

void *dmi_yaml_initialize(dmi_context_t *context, FILE *stream, const dmi_format_options_t *options);

bool dmi_yaml_dump_start(dmi_yaml_session_t *session);
bool dmi_yaml_entry(dmi_yaml_session_t *session);
bool dmi_yaml_table_start(dmi_yaml_session_t *session);
bool dmi_yaml_entity_start(dmi_yaml_session_t *session, const dmi_entity_t *entity);
bool dmi_yaml_entity_attrs_start(dmi_yaml_session_t *session, const dmi_entity_t *entity);

bool dmi_yaml_entity_attr(
        dmi_yaml_session_t    *session,
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attr,
        const void            *value);

bool dmi_yaml_entity_attr_array(
        dmi_yaml_session_t    *session,
        const dmi_attribute_t *attr,
        const dmi_data_t      *info,
        const void            *value);

bool dmi_yaml_entity_attr_struct(
        dmi_yaml_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value);

bool dmi_yaml_entity_attr_value(
        dmi_yaml_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value);

bool dmi_yaml_entity_attr_set(
        dmi_yaml_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value);

bool dmi_yaml_entity_attrs_end(dmi_yaml_session_t *session, const dmi_entity_t *entity);
bool dmi_yaml_entity_data(dmi_yaml_session_t *session, const dmi_entity_t *entity);
bool dmi_yaml_entity_properties(dmi_yaml_session_t *session, const dmi_entity_t *entity);
bool dmi_yaml_entity_overlays(dmi_yaml_session_t *session, const dmi_entity_t *entity);
bool dmi_yaml_entity_strings(dmi_yaml_session_t *session, const dmi_entity_t *entity);
bool dmi_yaml_entity_end(dmi_yaml_session_t *session, const dmi_entity_t *entity);
bool dmi_yaml_table_end(dmi_yaml_session_t *session);
bool dmi_yaml_dump_end(dmi_yaml_session_t *session);

void dmi_yaml_finalize(dmi_yaml_session_t *session);

__END_DECLS

#endif // !OPENDMI_FORMAT_YAML_HANDLERS_H
