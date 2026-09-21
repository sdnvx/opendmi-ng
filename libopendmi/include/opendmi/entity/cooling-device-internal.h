//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_COOLING_DEVICE_INTERNAL_H
#define OPENDMI_ENTITY_COOLING_DEVICE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/cooling-device.h>

// Value names, see cooling-device-names.c
extern const dmi_name_set_t dmi_cooling_device_type_names;

// Operation handlers, see cooling-device-handlers.c
uintmax_t dmi_cooling_device_convert_speed(uintmax_t raw);
bool dmi_cooling_device_link(dmi_entity_t *entity);

// Checks the lint rules of the specification perform, see cooling-device-rules.c
void dmi_cooling_device_lint_probe(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_COOLING_DEVICE_INTERNAL_H
