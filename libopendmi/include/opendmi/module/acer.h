//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_MODULE_ACER_H
#define OPENDMI_MODULE_ACER_H

#pragma once

#include <opendmi/module.h>

/**
 * @brief Acer structure type identifiers.
 */
typedef enum dmi_acer_type_id
{
    DMI_TYPE_ID_ACER_HOTKEYS = 170, ///< Hotkey functions
    DMI_TYPE_ID_ACER_DEVICES = 171  ///< Device list
} dmi_acer_type_id_t;

__BEGIN_DECLS

/** @brief Hotkey functions */
extern __dmi_api const dmi_type_t dmi_type_acer_hotkeys;

/** @brief Device list */
extern __dmi_api const dmi_type_t dmi_type_acer_devices;

extern __dmi_api const dmi_module_t dmi_acer_module;

__END_DECLS

#endif // !OPENDMI_MODULE_ACER_H
