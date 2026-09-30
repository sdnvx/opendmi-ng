//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_MODULE_CISCO_H
#define OPENDMI_MODULE_CISCO_H

#pragma once

#include <opendmi/module.h>

/**
 * @brief Cisco structure type identifiers.
 */
typedef enum dmi_cisco_type_id
{
    DMI_TYPE_ID_CISCO_SLOT_BUSES  = 201, ///< PCI slot buses
    DMI_TYPE_ID_CISCO_PCI_ADAPTER = 202  ///< PCI adapter information
} dmi_cisco_type_id_t;

__BEGIN_DECLS

/** @brief PCI slot buses */
extern __dmi_api const dmi_type_t dmi_type_cisco_slot_buses;

/** @brief PCI adapter information */
extern __dmi_api const dmi_type_t dmi_type_cisco_pci_adapter;

extern __dmi_api const dmi_module_t dmi_cisco_module;

__END_DECLS

#endif // !OPENDMI_MODULE_CISCO_H
