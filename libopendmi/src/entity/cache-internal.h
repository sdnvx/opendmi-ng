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
bool dmi_cache_decode_level(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);
bool dmi_cache_decode_size(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);
bool dmi_cache_decode_size_ex(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);
bool dmi_cache_encode_level(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);
bool dmi_cache_encode_size(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);
bool dmi_cache_encode_size_ex(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

// Checks the lint rules of the specification perform, see cache-rules.c
void dmi_cache_lint_size(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_cache_lint_sram(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_CACHE_INTERNAL_H
