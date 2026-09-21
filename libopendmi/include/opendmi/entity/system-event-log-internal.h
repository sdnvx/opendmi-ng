//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SYSTEM_EVENT_LOG_INTERNAL_H
#define OPENDMI_ENTITY_SYSTEM_EVENT_LOG_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/system-event-log.h>

//
// Access method address of indexed I/O access methods contains I/O ports
//
#define dmi_system_log_io_ports_variant(__method)                            \
    DMI_VARIANT(__method, dmi_system_event_log_t, access_ports, STRUCT, {    \
        .attrs = dmi_system_log_io_ports_attrs                               \
    })

/**
 * @internal
 * @brief Offset of the length of a descriptor of the supported log types,
 * which the specification fixes at two bytes.
 */
#define DMI_SYSTEM_EVENT_LOG_DESCRIPTOR_OFFSET 0x16

#define DMI_SYSTEM_EVENT_LOG_DESCRIPTOR_LENGTH 2

// Value names, see system-event-log-names.c
extern const dmi_name_set_t dmi_system_log_access_method_names;
extern const dmi_name_set_t dmi_system_log_status_names;
extern const dmi_name_set_t dmi_system_log_header_format_names;
extern const dmi_name_set_t dmi_event_log_type_names;
extern const dmi_name_set_t dmi_event_log_data_format_names;

// Operation handlers, see system-event-log-handlers.c
extern const dmi_attribute_t dmi_system_log_type_descriptor_attrs[];
extern const dmi_attribute_t dmi_system_log_io_ports_attrs[];
bool dmi_system_event_log_derive(dmi_entity_t *entity);
void dmi_system_event_log_cleanup(dmi_entity_t *entity);

// Checks the lint rules of the specification perform, see system-event-log-rules.c
void dmi_system_event_log_lint_area(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_system_event_log_lint_descriptors(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_SYSTEM_EVENT_LOG_INTERNAL_H
