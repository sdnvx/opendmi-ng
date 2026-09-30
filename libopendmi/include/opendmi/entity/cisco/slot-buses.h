//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_CISCO_SLOT_BUSES_H
#define OPENDMI_ENTITY_CISCO_SLOT_BUSES_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_cisco_slot_buses dmi_cisco_slot_buses_t;

/**
 * @brief Cisco PCI slot buses (type 201).
 *
 * Lists the PCI buses behind a slot of a Cisco UCS server, presumably: the
 * numbers are the ones of the buses of the adapters the PCI adapter
 * information (type 202) gives for the same slot, e.g. six of them for an
 * adapter with a PCI switch. Reverse engineered from the data corpus.
 */
struct dmi_cisco_slot_buses
{
    /**
     * @brief Slot, as the PCI adapter information names it, e.g.
     * `SlotID:1` or `SlotID:MLOM`.
     */
    const char *slot;

    /**
     * @brief Number of the elements of `buses`.
     */
    size_t bus_count;

    /**
     * @brief PCI bus numbers. May be @c nullptr when `bus_count` is 0.
     */
    uint8_t *buses;
};

/**
 * @brief Cisco PCI slot buses entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_cisco_slot_buses_spec;

#endif // !OPENDMI_ENTITY_CISCO_SLOT_BUSES_H
