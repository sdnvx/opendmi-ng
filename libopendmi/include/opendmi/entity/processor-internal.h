//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_PROCESSOR_INTERNAL_H
#define OPENDMI_ENTITY_PROCESSOR_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/processor.h>

/**
 * @internal
 * @brief Value of the family telling that the actual one is in the extended
 * field, which the names, the decoder and the rules all go by.
 */
#define DMI_PROCESSOR_FAMILY_EXTENDED 0xFE

/**
 * @internal
 * @brief Bits of a CPUID signature which are reserved, and thus zero in a
 * valid one. The decoder puts swapped identifier words back in place by
 * them, and the rule reports the same condition.
 */
#define DMI_PROCESSOR_ID_RESERVED 0xF000C000u

// Value names, see processor-names.c
extern const dmi_name_set_t dmi_processor_type_names;
extern const dmi_name_set_t dmi_processor_family_names;
extern const dmi_name_set_t dmi_processor_status_names;
extern const dmi_name_set_t dmi_processor_voltage_names;
extern const dmi_name_set_t dmi_processor_upgrade_names;
extern const dmi_name_set_t dmi_processor_features_names;
extern const dmi_name_set_t dmi_processor_x86_feature_names;

// Operation handlers, see processor-handlers.c
bool dmi_processor_decode_voltage(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);
bool dmi_processor_encode_voltage(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);
bool dmi_processor_derive(dmi_entity_t *entity);

// Checks the lint rules of the specification perform, see processor-rules.c
void dmi_processor_lint_cores(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_processor_lint_speed(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_processor_lint_cache(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_processor_lint_family(dmi_lint_t *lint, const dmi_entity_t *entity);
void dmi_processor_lint_id(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_PROCESSOR_INTERNAL_H
