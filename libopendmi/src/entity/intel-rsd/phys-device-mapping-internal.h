//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INTEL_RSD_PHYS_DEVICE_MAPPING_INTERNAL_H
#define OPENDMI_ENTITY_INTEL_RSD_PHYS_DEVICE_MAPPING_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/intel-rsd/phys-device-mapping.h>

/**
 * @internal
 * @brief Size of the device location in the structure.
 */
#define DMI_INTEL_RSD_PHYS_DEVICE_SIZE 4

/**
 * @internal
 * @brief Names of the types of the physical devices.
 */
extern const dmi_name_set_t dmi_intel_rsd_phys_device_type_names;

/**
 * @internal
 * @brief Attributes of a device location.
 *
 * @details Location numbers are named according to the device type, and the
 * data of unknown device types is shown as stored.
 */
extern const dmi_attribute_t dmi_intel_rsd_phys_device_attrs[];

/**
 * @internal
 * @brief Derive the type and the location numbers of every device of the
 * structure.
 *
 * @details Every device of a structure is of the type the structure declares,
 * and the location data means what that type says it does. The data of
 * unknown device types is marked to be shown as stored.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_intel_rsd_phys_device_mapping_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_INTEL_RSD_PHYS_DEVICE_MAPPING_INTERNAL_H
