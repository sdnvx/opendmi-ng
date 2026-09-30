//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_DELL_VIDEO_ROM_H
#define OPENDMI_ENTITY_DELL_VIDEO_ROM_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_dell_video_rom dmi_dell_video_rom_t;

/**
 * @brief Dell video BIOS information structure (type 216).
 *
 * Tells the vendor and the version of the video BIOS, e.g. `Intel Corp.`
 * and `2089`, or `ATI` and `RADEON 7000 V6.11`. Reverse engineered from
 * the data corpus.
 */
struct dmi_dell_video_rom
{
    /**
     * @brief Vendor of the video BIOS, as the firmware writes it, which may
     * be quoted.
     */
    const char *vendor;

    /**
     * @brief Version of the video BIOS, as the firmware writes it, which may
     * be quoted.
     */
    const char *version;

    /**
     * @brief Value whose meaning is not established, `1` in all known data.
     */
    uint8_t unknown_1;

    /**
     * @brief Value whose meaning is not established.
     */
    uint16_t unknown_2;
};

/**
 * @brief Dell video BIOS information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_dell_video_rom_spec;

#endif // !OPENDMI_ENTITY_DELL_VIDEO_ROM_H
