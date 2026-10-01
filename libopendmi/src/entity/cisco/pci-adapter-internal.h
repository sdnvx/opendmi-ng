//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_CISCO_PCI_ADAPTER_INTERNAL_H
#define OPENDMI_ENTITY_CISCO_PCI_ADAPTER_INTERNAL_H

#pragma once

#include <opendmi/field.h>

#include <opendmi/entity/cisco/pci-adapter.h>

/**
 * @internal
 * @brief Derive the vendor, device and subsystem identifiers of the adapter.
 *
 * @details Identifiers are words the structure holds in the big-endian byte
 * order, while they are read in the little-endian one, so their bytes are
 * swapped.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_cisco_pci_adapter_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_CISCO_PCI_ADAPTER_INTERNAL_H
