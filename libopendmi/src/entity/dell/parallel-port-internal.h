//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_PARALLEL_PORT_INTERNAL_H
#define OPENDMI_ENTITY_PARALLEL_PORT_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/dell/parallel-port.h>

/**
 * @internal
 * @brief Names of the connector types of a parallel port.
 */
extern const dmi_name_set_t dmi_dell_parallel_port_connector_type_names;

/**
 * @internal
 * @brief Names of the connector pinouts of a parallel port.
 */
extern const dmi_name_set_t dmi_dell_parallel_port_connector_pinout_names;

/**
 * @internal
 * @brief Names of the capabilities of a parallel port.
 */
extern const dmi_name_set_t dmi_dell_parallel_port_caps_names;

#endif // !OPENDMI_ENTITY_PARALLEL_PORT_INTERNAL_H
