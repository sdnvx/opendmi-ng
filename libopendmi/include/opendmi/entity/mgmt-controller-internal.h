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

// Value names, see mgmt-controller-names.c
extern const dmi_name_set_t dmi_mgmt_if_type_names;
extern const dmi_name_set_t dmi_mgmt_proto_names;
extern const dmi_name_set_t dmi_mgmt_nhi_device_type_names;
extern const dmi_name_set_t dmi_mgmt_nhi_characteristic_names;
extern const dmi_name_set_t dmi_mgmt_redfish_ip_assignment_names;
extern const dmi_name_set_t dmi_mgmt_redfish_ip_format_names;

// Operation handlers, see mgmt-controller-handlers.c
bool dmi_mgmt_controller_decode(dmi_entity_t *entity);
bool dmi_mgmt_controller_encode(dmi_writer_t *writer);
void dmi_mgmt_controller_cleanup(dmi_entity_t *entity);

// Checks the lint rules of the specification perform, see
// mgmt-controller-rules.c
void dmi_mgmt_controller_lint_records(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MGMT_CONTROLLER_INTERNAL_H
