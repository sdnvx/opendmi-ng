//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_NIC_H
#define OPENDMI_ENTITY_HPE_NIC_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_HPE_NIC_INFO_T
#   define DMI_HPE_NIC_INFO_T
    typedef struct dmi_hpe_nic_info dmi_hpe_nic_info_t;
#endif // !DMI_HPE_NIC_INFO_T

#ifndef DMI_HPE_NIC_PORT_T
#   define DMI_HPE_NIC_PORT_T
    typedef struct dmi_hpe_nic_port dmi_hpe_nic_port_t;
#endif // !DMI_HPE_NIC_PORT_T

/**
 * @brief State of a network port.
 */
typedef enum dmi_hpe_nic_state
{
    DMI_HPE_NIC_STATE_INSTALLED     = 0x00, ///< Installed
    DMI_HPE_NIC_STATE_DISABLED      = 0x01, ///< Disabled
    DMI_HPE_NIC_STATE_NOT_INSTALLED = 0x02  ///< Not installed
} dmi_hpe_nic_state_t;

/**
 * @brief Network port the firmware boots from.
 */
struct dmi_hpe_nic_port
{
    /**
     * @brief PCI device and function, `(device << 3) | function`.
     */
    uint8_t devfn;

    /**
     * @brief PCI bus.
     */
    uint8_t bus;

    /**
     * @brief MAC address.
     */
    dmi_binary_t mac_address;

    /**
     * @brief State of the port: disabled if the bus and the device are zero,
     * and not installed if they have all bits set.
     */
    dmi_hpe_nic_state_t state;
};

/**
 * @brief HP/HPE BIOS PXE NIC PCI and MAC information (type 209), and BIOS
 * iSCSI NIC PCI and MAC information (type 221) up to G7.
 *
 * Lists the network ports the firmware boots from over PXE or iSCSI.
 */
struct dmi_hpe_nic_info
{
    /**
     * @brief Number of ports.
     */
    size_t port_count;

    /**
     * @brief Ports.
     */
    dmi_hpe_nic_port_t *ports;
};

/**
 * @brief HP/HPE BIOS PXE NIC PCI and MAC information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_pxe_nic_spec;

/**
 * @brief HP/HPE BIOS iSCSI NIC PCI and MAC information entity specification.
 *
 * Type 221 is given to the iSCSI ports up to G7, and is deprecated later.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_iscsi_nic_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_hpe_nic_state_name(dmi_hpe_nic_state_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_HPE_NIC_H
