//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INFRARED_PORT_INTERNAL_H
#define OPENDMI_ENTITY_INFRARED_PORT_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/dell/infrared-port.h>

/**
 * @internal
 * @brief Names of the infrared port protocols.
 */
extern const dmi_name_set_t dmi_dell_infrared_proto_names;

/**
 * @internal
 * @brief Decode the speed of the port into bits per second.
 *
 * @details Speeds are carried in hundreds of bits per second.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Variable to store the speed in.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_dell_infrared_port_decode_speed(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Encode the speed of the port into hundreds of bits per second,
 * which undoes `dmi_dell_infrared_port_decode_speed()`.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Variable holding the speed in bits per second.
 * @param[out] data  Data the field is to carry.
 *
 * @return Always `true`.
 */
bool dmi_dell_infrared_port_encode_speed(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

#endif // !OPENDMI_ENTITY_INFRARED_PORT_INTERNAL_H
