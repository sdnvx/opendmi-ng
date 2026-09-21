//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_RSD_PHYS_DEVICE_MAPPING_INTERNAL_H
#define OPENDMI_ENTITY_RSD_PHYS_DEVICE_MAPPING_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/intel/rsd-phys-device-mapping.h>

/**
 * @brief Size of the device location in the structure.
 */
#define DMI_INTEL_RSD_PHYS_DEVICE_SIZE 4

// Value names, see rsd-phys-device-mapping-names.c
extern const dmi_name_set_t dmi_intel_rsd_phys_device_type_names;

// Operation handlers, see rsd-phys-device-mapping-handlers.c
extern const dmi_attribute_t dmi_intel_rsd_phys_device_attrs[];
bool dmi_intel_rsd_phys_device_mapping_derive(dmi_entity_t *entity);
void dmi_intel_rsd_phys_device_mapping_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_RSD_PHYS_DEVICE_MAPPING_INTERNAL_H
