//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_FORMAT_TEXT_HANDLERS_H
#define OPENDMI_FORMAT_TEXT_HANDLERS_H

#pragma once

#include <opendmi/format/text/types.h>

__BEGIN_DECLS

void *dmi_text_initialize(dmi_context_t *context, FILE *stream, const dmi_format_options_t *options);

bool dmi_text_entry(dmi_text_session_t *session);
bool dmi_text_entity_start(dmi_text_session_t *session, const dmi_entity_t *entity);

bool dmi_text_entity_attr(
        dmi_text_session_t    *session,
        const dmi_entity_t    *entity,
        const dmi_attribute_t *attr,
        const void            *value);

void dmi_text_entity_attr_array(
        dmi_text_session_t    *session,
        const dmi_attribute_t *attr,
        const dmi_data_t      *info,
        const void            *value);

void dmi_text_entity_attr_struct(
       dmi_text_session_t     *session,
        const dmi_attribute_t *attr,
        const void            *value);

void dmi_text_entity_attr_value(
        dmi_text_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value,
        const char            *descr);

void dmi_text_entity_attr_set(
        dmi_text_session_t    *session,
        const dmi_attribute_t *attr,
        const void            *value);

bool dmi_text_entity_data(dmi_text_session_t *session, const dmi_entity_t *entity);
bool dmi_text_entity_strings(dmi_text_session_t *session, const dmi_entity_t *entity);

bool dmi_text_entity_end(dmi_text_session_t *session, const dmi_entity_t *entity);
void dmi_text_finalize(dmi_text_session_t *session);

__END_DECLS

#endif // !OPENDMI_FORMAT_TEXT_HANDLERS_H
