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

// Value names, see power-supply-names.c
extern const dmi_name_set_t dmi_power_supply_type_names;
extern const dmi_name_set_t dmi_range_switching_type_names;

// Operation handlers, see power-supply-handlers.c
bool dmi_power_supply_link(dmi_entity_t *entity);

// Checks the lint rules of the specification perform, see power-supply-rules.c
void dmi_power_supply_lint_probes(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_POWER_SUPPLY_INTERNAL_H
