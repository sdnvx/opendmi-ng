//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_MODULE_INTERNAL_H
#define OPENDMI_ENTITY_MEMORY_MODULE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/memory-module.h>

// Value names, see memory-module-names.c
extern const dmi_name_set_t dmi_memory_module_type_names;
extern const dmi_name_set_t dmi_memory_module_size_status_names;
extern const dmi_name_set_t dmi_memory_module_error_names;

// Operation handlers, see memory-module-handlers.c
bool dmi_memory_module_decode_installed_size(
        dmi_entity_t      *entity,
        const dmi_field_t *field,
        void              *value);
bool dmi_memory_module_decode_enabled_size(
        dmi_entity_t      *entity,
        const dmi_field_t *field,
        void              *value);

#endif // !OPENDMI_ENTITY_MEMORY_MODULE_INTERNAL_H
