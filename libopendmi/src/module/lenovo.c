//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/module/lenovo.h>

#include <opendmi/entity/lenovo/tvt.h>

/**
 * @brief Lenovo extension module.
 */
const dmi_module_t dmi_lenovo_module =
{
    .code      = "lenovo",
    .name      = "IBM/Lenovo extensions",
    .entities  = (const dmi_entity_spec_t *[]){
        &dmi_lenovo_tvt_spec,
        nullptr
    },
    .platforms = DMI_PLATFORMS({
        { .firmware_vendor = DMI_VENDOR_IBM    },
        { .firmware_vendor = DMI_VENDOR_LENOVO },
        DMI_PLATFORM_NULL
    })
};
