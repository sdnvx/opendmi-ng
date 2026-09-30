//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/module/cisco.h>
#include <opendmi/entity/cisco/pci-adapter.h>
#include <opendmi/entity/cisco/slot-buses.h>

// Structure types of the module
const dmi_type_t dmi_type_cisco_slot_buses  = { .id = DMI_TYPE_ID(CISCO_SLOT_BUSES)  };
const dmi_type_t dmi_type_cisco_pci_adapter = { .id = DMI_TYPE_ID(CISCO_PCI_ADAPTER) };

/**
 * @brief Cisco extension module.
 *
 * Structures of the firmware of Cisco UCS servers. Other Cisco appliances
 * carry the firmware of other vendors, e.g. Dell, and are left to their
 * modules.
 */
const dmi_module_t dmi_cisco_module =
{
    .code      = "cisco",
    .name      = "Cisco extensions",
    .entities  = (const dmi_entity_spec_t *[]){
        &dmi_cisco_slot_buses_spec,
        &dmi_cisco_pci_adapter_spec,
        nullptr
    },
    .platforms = DMI_PLATFORMS({
        { .firmware_vendor = DMI_VENDOR_CISCO },
        DMI_PLATFORM_NULL
    })
};
