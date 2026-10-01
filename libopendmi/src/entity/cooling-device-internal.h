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

bool dmi_cooling_device_encode_speed(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

// Checks the lint rules of the specification perform, see cooling-device-rules.c
void dmi_cooling_device_lint_probe(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_COOLING_DEVICE_INTERNAL_H
