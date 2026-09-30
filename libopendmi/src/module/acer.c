//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/module/acer.h>
#include <opendmi/entity/acer/hotkeys.h>

/**
 * @brief Acer extension module.
 */
const dmi_module_t dmi_acer_module =
{
    .code      = "acer",
    .name      = "Acer extensions",
    .entities  = (const dmi_entity_spec_t *[]){
        &dmi_acer_hotkeys_spec,
        nullptr
    },
    //
    // Acer laptops carry the firmware of other vendors, e.g. Insyde, and are
    // told by the system information
    //
    .platforms = DMI_PLATFORMS({
        { .firmware_vendor = DMI_VENDOR_ACER },
        { .system_vendor   = DMI_VENDOR_ACER },
        DMI_PLATFORM_NULL
    })
};
