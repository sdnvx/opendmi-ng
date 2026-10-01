//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_ARRAY_ADDR_INTERNAL_H
#define OPENDMI_ENTITY_MEMORY_ARRAY_ADDR_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/memory-array-addr.h>

/**
 * @internal
 * @brief Check that the address range of a structure is well-formed.
 *
 * @details The extended addresses are used if, and only if, both of the
 * 32-bit ones are `0xFFFFFFFF`, and the end of a range is above its start.
 *
 * @param[in] entity Structure being validated.
 *
 * @return `true` if the structure is valid, `false` otherwise.
 */
bool dmi_memory_array_addr_validate(dmi_entity_t *entity);

/**
 * @internal
 * @brief Derive the size of a memory array address range.
 *
 * @details Size of the range is what its bounds say, in whichever order the
 * data happens to carry them, and both of the bounds belong to the range.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_memory_array_addr_derive(dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the address range of a memory array does not start
 * after its end.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_memory_array_addr_lint_range(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the address range of a memory array does not overlap
 * the ranges of the other arrays.
 *
 * @details Ranges of the arrays describe the physical address space of the
 * system, so no two of them may claim the same addresses.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_memory_array_addr_lint_overlap(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MEMORY_ARRAY_ADDR_INTERNAL_H
