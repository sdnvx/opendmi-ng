//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_BIS_ENTRY_POINT_INTERNAL_H
#define OPENDMI_ENTITY_BIS_ENTRY_POINT_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/bis-entry-point.h>

// Operation handlers, see bis-entry-point-handlers.c
extern const dmi_attribute_t dmi_bis_real_mode_address_attrs[];
bool dmi_bis_entry_point_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_BIS_ENTRY_POINT_INTERNAL_H
