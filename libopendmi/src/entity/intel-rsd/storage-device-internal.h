//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INTEL_RSD_STORAGE_DEVICE_INTERNAL_H
#define OPENDMI_ENTITY_INTEL_RSD_STORAGE_DEVICE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/intel-rsd/storage-device.h>

/**
 * @internal
 * @brief Names of the connector types of a storage device.
 */
extern const dmi_name_set_t dmi_intel_rsd_storage_connector_names;

/**
 * @internal
 * @brief Names of the protocols of a storage device.
 */
extern const dmi_name_set_t dmi_intel_rsd_storage_proto_names;

/**
 * @internal
 * @brief Names of the storage device types.
 */
extern const dmi_name_set_t dmi_intel_rsd_storage_device_type_names;

#endif // !OPENDMI_ENTITY_INTEL_RSD_STORAGE_DEVICE_INTERNAL_H
