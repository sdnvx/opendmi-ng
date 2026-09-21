//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_BASEBOARD_INTERNAL_H
#define OPENDMI_ENTITY_BASEBOARD_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/baseboard.h>

// Value names, see baseboard-names.c
extern const dmi_name_set_t dmi_baseboard_type_names;
extern const dmi_name_set_t dmi_baseboard_feature_names;

// Operation handlers, see baseboard-handlers.c
bool dmi_baseboard_link(dmi_entity_t *entity);
void dmi_baseboard_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_BASEBOARD_INTERNAL_H
