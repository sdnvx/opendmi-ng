//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/module/ami.h>
#include <opendmi/entity/ami/firewire-guid.h>

// Structure types of the module
const dmi_type_t dmi_type_ami_firewire_guid = { .id = DMI_TYPE_ID(AMI_FIREWIRE_GUID) };

/**
 * @brief AMI extension module.
 */
const dmi_module_t dmi_ami_module =
{
    .code      = "ami",
    .name      = "AMI extensions",
    .entities  = (const dmi_entity_spec_t *[]){
        &dmi_ami_firewire_guid_spec,
        nullptr
    },
    .platforms = DMI_PLATFORMS({
        { .firmware_vendor = DMI_VENDOR_AMI },
        DMI_PLATFORM_NULL
    })
};
