//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_NIC_MAC_H
#define OPENDMI_ENTITY_HPE_NIC_MAC_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/entity/hpe/nic.h>

typedef struct dmi_hpe_nic_mac dmi_hpe_nic_mac_t;

/**
 * @brief HP/HPE NIC PCI and MAC information (type 233).
 *
 * Describes a network port the firmware boots from over PXE, one per
 * structure, which replaces type 209 on the newer servers.
 */
struct dmi_hpe_nic_mac
{
    /**
     * @brief PCI segment group, zero for a single group topology.
     */
    uint16_t segment;

    /**
     * @brief PCI bus.
     */
    uint8_t bus;

    /**
     * @brief PCI device and function, `(device << 3) | function`.
     */
    uint8_t devfn;

    /**
     * @brief MAC address.
     */
    dmi_binary_t mac_address;

    /**
     * @brief State of the port: disabled if the bus and the device are zero,
     * and not installed if they have all bits set.
     */
    dmi_hpe_nic_state_t state;

    /**
     * @brief Number of the port. Set to `UINT8_MAX` when the structure holds
     * none.
     */
    uint8_t port;

    /**
     * @brief UEFI device path of the port.
     */
    const char *uefi_device_path;
};

/**
 * @brief HP/HPE NIC PCI and MAC information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_nic_mac_spec;

#endif // !OPENDMI_ENTITY_HPE_NIC_MAC_H
