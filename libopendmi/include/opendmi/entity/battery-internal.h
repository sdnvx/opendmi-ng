//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_BATTERY_INTERNAL_H
#define OPENDMI_ENTITY_BATTERY_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/battery.h>

// Value names, see battery-names.c
extern const dmi_name_set_t dmi_battery_chemistry_names;

// Operation handlers, see battery-handlers.c
bool dmi_battery_decode_date(
        dmi_entity_t      *entity,
        const dmi_field_t *field,
        void              *value);
bool dmi_battery_decode_sbds_date(
        dmi_entity_t      *entity,
        const dmi_field_t *field,
        void              *value);
bool dmi_battery_decode_capacity_factor(
        dmi_entity_t      *entity,
        const dmi_field_t *field,
        void              *value);

// Checks the lint rules of the specification perform, see battery-rules.c
void dmi_battery_lint_sbds(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_BATTERY_INTERNAL_H
