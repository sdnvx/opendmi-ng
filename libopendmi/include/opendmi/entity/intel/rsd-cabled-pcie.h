//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DMI_ENTITY_INTEL_RSD_CABLED_PCIE_PORT_H
#define DMI_ENTITY_INTEL_RSD_CABLED_PCIE_PORT_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/entity/common.h>

typedef struct dmi_intel_rsd_cabled_pcie dmi_intel_rsd_cabled_pcie_t;
typedef struct dmi_intel_rsd_cabled_pcie_port dmi_intel_rsd_cabled_pcie_port_t;

/**
 * @brief Intel RSD cabled PCIe port information (type 199).
 *
 * SMBIOS OEM type 199 is used to declare the Fixed Side Cabled PCle Port
 * information on the system. The structure is defined in Table 15. UEFI FW
 * shall implement this structure only if the Cabled PCle ports are present
 * on the platform. Typically, Cabled PCle ports are implemented in a system
 * using the PCle Cable Adapters that are either present onboard or attached
 * to an existing PCle slot via an adapter card (often with a re-timer). This
 * structure describes each Cable Management Interface (CMI) controller within
 * a Cabled PCle port.
 *
 * @note Platform BMC and BIOS must use the same value for the Slot ID for a
 * given physical slot represented in the structure below. Determination and
 * communication between Platform BMC and BIOS of which specific PCle slot in
 * the system has Cabled PCle ports and determination of link widths, specific
 * Slot ID etc. are out of the scope of this specification.
 *
 * Intel RSD requires that cabled PCle ports are designed following the PCI
 * Express External Cabling Specification, Revision 3.0 as specified in the
 * specification-referenced in Table 2. Intel® RSD also requires systems
 * implementing cabled PCle ports to support SMBus accessibility to the slots
 * by the system BMC to allow for cable identification and management.
 */
struct dmi_intel_rsd_cabled_pcie
{
    /**
     * @brief PCIe slot ID to which this specific Cabled PCIe slot is connected.
     *
     * This information used to identify and address specific Cabled PCIe slot
     * or CMI controller by the BMC. This matches the slot ID field defined in
     * the System Slots SMBIOS table (type 9) as defined by the SMBIOS
     * specification.
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
    unsigned port_count;

    /**
     * @brief Cable indices properties.
     */
    dmi_intel_rsd_cabled_pcie_port_t *ports;
};

struct dmi_intel_rsd_cabled_pcie_port
{
    /**
     * @brief 0-based index that identifies a specific cable port index within
     * the PCIe slot identified by the Slot ID field above.
     *
     * It is required that field match the port index the BMC uses for this
     * port. This field will be passed into the Get Cable EEPROM Data IPMI
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

#endif // !DMI_ENTITY_INTEL_RSD_CABLED_PCIE_PORT_H
