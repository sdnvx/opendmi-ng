//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DMI_ENTITY_INTEL_RSD_NETWORK_CARD_H
#define DMI_ENTITY_INTEL_RSD_NETWORK_CARD_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/entity/common.h>

/**
 * @brief Length of MAC address field in bytes.
 */
#define DMI_INTEL_RSD_MAC_ADDRESS_LENGTH 32

typedef struct dmi_intel_rsd_network_card dmi_intel_rsd_network_card_t;

/**
 * @brief Intel RSD Network card information (type 190).
 */
struct dmi_intel_rsd_network_card
{
    /**
     * @brief PCI class code is used to identify the generic function of the
     * device.
     */
    dmi_pci_class_t pci_class;

    /**
     * @brief Physical slot number of the slot connected to the PCIe port.
     */
    dmi_pci_slot_t pci_slot_id;

    /**
     * @brief Vendor identification number.
     */
    dmi_pci_vendor_id_t vendor_id;

    /**
     * @brief Device identification number.
     */
    dmi_pci_device_id_t device_id;

    /**
     * @brief Sub-vendor identifier used to differentiate specific model.
     */
    dmi_pci_vendor_id_t sub_vendor_id;

    /**
     * @brief Sub-device identifier used to differentiate specific model.
     */
    dmi_pci_device_id_t sub_device_id;

    /**
     * @brief Maximum speed in Mbps (for example 10000 for 10 Gbps, 40000 for
     * 40 Gbps).
     */
    uint32_t maximum_speed;

    /**
     * @brief Current speed in Mbps (for example 10000 for 10 Gbps, 40000 for
     * 40 Gbps).
     */
    uint32_t current_speed;

    /**
     * @brief Port index in the system. This value matches the value exposed by
     * the BIOS in the IPMI specification's device instance selector field
     * definition for `Get/Set System Boot Options` IPMI command (Boot options
     * parameter structure, Boot flags parameter 5 data 5).
     */
    uint16_t port_index;

    /**
     * @brief Network MAC address, `DMI_INTEL_RSD_MAC_ADDRESS_LENGTH` bytes.
     */
    dmi_binary_t mac_address;

    /**
     * @brief The string that identifies the version of the firmware installed
     * on the NIC.
     */
    const char *firmware_version;
};

/**
 * @brief Intel RSD Network card information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_intel_rsd_network_card_spec;

#endif // !DMI_ENTITY_INTEL_RSD_NETWORK_CARD_H
