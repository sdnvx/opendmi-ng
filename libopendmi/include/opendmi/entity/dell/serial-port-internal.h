//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SERIAL_PORT_INTERNAL_H
#define OPENDMI_ENTITY_SERIAL_PORT_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/dell/serial-port.h>

// Value names, see serial-port-names.c
extern const dmi_name_set_t dmi_dell_serial_port_connector_type_names;
extern const dmi_name_set_t dmi_dell_serial_port_caps_names;

// Operation handlers, see serial-port-handlers.c

// Speeds are carried in hundreds of bits per second, see serial-port-handlers.c
uintmax_t dmi_dell_serial_port_convert_speed(uintmax_t raw);

#endif // !OPENDMI_ENTITY_SERIAL_PORT_INTERNAL_H
