//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_CHASSIS_INTERNAL_H
#define OPENDMI_ENTITY_CHASSIS_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/chassis.h>

// Value names, see chassis-names.c
extern const dmi_name_set_t dmi_chassis_type_names;
extern const dmi_name_set_t dmi_chassis_security_status_names;
extern const dmi_name_set_t dmi_rack_type_names;

// Operation handlers, see chassis-handlers.c
bool dmi_chassis_decode_element_type(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);
bool dmi_chassis_encode_element_type(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);
bool dmi_chassis_decode_maximum_count(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);
bool dmi_chassis_encode_maximum_count(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

void dmi_chassis_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_CHASSIS_INTERNAL_H
