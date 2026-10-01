//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_BATTERY_INTERNAL_H
#define OPENDMI_ENTITY_BATTERY_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/battery.h>

/**
 * @internal
 * @brief Names of the battery chemistries.
 */
extern const dmi_name_set_t dmi_battery_chemistry_names;

/**
 * @internal
 * @brief Decode the SBDS manufacture date of a battery.
 *
 * @details SBDS date packs the year counted from 1980, the month and the day
 * into one word, and zero stands for no date.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Variable to store the date in.
 *
 * @return `true` if the data has been decoded, `false` otherwise.
 */
bool dmi_battery_decode_sbds_date(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Encode the SBDS manufacture date of a battery, which undoes
 * `dmi_battery_decode_sbds_date()`.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Variable holding the date.
 * @param[out] data  Data the field is to carry.
 *
 * @return Always `true`.
 */
bool dmi_battery_encode_sbds_date(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @internal
 * @brief Derive the manufacture date and the design capacity of a battery.
 *
 * @details Manufacture date is the one the string spells, or the SBDS one
 * when the string spells none, and the capacity is the design one times its
 * multiplier.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_battery_derive(dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that a battery provides either the SBDS values or its own
 * ones, and not both.
 *
 * @details The specification puts the SBDS values in place of the ones the
 * structure carries itself, so a battery provides either of them, and not
 * both.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_battery_lint_sbds(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_BATTERY_INTERNAL_H
