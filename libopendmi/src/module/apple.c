//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/module/apple.h>

// Structure types of the module
const dmi_type_t dmi_type_apple_firmware_volume     = { .id = DMI_TYPE_ID(APPLE_FIRMWARE_VOLUME)     };
const dmi_type_t dmi_type_apple_memory_spd_data     = { .id = DMI_TYPE_ID(APPLE_MEMORY_SPD_DATA)     };
const dmi_type_t dmi_type_apple_processor_type      = { .id = DMI_TYPE_ID(APPLE_PROCESSOR_TYPE)      };
const dmi_type_t dmi_type_apple_processor_bus_speed = { .id = DMI_TYPE_ID(APPLE_PROCESSOR_BUS_SPEED) };
const dmi_type_t dmi_type_apple_platform_feature    = { .id = DMI_TYPE_ID(APPLE_PLATFORM_FEATURE)    };
const dmi_type_t dmi_type_apple_smc_version         = { .id = DMI_TYPE_ID(APPLE_SMC_VERSION)         };

/**
 * @brief Apple extension module.
 */
const dmi_module_t dmi_apple_module =
{
    .code      = "apple",
    .name      = "Apple extensions",
    .entities  = nullptr,
    .platforms = DMI_PLATFORMS({
        { .firmware_vendor = DMI_VENDOR_APPLE },
        DMI_PLATFORM_NULL
    })
};
