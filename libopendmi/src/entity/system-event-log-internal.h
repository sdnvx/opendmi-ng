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

/**
 * @internal
 * @brief Variant of the access method address for an indexed I/O access
 * method, whose address contains I/O ports.
 *
 * @param __method Access method the variant is shown for.
 */
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

/**
 * @internal
 * @brief Length of a descriptor of the supported log types the specification
 * defines.
 */
#define DMI_SYSTEM_EVENT_LOG_DESCRIPTOR_LENGTH 2

/**
 * @internal
 * @brief Names of the access methods of an event log.
 */
extern const dmi_name_set_t dmi_system_log_access_method_names;

/**
 * @internal
 * @brief Names of the status bits of an event log.
 */
extern const dmi_name_set_t dmi_system_log_status_names;

/**
 * @internal
 * @brief Names of the header formats of an event log.
 */
extern const dmi_name_set_t dmi_system_log_header_format_names;

/**
 * @internal
 * @brief Names of the event log types.
 */
extern const dmi_name_set_t dmi_event_log_type_names;

/**
 * @internal
 * @brief Names of the variable data formats of a log record.
 */
extern const dmi_name_set_t dmi_event_log_data_format_names;

/**
 * @internal
 * @brief Attributes of a descriptor of the supported log types, which is a log
 * type and a data format.
 */
extern const dmi_attribute_t dmi_system_log_type_descriptor_attrs[];

/**
 * @internal
 * @brief Attributes of the I/O ports of an indexed I/O access method, which
 * are an index port and a data port.
 */
extern const dmi_attribute_t dmi_system_log_io_ports_attrs[];

/**
 * @internal
 * @brief Derive the I/O ports and the GPNV handle from the access method
 * address of an event log.
 *
 * @details Access method address is interpreted according to the access
 * method, so every interpretation is derived, and the one matching the
 * method is shown.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_system_event_log_derive(dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the header and the data of an event log start within its
 * area.
 *
 * @details Header and data both live in the area of the log, which is what its
 * length covers. Logs with an area length of zero are not checked.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_system_event_log_lint_area(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the descriptors of the supported log types are of the
 * length the specification defines.
 *
 * @details Length is read from the structure itself, since the decoder keeps
 * the descriptors rather than their layout.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_system_event_log_lint_descriptors(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_SYSTEM_EVENT_LOG_INTERNAL_H
