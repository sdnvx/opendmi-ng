//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_CISCO_PCI_ADAPTER_H
#define OPENDMI_ENTITY_CISCO_PCI_ADAPTER_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/entity/common.h>

typedef struct dmi_cisco_pci_adapter dmi_cisco_pci_adapter_t;

/**
 * @brief Cisco PCI adapter information (type 202).
 *
 * Describes an adapter of a Cisco UCS server: its PCI identifiers, its class,
 * its model, the slot it is installed in, and the version of its firmware.
 * Reverse engineered from the data corpus, whose identifiers match the ones
 * of the PCI ID database and the model names the structures give.
 *
 * The structure holds the identifiers in the big-endian byte order, which
 * the `raw_` fields keep, and the other fields give in the order of the
 * host.
 */
struct dmi_cisco_pci_adapter
{
    /**
     * @brief Version of the structure, presumably, 3 in all known data.
     */
    uint8_t version;

    /**
     * @brief Vendor ID, as the structure holds it.
     */
    uint16_t raw_vendor_id;

    /**
     * @brief Device ID, as the structure holds it.
     */
    uint16_t raw_device_id;

    /**
     * @brief Programming interface of the class of the function.
     */
    uint8_t prog_if;

    /**
     * @brief Subclass of the function.
     */
    uint8_t subclass;

    /**
     * @brief Class of the function, e.g. `0x02` for the network controllers.
     */
    uint8_t pci_class;

    /**
     * @brief Revision ID of the function.
     */
    uint8_t revision;

    /**
     * @brief Model of the adapter, e.g. `Cisco UCS VIC 1455`.
     */
    const char *model;

    /**
     * @brief Slot the adapter is installed in, e.g. `SlotID:1`, `SlotID:MLOM`
     * or `SlotID:MRAID`.
     */
    const char *slot;

    /**
     * @brief Subsystem vendor ID, as the structure holds it.
     */
    uint16_t raw_subsystem_vendor_id;

    /**
     * @brief Subsystem ID, as the structure holds it.
     */
    uint16_t raw_subsystem_id;

    /**
     * @brief Version of the firmware of the adapter, `N/A` if it has none.
     */
    const char *firmware_version;

    /**
     * @brief PCI vendor ID.
     */
    dmi_pci_vendor_id_t vendor_id;

    /**
     * @brief PCI device ID.
     */
    uint16_t device_id;

    /**
     * @brief PCI subsystem vendor ID.
     */
    dmi_pci_vendor_id_t subsystem_vendor_id;

    /**
     * @brief PCI subsystem ID.
     */
    uint16_t subsystem_id;
};

/**
 * @brief Cisco PCI adapter information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_cisco_pci_adapter_spec;

#endif // !OPENDMI_ENTITY_CISCO_PCI_ADAPTER_H
