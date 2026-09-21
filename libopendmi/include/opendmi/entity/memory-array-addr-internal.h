//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_ARRAY_ADDR_INTERNAL_H
#define OPENDMI_ENTITY_MEMORY_ARRAY_ADDR_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/memory-array-addr.h>

// Operation handlers, see memory-array-addr-handlers.c
bool dmi_memory_array_addr_validate(dmi_entity_t *entity);
bool dmi_memory_array_addr_derive(dmi_entity_t *entity);
bool dmi_memory_array_addr_link(dmi_entity_t *entity);

// Checks the lint rules of the specification perform, see memory-array-addr-rules.c
void dmi_memory_array_addr_lint_range(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_memory_array_addr_lint_overlap(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MEMORY_ARRAY_ADDR_INTERNAL_H
