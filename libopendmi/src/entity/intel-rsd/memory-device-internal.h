//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INTEL_RSD_MEMORY_DEVICE_INTERNAL_H
#define OPENDMI_ENTITY_INTEL_RSD_MEMORY_DEVICE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/intel-rsd/memory-device.h>

/**
 * @internal
 * @brief Names of the memory types.
 */
extern const dmi_name_set_t dmi_intel_rsd_memory_type_names;

/**
 * @internal
 * @brief Names of the memory media.
 */
extern const dmi_name_set_t dmi_intel_rsd_memory_media_names;

#endif // !OPENDMI_ENTITY_INTEL_RSD_MEMORY_DEVICE_INTERNAL_H
