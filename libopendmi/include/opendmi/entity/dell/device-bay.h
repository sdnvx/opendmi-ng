//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_DELL_DEVICE_BAY_H
#define OPENDMI_ENTITY_DELL_DEVICE_BAY_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_dell_device_bay dmi_dell_device_bay_t;

/**
 * @brief Dell device bay structure (type 219).
 *
 * Describes a bay of a laptop or of its docking station, which takes one of
 * several devices, e.g. an optical drive or a second battery. Reverse
 * engineered from the data corpus.
 */
struct dmi_dell_device_bay
{
    /**
     * @brief Value whose meaning is not established, `3` in all known data.
     */
    uint8_t unknown_1;

    /**
     * @brief Name of the bay, e.g. `System Device Bay` or `Dock DBay`.
     */
    const char *name;

    /**
     * @brief Devices the bay takes, separated by commas, e.g. `Floppy,
     * Battery, CD-ROM, CD-RW, Hard Disk, DVD`.
     */
    const char *supported_devices;

    /**
     * @brief Device the bay holds, e.g. `Battery`, or `EMPTY` for none.
     */
    const char *installed_device;

    /**
     * @brief Value whose meaning is not established, `0xFF` in all known
     * data.
     */
    uint8_t unknown_2;
};

/**
 * @brief Dell device bay entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_dell_device_bay_spec;

#endif // !OPENDMI_ENTITY_DELL_DEVICE_BAY_H
