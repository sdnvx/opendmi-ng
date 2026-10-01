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

extern const dmi_name_set_t dmi_mgmt_if_type_names;
extern const dmi_name_set_t dmi_mgmt_proto_names;
extern const dmi_name_set_t dmi_mgmt_nhi_device_type_names;
extern const dmi_name_set_t dmi_mgmt_nhi_characteristic_names;
extern const dmi_name_set_t dmi_mgmt_redfish_ip_assignment_names;
extern const dmi_name_set_t dmi_mgmt_redfish_ip_format_names;

bool dmi_mgmt_controller_decode(dmi_decoder_t *decoder);

/**
 * @internal
 * @brief Encode a management controller host interface structure.
 *
 * @details Interface data and protocol records are written as the structure
 * holds them, since the ways they are read again by their types are derived
 * from them. Protocol records are present since SMBIOS 3.2.
 */
bool dmi_mgmt_controller_encode(dmi_encoder_t *encoder);

void dmi_mgmt_controller_cleanup(dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the protocol records fit the structure.
 *
 * @details Records follow each other, each one carrying its own length, so
 * they all fit the structure only if the lengths agree with it.
 */
void dmi_mgmt_controller_lint_records(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MGMT_CONTROLLER_INTERNAL_H
