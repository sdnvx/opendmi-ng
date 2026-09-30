//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_DELL_HOTKEYS_H
#define OPENDMI_ENTITY_DELL_HOTKEYS_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_dell_hotkeys dmi_dell_hotkeys_t;
typedef struct dmi_dell_hotkey  dmi_dell_hotkey_t;

/**
 * @brief Mapping of a hotkey.
 */
struct dmi_dell_hotkey
{
    /**
     * @brief Scan code of the key, as the firmware reports it in the WMI
     * events of the hotkeys.
     */
    uint16_t scancode;

    /**
     * @brief Code of the function of the key, as the Dell WMI driver of
     * Linux numbers the functions.
     */
    uint16_t keycode;
};

/**
 * @brief Dell hotkeys structure (type 178).
 *
 * Maps the scan codes of the hotkeys of a laptop to their functions, which
 * the Dell WMI driver reads.
 */
struct dmi_dell_hotkeys
{
    /**
     * @brief Number of the hotkeys.
     */
    size_t hotkey_count;

    /**
     * @brief Hotkeys.
     */
    dmi_dell_hotkey_t *hotkeys;
};

/**
 * @brief Dell hotkeys entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_dell_hotkeys_spec;

#endif // !OPENDMI_ENTITY_DELL_HOTKEYS_H
