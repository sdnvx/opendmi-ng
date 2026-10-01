//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_CHASSIS_INTERNAL_H
#define OPENDMI_ENTITY_CHASSIS_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/chassis.h>

extern const dmi_name_set_t dmi_chassis_type_names;
extern const dmi_name_set_t dmi_chassis_security_status_names;
extern const dmi_name_set_t dmi_rack_type_names;

/**
 * @internal
 * @brief Decode the type of a contained element.
 *
 * @details An element is named by either an SMBIOS structure type or a
 * baseboard type, which the most significant bit of the field tells apart.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Element to store the type in.
 *
 * @return Always `true`.
 */
bool dmi_chassis_decode_element_type(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Encode the type of a contained element, which undoes
 * `dmi_chassis_decode_element_type()`.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Element to take the type from.
 * @param[out] data  Data the field is to carry.
 *
 * @return Always `true`.
 */
bool dmi_chassis_encode_element_type(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @internal
 * @brief Decode the maximum number of a contained element.
 *
 * @details Elements which may be there in any number carry a maximum of
 * zero, which the specification reserves for saying that there is no
 * maximum. Such a maximum is decoded as `UINTMAX_MAX`.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Variable to store the maximum in.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_chassis_decode_maximum_count(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Encode the maximum number of a contained element, which undoes
 * `dmi_chassis_decode_maximum_count()`.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Variable holding the maximum.
 * @param[out] data  Data the field is to carry.
 *
 * @return Always `true`.
 */
bool dmi_chassis_encode_maximum_count(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

#endif // !OPENDMI_ENTITY_CHASSIS_INTERNAL_H
