//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_MODULE_DELL_H
#define OPENDMI_MODULE_DELL_H

#pragma once

#include <opendmi/module.h>

/**
 * @brief Dell structure type identifiers.
 */
typedef enum dmi_dell_type
{
    DMI_TYPE_DELL_BIOS_FLAGS        = 177, ///< Dell BIOS flags
    DMI_TYPE_DELL_REVISIONS         = 208, ///< Dell revisions and IDs
    DMI_TYPE_DELL_PARALLEL_PORT     = 209, ///< Dell parallel port
    DMI_TYPE_DELL_SERIAL_PORT       = 210, ///< Dell serial port
    DMI_TYPE_DELL_INFRARED_PORT     = 211, ///< Dell infrared port
    DMI_TYPE_DELL_INDEXED_IO        = 212, ///< Dell indexed IO
    DMI_TYPE_DELL_PROTECTED_AREA_1  = 214, ///< Dell protected area type 1
    DMI_TYPE_DELL_PROTECTED_AREA_2  = 215, ///< Dell protected area type 2
    DMI_TYPE_DELL_CALLING_IFACE     = 218  ///< Dell calling interface
} dmi_dell_type_t;

__BEGIN_DECLS

extern __dmi_api const dmi_module_t dmi_dell_module;

__END_DECLS

#endif // !OPENDMI_MODULE_DELL_H
