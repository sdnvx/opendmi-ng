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
uintmax_t dmi_memory_controller_convert_size(uintmax_t raw);
bool dmi_memory_controller_derive(dmi_entity_t *entity);
bool dmi_memory_controller_link(dmi_entity_t *entity);
void dmi_memory_controller_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MEMORY_CONTROLLER_INTERNAL_H
