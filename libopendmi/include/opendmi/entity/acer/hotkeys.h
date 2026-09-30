//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_ACER_HOTKEYS_H
#define OPENDMI_ENTITY_ACER_HOTKEYS_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_acer_hotkeys dmi_acer_hotkeys_t;
typedef struct dmi_acer_hotkey  dmi_acer_hotkey_t;

/**
 * @brief Communication function of the hotkeys, as a bit of the
 * communication function bitmap.
 */
typedef enum dmi_acer_comm_function
{
    DMI_ACER_COMM_FUNCTION_WIFI      = 0,  ///< WiFi
    DMI_ACER_COMM_FUNCTION_3G        = 6,  ///< 3G
    DMI_ACER_COMM_FUNCTION_WIMAX     = 7,  ///< WiMAX
    DMI_ACER_COMM_FUNCTION_BLUETOOTH = 11  ///< Bluetooth
} dmi_acer_comm_function_t;

/**
 * @brief Hotkey, as the structure lists it past the function bitmaps.
 */
struct dmi_acer_hotkey
{
    /**
     * @brief Number of the key, whose high bits tell the button group:
     * `0x0_` communication, `0x4_` media, `0x6_` display and `0x8_` others.
     */
    uint8_t key;

    /**
     * @brief Value whose meaning is not established, `2` in all known data.
     */
    uint8_t kind;

    /**
     * @brief Function of the key, as a bit of the function bitmap of its
     * button group.
     */
    uint16_t function;
};

/**
 * @brief Acer hotkey functions structure (type 170).
 *
 * Tells the functions of the hotkeys of a laptop, which the Acer WMI driver
 * reads. Some laptops of other brands, e.g. Fujitsu-Siemens, Medion, Lenovo
 * and eMachines, are said to carry it too.
 */
struct dmi_acer_hotkeys
{
    /**
     * @brief Functions of the communication button, whose bits are the
     * values of `dmi_acer_comm_function_t`.
     */
    uint16_t comm_functions;

    /**
     * @brief Functions of the application button.
     */
    uint16_t app_functions;

    /**
     * @brief Functions of the media button.
     */
    uint16_t media_functions;

    /**
     * @brief Functions of the display button.
     */
    uint16_t display_functions;

    /**
     * @brief Functions of the other buttons.
     */
    uint16_t other_functions;

    /**
     * @brief Number of the key of the communication function, which is the
     * key of the first hotkey. Set to `UINT8_MAX` when the structure holds
     * none.
     */
    uint8_t comm_key;

    /**
     * @brief Number of the hotkeys.
     */
    size_t hotkey_count;

    /**
     * @brief Hotkeys, which run to the end of the structure. May be
     * @c nullptr when `hotkey_count` is 0.
     */
    dmi_acer_hotkey_t *hotkeys;
};

/**
 * @brief Acer hotkey functions entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_acer_hotkeys_spec;

#endif // !OPENDMI_ENTITY_ACER_HOTKEYS_H
