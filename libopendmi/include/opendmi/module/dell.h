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
typedef enum dmi_dell_type_id
{
    DMI_TYPE_ID_DELL_BIOS_FLAGS       = 177, ///< Dell BIOS flags
    DMI_TYPE_ID_DELL_HOTKEYS          = 178, ///< Dell hotkeys
    DMI_TYPE_ID_DELL_REVISIONS        = 208, ///< Dell revisions and IDs
    DMI_TYPE_ID_DELL_PARALLEL_PORT    = 209, ///< Dell parallel port
    DMI_TYPE_ID_DELL_SERIAL_PORT      = 210, ///< Dell serial port
    DMI_TYPE_ID_DELL_INFRARED_PORT    = 211, ///< Dell infrared port
    DMI_TYPE_ID_DELL_INDEXED_IO       = 212, ///< Dell indexed IO
    DMI_TYPE_ID_DELL_PROTECTED_AREA_1 = 213, ///< Dell protected area type 1
    DMI_TYPE_ID_DELL_PROTECTED_AREA_2 = 214, ///< Dell protected area type 2
    DMI_TYPE_ID_DELL_VIDEO_ROM        = 216, ///< Dell video BIOS information
    DMI_TYPE_ID_DELL_CALLING_IFACE    = 218, ///< Dell calling interface
    DMI_TYPE_ID_DELL_TOKEN_REFS_1     = 220, ///< Dell token references type 1
    DMI_TYPE_ID_DELL_TOKEN_REFS_2     = 221, ///< Dell token references type 2
    DMI_TYPE_ID_DELL_SYSTEM_ID        = 255  ///< Dell system ID record
} dmi_dell_type_id_t;

__BEGIN_DECLS

/** @brief Dell BIOS flags */
extern __dmi_api const dmi_type_t dmi_type_dell_bios_flags;

/** @brief Dell hotkeys */
extern __dmi_api const dmi_type_t dmi_type_dell_hotkeys;

/** @brief Dell revisions and IDs */
extern __dmi_api const dmi_type_t dmi_type_dell_revisions;

/** @brief Dell parallel port */
extern __dmi_api const dmi_type_t dmi_type_dell_parallel_port;

/** @brief Dell serial port */
extern __dmi_api const dmi_type_t dmi_type_dell_serial_port;

/** @brief Dell infrared port */
extern __dmi_api const dmi_type_t dmi_type_dell_infrared_port;

/** @brief Dell indexed IO */
extern __dmi_api const dmi_type_t dmi_type_dell_indexed_io;

/** @brief Dell protected area type 1 */
extern __dmi_api const dmi_type_t dmi_type_dell_protected_area_1;

/** @brief Dell protected area type 2 */
extern __dmi_api const dmi_type_t dmi_type_dell_protected_area_2;

/** @brief Dell video BIOS information */
extern __dmi_api const dmi_type_t dmi_type_dell_video_rom;

/** @brief Dell calling interface */
extern __dmi_api const dmi_type_t dmi_type_dell_calling_iface;

/** @brief Dell token references type 1 */
extern __dmi_api const dmi_type_t dmi_type_dell_token_refs_1;

/** @brief Dell token references type 2 */
extern __dmi_api const dmi_type_t dmi_type_dell_token_refs_2;

/** @brief Dell system ID record */
extern __dmi_api const dmi_type_t dmi_type_dell_system_id;

extern __dmi_api const dmi_module_t dmi_dell_module;

__END_DECLS

#endif // !OPENDMI_MODULE_DELL_H
