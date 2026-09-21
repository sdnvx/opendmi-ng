//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_CACHE_INTERNAL_H
#define OPENDMI_ENTITY_CACHE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/cache.h>

// Value names, see cache-names.c
extern const dmi_name_set_t dmi_cache_type_names;
extern const dmi_name_set_t dmi_cache_mode_names;
extern const dmi_name_set_t dmi_cache_assoc_names;
extern const dmi_name_set_t dmi_cache_location_names;
extern const dmi_name_set_t dmi_cache_sram_type_names;

// Operation handlers, see cache-handlers.c
uintmax_t dmi_cache_convert_level(uintmax_t raw);
uintmax_t dmi_cache_convert_size(uintmax_t raw);
uintmax_t dmi_cache_convert_size_ex(uintmax_t raw);

// Checks the lint rules of the specification perform, see cache-rules.c
void dmi_cache_lint_size(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_cache_lint_sram(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_CACHE_INTERNAL_H
