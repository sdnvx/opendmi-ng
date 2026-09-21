//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_DEVICE_ADDR_INTERNAL_H
#define OPENDMI_ENTITY_MEMORY_DEVICE_ADDR_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/memory-device-addr.h>

// Operation handlers, see memory-device-addr-handlers.c
bool dmi_memory_device_addr_validate(dmi_entity_t *entity);
bool dmi_memory_device_addr_derive(dmi_entity_t *entity);
bool dmi_memory_device_addr_link(dmi_entity_t *entity);

// Checks the lint rules of the specification perform, see memory-device-addr-rules.c
void dmi_memory_device_addr_lint_range(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_memory_device_addr_lint_bounds(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MEMORY_DEVICE_ADDR_INTERNAL_H
