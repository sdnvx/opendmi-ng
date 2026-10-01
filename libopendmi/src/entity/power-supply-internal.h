//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_POWER_SUPPLY_INTERNAL_H
#define OPENDMI_ENTITY_POWER_SUPPLY_INTERNAL_H

#pragma once

#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/power-supply.h>

/**
 * @internal
 * @brief Names of the power supply types.
 */
extern const dmi_name_set_t dmi_power_supply_type_names;

/**
 * @internal
 * @brief Names of the input voltage range switching types.
 */
extern const dmi_name_set_t dmi_range_switching_type_names;

/**
 * @internal
 * @brief Check that the voltage probe, the cooling device and the current
 * probe handles refer to structures of the expected types.
 *
 * @details Handles which are unset or unsupported, and ones referring to
 * missing structures, are not checked.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_power_supply_lint_probes(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_POWER_SUPPLY_INTERNAL_H
