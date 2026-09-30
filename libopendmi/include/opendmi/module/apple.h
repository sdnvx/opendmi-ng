//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_MODULE_APPLE_H
#define OPENDMI_MODULE_APPLE_H

#pragma once

#include <opendmi/module.h>

/**
 * @brief Apple SMBIOS structure type identifiers.
 */
typedef enum dmi_apple_type_id
{
    DMI_TYPE_ID_APPLE_FIRMWARE_VOLUME     = 128, ///< Apple: Firmware volume information
    DMI_TYPE_ID_APPLE_MEMORY_SPD_DATA     = 130, ///< Apple: Memory SPD data
    DMI_TYPE_ID_APPLE_PROCESSOR_TYPE      = 131, ///< Apple: Processor type information
    DMI_TYPE_ID_APPLE_PROCESSOR_BUS_SPEED = 132, ///< Apple: Processor and bus speed information
    DMI_TYPE_ID_APPLE_PLATFORM_FEATURE    = 133, ///< Apple: Platform feature information
    DMI_TYPE_ID_APPLE_SMC_VERSION         = 134  ///< Apple: SMC version information
} dmi_apple_type_id_t;

__BEGIN_DECLS

/** @brief Apple: Firmware volume information */
extern __dmi_api const dmi_type_t dmi_type_apple_firmware_volume;

/** @brief Apple: Memory SPD data */
extern __dmi_api const dmi_type_t dmi_type_apple_memory_spd_data;

/** @brief Apple: Processor type information */
extern __dmi_api const dmi_type_t dmi_type_apple_processor_type;

/** @brief Apple: Processor and bus speed information */
extern __dmi_api const dmi_type_t dmi_type_apple_processor_bus_speed;

/** @brief Apple: Platform feature information */
extern __dmi_api const dmi_type_t dmi_type_apple_platform_feature;

/** @brief Apple: SMC version information */
extern __dmi_api const dmi_type_t dmi_type_apple_smc_version;

extern __dmi_api const dmi_module_t dmi_apple_module;

__END_DECLS

#endif // !OPENDMI_MODULE_APPLE_H
