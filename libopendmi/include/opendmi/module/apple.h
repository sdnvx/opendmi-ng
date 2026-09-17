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
typedef enum dmi_apple_type
{
    DMI_TYPE_APPLE_FIRMWARE_VOLUME     = 128, ///< Apple: Firmware volume information
    DMI_TYPE_APPLE_MEMORY_SPD_DATA     = 130, ///< Apple: Memory SPD data
    DMI_TYPE_APPLE_PROCESSOR_TYPE      = 131, ///< Apple: Processor type information
    DMI_TYPE_APPLE_PROCESSOR_BUS_SPEED = 132, ///< Apple: Processor and bus speed information
    DMI_TYPE_APPLE_PLATFORM_FEATURE    = 133, ///< Apple: Platform feature information
    DMI_TYPE_APPLE_SMC_VERSION         = 134  ///< Apple: SMC version information
} dmi_apple_type_t;

__BEGIN_DECLS

extern const dmi_module_t dmi_apple_module;

__END_DECLS

#endif // !OPENDMI_MODULE_APPLE_H
