//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_CONTROLLER_INTERNAL_H
#define OPENDMI_ENTITY_MEMORY_CONTROLLER_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/memory-controller.h>

// Value names, see memory-controller-names.c
extern const dmi_name_set_t dmi_error_detect_method_names;
extern const dmi_name_set_t dmi_error_correct_caps_names;
extern const dmi_name_set_t dmi_memory_module_speed_names;
extern const dmi_name_set_t dmi_memory_interleave_names;
extern const dmi_name_set_t dmi_memory_module_voltage_names;

// Operation handlers, see memory-controller-handlers.c
bool dmi_memory_controller_decode_size(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);
bool dmi_memory_controller_encode_size(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

bool dmi_memory_controller_derive(dmi_entity_t *entity);
bool dmi_memory_controller_link(dmi_entity_t *entity);
void dmi_memory_controller_cleanup(dmi_entity_t *entity);

// Checks the lint rules of the specification perform, see memory-controller-rules.c
void dmi_memory_controller_lint_module_size(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MEMORY_CONTROLLER_INTERNAL_H
