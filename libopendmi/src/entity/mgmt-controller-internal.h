//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MGMT_CONTROLLER_INTERNAL_H
#define OPENDMI_ENTITY_MGMT_CONTROLLER_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/mgmt-controller.h>

/**
 * @internal
 * @brief Names of the management interface types.
 */
extern const dmi_name_set_t dmi_mgmt_if_type_names;

/**
 * @internal
 * @brief Names of the management protocol types.
 */
extern const dmi_name_set_t dmi_mgmt_proto_names;

/**
 * @internal
 * @brief Names of the network host interface device types.
 */
extern const dmi_name_set_t dmi_mgmt_nhi_device_type_names;

/**
 * @internal
 * @brief Names of the network host interface device characteristics.
 */
extern const dmi_name_set_t dmi_mgmt_nhi_characteristic_names;

/**
 * @internal
 * @brief Names of the Redfish host and service IP address assignment types.
 */
extern const dmi_name_set_t dmi_mgmt_redfish_ip_assignment_names;

/**
 * @internal
 * @brief Names of the Redfish IP address formats.
 */
extern const dmi_name_set_t dmi_mgmt_redfish_ip_format_names;

/**
 * @internal
 * @brief Decode a management controller host interface structure.
 *
 * @details Interface data is read as a whole, and then read again by the
 * interface type. Protocol records are present since SMBIOS 3.2, so a
 * structure ending right after the interface data carries none of them.
 *
 * @param[in,out] decoder Decoder of the structure.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Protocol records or their strings cannot be
 * allocated
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_mgmt_controller_decode(dmi_decoder_t *decoder);

/**
 * @internal
 * @brief Encode a management controller host interface structure.
 *
 * @details Interface data and protocol records are written as the structure
 * holds them, since the ways they are read again by their types are derived
 * from them. Protocol records are present since SMBIOS 3.2.
 *
 * @param[in,out] encoder Encoder of the structure.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_mgmt_controller_encode(dmi_encoder_t *encoder);

/**
 * @internal
 * @brief Free the protocol records and the strings of a decoded structure.
 *
 * @param[in,out] entity Structure being cleaned up.
 */
void dmi_mgmt_controller_cleanup(dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the protocol records fit the structure.
 *
 * @details Records follow each other, each one carrying its own length, so
 * they all fit the structure only if the lengths agree with it.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_mgmt_controller_lint_records(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MGMT_CONTROLLER_INTERNAL_H
