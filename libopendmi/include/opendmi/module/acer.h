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
typedef enum dmi_acer_type
{
    DMI_TYPE_ACER_HOTKEYS = 170 ///< Hotkey functions
} dmi_acer_type_t;

__BEGIN_DECLS

extern __dmi_api const dmi_module_t dmi_acer_module;

__END_DECLS

#endif // !OPENDMI_MODULE_ACER_H
