//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_DEVICE_ADDR_INTERNAL_H
#define OPENDMI_ENTITY_MEMORY_DEVICE_ADDR_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/memory-device-addr.h>

bool dmi_memory_device_addr_validate(dmi_entity_t *entity);

/**
 * @internal
 * @brief Derive the size of the address range of a memory device.
 *
 * @details Size of the range is what its bounds say, in whichever order the
 * data happens to carry them, and both of the bounds belong to the range.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_memory_device_addr_derive(dmi_entity_t *entity);

void dmi_memory_device_addr_lint_range(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the address range of a memory device lies within the
 * range of the memory array it belongs to.
 *
 * @details A device is mapped within the range of the array it belongs to,
 * since the array is what the range of the device is carved out of.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_memory_device_addr_lint_bounds(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MEMORY_DEVICE_ADDR_INTERNAL_H
