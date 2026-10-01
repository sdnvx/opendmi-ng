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

extern const dmi_name_set_t dmi_slot_type_names;
extern const dmi_name_set_t dmi_slot_width_names;
extern const dmi_name_set_t dmi_slot_length_names;
extern const dmi_name_set_t dmi_slot_feature_names;
extern const dmi_name_set_t dmi_slot_feature_ex_names;
extern const dmi_name_set_t dmi_slot_height_names;
extern const dmi_name_set_t dmi_slot_usage_names;


/**
 * @internal
 * @brief Check that the slot is at least as wide as its bus.
 *
 * @details A card of the width of the bus has to fit the slot physically, so
 * the slot is at least as wide as its bus.
 */
void dmi_slot_lint_width(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_SLOT_INTERNAL_H
