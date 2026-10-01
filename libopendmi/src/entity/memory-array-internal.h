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
 * @brief Offset of the maximum capacity.
 */
#define DMI_MEMORY_ARRAY_CAPACITY_OFFSET 0x07

/**
 * @internal
 * @brief Value of the maximum capacity telling that the actual one is in the
 * extended field.
 */
#define DMI_MEMORY_ARRAY_CAPACITY_OFFSET_EXTENDED 0x80000000

/**
 * @internal
 * @brief Length of a structure carrying the extended maximum capacity.
 */
#define DMI_MEMORY_ARRAY_CAPACITY_OFFSET_LENGTH 0x17

extern const dmi_name_set_t dmi_memory_array_location_names;
extern const dmi_name_set_t dmi_memory_array_usage_names;

/**
 * @internal
 * @brief Count the memory devices of an array and sum up their sizes.
 *
 * @details Sum of the sizes of the devices of an array, along with their
 * number, which both rules of the array are checked against.
 *
 * @param[in]     lint     Check in progress.
 * @param[in]     entity   Memory array.
 * @param[in,out] capacity Variable to add the sizes of the devices to, or
 *                         `nullptr`.
 *
 * @return Number of the devices of the array.
 */
size_t dmi_memory_array_devices(
        dmi_lint_t         *lint,
        const dmi_entity_t *entity,
        dmi_size_t         *capacity);

void dmi_memory_array_lint_device_count(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_memory_array_lint_capacity(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_memory_array_lint_extended_capacity(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MEMORY_ARRAY_INTERNAL_H
