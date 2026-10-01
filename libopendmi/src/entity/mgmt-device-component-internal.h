//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MGMT_DEVICE_COMPONENT_INTERNAL_H
#define OPENDMI_ENTITY_MGMT_DEVICE_COMPONENT_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/mgmt-device-component.h>

// Operation handlers, see mgmt-device-component-handlers.c
bool dmi_mgmt_device_component_link(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MGMT_DEVICE_COMPONENT_INTERNAL_H
