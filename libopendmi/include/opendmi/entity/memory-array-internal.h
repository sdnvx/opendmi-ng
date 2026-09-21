//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_ARRAY_INTERNAL_H
#define OPENDMI_ENTITY_MEMORY_ARRAY_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/memory-array.h>

/**
 * @internal
 * @brief Offset of the maximum capacity, the value telling that the actual one is in the
 * extended field, and the length of a structure carrying that field.
 */
#define DMI_MEMORY_ARRAY_CAPACITY_OFFSET 0x07

#define DMI_MEMORY_ARRAY_CAPACITY_OFFSET_EXTENDED 0x80000000

#define DMI_MEMORY_ARRAY_CAPACITY_OFFSET_LENGTH 0x17

// Value names, see memory-array-names.c
extern const dmi_name_set_t dmi_memory_array_location_names;
extern const dmi_name_set_t dmi_memory_array_usage_names;

// Operation handlers, see memory-array-handlers.c
size_t dmi_memory_array_devices(
        dmi_lint_t         *lint,
        const dmi_entity_t *entity,
        dmi_size_t         *capacity);

// Checks the lint rules of the specification perform, see memory-array-rules.c
void dmi_memory_array_lint_device_count(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_memory_array_lint_capacity(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_memory_array_lint_extended_capacity(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MEMORY_ARRAY_INTERNAL_H
