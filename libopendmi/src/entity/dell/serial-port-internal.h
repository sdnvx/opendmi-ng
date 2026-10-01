//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SERIAL_PORT_INTERNAL_H
#define OPENDMI_ENTITY_SERIAL_PORT_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/dell/serial-port.h>

/**
 * @internal
 * @brief Names of the connector types of a serial port.
 */
extern const dmi_name_set_t dmi_dell_serial_port_connector_type_names;

/**
 * @internal
 * @brief Names of the capabilities of a serial port.
 */
extern const dmi_name_set_t dmi_dell_serial_port_caps_names;

/**
 * @internal
 * @brief Decode the speed of a serial port.
 *
 * @details Speeds are carried in hundreds of bits per second.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Variable to store the speed in.
 *
 * @return `true` if the data has been decoded, `false` otherwise.
 */
bool dmi_dell_serial_port_decode_speed(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Encode the speed of a serial port, which undoes
 * `dmi_dell_serial_port_decode_speed()`.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Variable holding the speed.
 * @param[out] data  Data the field is to carry.
 *
 * @return Always `true`.
 */
bool dmi_dell_serial_port_encode_speed(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

#endif // !OPENDMI_ENTITY_SERIAL_PORT_INTERNAL_H
