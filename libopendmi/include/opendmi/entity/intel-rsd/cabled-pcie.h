//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INTEL_RSD_CABLED_PCIE_H
#define OPENDMI_ENTITY_INTEL_RSD_CABLED_PCIE_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/entity/common.h>

typedef struct dmi_intel_rsd_cabled_pcie dmi_intel_rsd_cabled_pcie_t;
typedef struct dmi_intel_rsd_cabled_pcie_port dmi_intel_rsd_cabled_pcie_port_t;

/**
 * @brief Intel RSD cabled PCIe port information (type 199).
 *
 * Intel Rack Scale Design (RSD) OEM structure describing one cabled PCIe port
 * on the fixed side of the system, typically provided by a PCIe cable adapter
 * that is either on board or plugged into a regular PCIe slot. Firmware
 * provides the structure only when the platform has cabled PCIe ports, one
 * per Cable Management Interface (CMI) controller of such a port.
 *
 * @note The BMC and the firmware are expected to use the same slot ID for a
 * given physical slot; how they agree on it, and on the link widths, is left
 * to the platform.
 */
struct dmi_intel_rsd_cabled_pcie
{
    /**
     * @brief PCIe slot ID to which this specific Cabled PCIe slot is connected.
     *
     * This information is used by the BMC to identify and address a specific
     * Cabled PCIe slot or CMI controller. This matches the slot ID field
     * defined in the System Slots SMBIOS table (type 9) as defined by the
     * SMBIOS specification.
     *
     * It is required that this field also matches with the slot ID that the
     * BMC uses for the same slot. This field will be passed into the Get Cable
     * EEPROM Data IPMI command to identify the specific cable port to fetch
     * data.
     */
    dmi_pci_slot_t pci_slot_id;

    /**
     * @brief Indicates the link width of this specific Cable Port. Typically
     * `4` to indicate x4 cable port, `8` to indicate a x8 cable port etc.
     */
    unsigned link_width;

    /**
     * @brief Number of cable indices and corresponding PCIe lane ranges
     * available within this Cable Port.
     */
    size_t port_count;

    /**
     * @brief Cable indices properties, an array of `port_count` elements.
     */
    dmi_intel_rsd_cabled_pcie_port_t *ports;
};

/**
 * @brief Cable index of an Intel RSD cabled PCIe port.
 *
 * Tells which x4 group of lanes of the port a cable index covers.
 */
struct dmi_intel_rsd_cabled_pcie_port
{
    /**
     * @brief 0-based index that identifies a specific cable port index within
     * the PCIe slot identified by the Slot ID field above.
     *
     * It is required that the field matches the port index the BMC uses for
     * this port. This field will be passed into the Get Cable EEPROM Data IPMI
     * command to identify the specific cable port to fetch data.
     */
    unsigned index;

    /**
     * @brief One of `0`, `4`, `8` or `12` to indicate the starting lane of the
     * x4 lane to which the specific cable port index applies.
     */
    unsigned start_lane;
};

/**
 * @brief Intel RSD cabled PCIe port information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_intel_rsd_cabled_pcie_spec;

#endif // !OPENDMI_ENTITY_INTEL_RSD_CABLED_PCIE_H
