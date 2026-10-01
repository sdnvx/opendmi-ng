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

/**
 * @internal
 * @brief Names of the types of a cooling device.
 */
extern const dmi_name_set_t dmi_cooling_device_type_names;

/**
 * @internal
 * @brief Decode the nominal speed of a cooling device.
 *
 * @details Speeds are carried in revolutions per minute, with the most
 * significant bit set aside, and the value of exactly 0x8000 stands for
 * "unknown".
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Variable to store the speed in.
 *
 * @return `true` if the data has been decoded, `false` otherwise.
 */
bool dmi_cooling_device_decode_speed(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Encode the nominal speed of a cooling device, which undoes
 * `dmi_cooling_device_decode_speed()`.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Variable holding the speed.
 * @param[out] data  Data the field is to carry.
 *
 * @return Always `true`.
 */
bool dmi_cooling_device_encode_speed(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @internal
 * @brief Check that the temperature probe handle of a cooling device refers
 * to a temperature probe.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_cooling_device_lint_probe(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_COOLING_DEVICE_INTERNAL_H
