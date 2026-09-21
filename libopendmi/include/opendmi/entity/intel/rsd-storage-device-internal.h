//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_RSD_STORAGE_DEVICE_INTERNAL_H
#define OPENDMI_ENTITY_RSD_STORAGE_DEVICE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/intel/rsd-storage-device.h>

// Value names, see rsd-storage-device-names.c
extern const dmi_name_set_t dmi_intel_rsd_storage_connector_names;
extern const dmi_name_set_t dmi_intel_rsd_storage_proto_names;
extern const dmi_name_set_t dmi_intel_rsd_storage_device_type_names;

#endif // !OPENDMI_ENTITY_RSD_STORAGE_DEVICE_INTERNAL_H
