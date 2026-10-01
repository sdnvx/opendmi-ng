//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_DEVICE_CORRELATION_INTERNAL_H
#define OPENDMI_ENTITY_HPE_DEVICE_CORRELATION_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/hpe/device-correlation.h>

/**
 * @internal
 * @brief Names of the types of a correlated device.
 */
extern const dmi_name_set_t dmi_hpe_device_type_names;

/**
 * @internal
 * @brief Names of the locations of a correlated device.
 */
extern const dmi_name_set_t dmi_hpe_device_location_names;

#endif // !OPENDMI_ENTITY_HPE_DEVICE_CORRELATION_INTERNAL_H
