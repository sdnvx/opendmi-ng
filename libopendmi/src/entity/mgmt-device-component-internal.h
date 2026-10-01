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

/**
 * @internal
 * @brief Resolve the management device, the component and the threshold of
 * a management device component to the structures they refer to.
 *
 * @details Management device and component are required, while the
 * threshold is optional. Units of the threshold values are set by the type of
 * the component.
 *
 * @param[in,out] entity Structure being linked.
 *
 * @error DMI_ERROR_ENTITY_NOT_FOUND Management device or component is not
 * specified
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_mgmt_device_component_link(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MGMT_DEVICE_COMPONENT_INTERNAL_H
