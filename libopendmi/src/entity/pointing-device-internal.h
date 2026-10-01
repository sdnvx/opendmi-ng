//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_POINTING_DEVICE_INTERNAL_H
#define OPENDMI_ENTITY_POINTING_DEVICE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/pointing-device.h>

/**
 * @internal
 * @brief Names of the types of the pointing devices.
 */
extern const dmi_name_set_t dmi_pointing_device_type_names;

/**
 * @internal
 * @brief Names of the interfaces of the pointing devices.
 */
extern const dmi_name_set_t dmi_pointing_device_iface_names;

#endif // !OPENDMI_ENTITY_POINTING_DEVICE_INTERNAL_H
