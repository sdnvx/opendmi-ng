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

#ifndef DMI_DELL_DEVICE_BAY_T
#   define DMI_DELL_DEVICE_BAY_T
    typedef struct dmi_dell_device_bay dmi_dell_device_bay_t;
#endif // !DMI_DELL_DEVICE_BAY_T

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
     * @brief Value whose meaning is not established, `3` on the systems which
     * have a bay, and `0` or `2` on the ones which have none.
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
     * @brief Value whose meaning is not established, `0xFF` in most known
     * data, and `0` on Precision M3800.
     */
    uint8_t unknown_2;

    /**
     * @brief String whose meaning is not established, which the structures
     * of 11 bytes refer to, e.g. a blank one on Precision M3800 or `00h` on
     * G15 5515.
     */
    const char *unknown_string_1;

    /**
     * @brief Second string whose meaning is not established.
     */
    const char *unknown_string_2;

    /**
     * @brief Whether the structure refers to `unknown_string_1` and
     * `unknown_string_2`, which the structures of 9 bytes leave out.
     */
    bool has_unknown_strings;
};

/**
 * @brief Dell device bay entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_dell_device_bay_spec;

#endif // !OPENDMI_ENTITY_DELL_DEVICE_BAY_H
