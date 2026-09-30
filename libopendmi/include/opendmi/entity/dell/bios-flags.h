//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_DELL_BIOS_FLAGS_H
#define OPENDMI_ENTITY_DELL_BIOS_FLAGS_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_dell_bios_flags dmi_dell_bios_flags_t;

/**
 * @brief Dell BIOS flags structure (type 177).
 *
 * Tells the features of the firmware the Dell drivers rely on. The flags
 * take 8 bytes, of which the Dell SMBIOS WMI driver of Linux reads the first
 * word, and the structure is 12 bytes long.
 */
struct dmi_dell_bios_flags
{
    /**
     * @brief Flags, of which only bit 1 is known.
     */
    uint16_t flags;

    /**
     * @brief Whether the firmware supports ACPI WMI, which the Dell WMI
     * drivers use, as bit 1 of the flags tells.
     */
    bool is_acpi_wmi;
};

/**
 * @brief Dell BIOS flags entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_dell_bios_flags_spec;

#endif // !OPENDMI_ENTITY_DELL_BIOS_FLAGS_H
