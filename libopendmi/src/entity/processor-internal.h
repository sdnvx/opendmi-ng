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
 * @brief Bits of a CPUID signature which are reserved, and thus zero in a
 * valid one. The decoder puts swapped identifier words back in place by
 * them, and the rule reports the same condition.
 */
#define DMI_PROCESSOR_ID_RESERVED 0xF000C000u

/**
 * @internal
 * @brief Names of the processor types.
 */
extern const dmi_name_set_t dmi_processor_type_names;

/**
 * @internal
 * @brief Names of the processor families.
 */
extern const dmi_name_set_t dmi_processor_family_names;

/**
 * @internal
 * @brief Names of the statuses of a processor.
 */
extern const dmi_name_set_t dmi_processor_status_names;

/**
 * @internal
 * @brief Names of the voltages a processor supports.
 */
extern const dmi_name_set_t dmi_processor_voltage_names;

/**
 * @internal
 * @brief Names of the processor upgrades, which are the socket types.
 */
extern const dmi_name_set_t dmi_processor_upgrade_names;

/**
 * @internal
 * @brief Names of the processor characteristics.
 */
extern const dmi_name_set_t dmi_processor_features_names;

/**
 * @internal
 * @brief Feature flags of x86 processors, as returned by CPUID leaf 1 in EDX
 * register.
 */
extern const dmi_name_set_t dmi_processor_x86_feature_names;

/**
 * @internal
 * @brief Decode the voltage of a processor.
 *
 * @details One byte carries either the current voltage or the ones the
 * processor supports, which the most significant bit of it tells apart.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Structure to store the voltage in.
 *
 * @return `true` if the data has been decoded, `false` otherwise.
 */
bool dmi_processor_decode_voltage(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Encode the voltage of a processor.
 *
 * @details Voltage is written as the current one whenever there is one, and
 * as the supported ones otherwise.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Structure holding the voltage.
 * @param[out] data  Data the field is to carry.
 *
 * @return `true` if the value has been encoded, `false` otherwise.
 */
bool dmi_processor_encode_voltage(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @internal
 * @brief Decode the identifier of a processor.
 *
 * @details Processor identifier is taken as stored, and means what the family
 * and the vendor say it does, so it is read once the fields are there.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_processor_derive(dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the numbers of the cores and threads of a processor
 * agree: no more of them are enabled than there are, and there are no fewer
 * threads than cores.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_processor_lint_cores(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that a processor does not run faster than its maximum speed.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_processor_lint_speed(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the cache handles of a processor refer to the caches of
 * the levels they stand for.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_processor_lint_cache(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the family of a processor does not refer to the extended
 * family, which the structure does not carry.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_processor_lint_family(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the identifier of a processor does not have its words
 * swapped.
 *
 * @details Firmware of some vendors stores the feature flags before the
 * signature, which the decoder puts back in place. The swap is told by the
 * bits the signature reserves, since a valid signature has none of them set.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_processor_lint_id(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_PROCESSOR_INTERNAL_H
