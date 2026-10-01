//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_POWER_CONTROLS_INTERNAL_H
#define OPENDMI_ENTITY_POWER_CONTROLS_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/power-controls.h>

// Checks the lint rules of the specification perform, see power-controls-rules.c
void dmi_power_controls_lint_bcd(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_POWER_CONTROLS_INTERNAL_H
