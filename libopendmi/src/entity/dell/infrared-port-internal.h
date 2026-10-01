//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INFRARED_PORT_INTERNAL_H
#define OPENDMI_ENTITY_INFRARED_PORT_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/dell/infrared-port.h>

// Value names, see infrared-port-names.c
extern const dmi_name_set_t dmi_dell_infrared_proto_names;

// Speeds are carried in hundreds of bits per second, see infrared-port-handlers.c
bool dmi_dell_infrared_port_decode_speed(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);
bool dmi_dell_infrared_port_encode_speed(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

#endif // !OPENDMI_ENTITY_INFRARED_PORT_INTERNAL_H
