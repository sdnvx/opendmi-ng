//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_DEVICE_INTERNAL_H
#define OPENDMI_ENTITY_MEMORY_DEVICE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/memory-device.h>

// Value names, see memory-device-names.c
extern const dmi_name_set_t dmi_memory_device_type_names;
extern const dmi_name_set_t dmi_memory_device_type_detail_names;
extern const dmi_name_set_t dmi_memory_device_form_factor_names;
extern const dmi_name_set_t dmi_memory_device_tech_names;

// Sizes are carried in granules of their own, and in a field of the width the
// plain one is too narrow for, see memory-device-handlers.c
bool dmi_memory_device_decode_size(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);
bool dmi_memory_device_decode_size_ex(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);
bool dmi_memory_device_encode_size(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);
bool dmi_memory_device_encode_size_ex(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

// Checks the lint rules of the specification perform, see memory-device-rules.c
void dmi_memory_device_lint_extended_size(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_memory_device_lint_width(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_memory_device_lint_speed(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_memory_device_lint_voltage(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_memory_device_lint_sizes(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MEMORY_DEVICE_INTERNAL_H
