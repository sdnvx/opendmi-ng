//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SLOT_INTERNAL_H
#define OPENDMI_ENTITY_SLOT_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/slot.h>

// Value names, see slot-names.c
extern const dmi_name_set_t dmi_slot_type_names;
extern const dmi_name_set_t dmi_slot_width_names;
extern const dmi_name_set_t dmi_slot_length_names;
extern const dmi_name_set_t dmi_slot_feature_names;
extern const dmi_name_set_t dmi_slot_feature_ex_names;
extern const dmi_name_set_t dmi_slot_height_names;
extern const dmi_name_set_t dmi_slot_usage_names;

// Operation handlers, see slot-handlers.c
void dmi_slot_cleanup(dmi_entity_t *entity);

// Checks the lint rules of the specification perform, see slot-rules.c
void dmi_slot_lint_width(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_SLOT_INTERNAL_H
