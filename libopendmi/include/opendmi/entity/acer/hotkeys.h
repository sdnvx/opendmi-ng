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

#ifndef DMI_ACER_HOTKEYS_T
#   define DMI_ACER_HOTKEYS_T
    typedef struct dmi_acer_hotkeys dmi_acer_hotkeys_t;
#endif // !DMI_ACER_HOTKEYS_T

#ifndef DMI_ACER_HOTKEY_ENTRY_T
#   define DMI_ACER_HOTKEY_ENTRY_T
    typedef struct dmi_acer_hotkey_entry dmi_acer_hotkey_entry_t;
#endif // !DMI_ACER_HOTKEY_ENTRY_T

/**
 * @brief Communication function of the hotkeys, as a bit of the
 * communication function bitmap, named the way the Acer WMI driver of Linux
 * names them.
 */
typedef enum dmi_acer_comm_function
{
    DMI_ACER_COMM_FUNCTION_WIFI      = 0,  ///< WiFi
    DMI_ACER_COMM_FUNCTION_3G        = 6,  ///< 3G
    DMI_ACER_COMM_FUNCTION_WIMAX     = 7,  ///< WiMAX
    DMI_ACER_COMM_FUNCTION_BLUETOOTH = 11, ///< Bluetooth
    DMI_ACER_COMM_FUNCTION_RF_BUTTON = 14  ///< Radio button
} dmi_acer_comm_function_t;

/**
 * @brief Hotkey, as the structure lists it past the function bitmaps.
 */
struct dmi_acer_hotkey_entry
{
    /**
     * @brief Number of the key, whose high bits tell the button group:
     * `0x0_` communication, `0x4_` media, `0x6_` display and `0x8_` others.
     */
    uint8_t key;

    /**
     * @brief Value whose meaning is not established, `1` or `2` in the known
     * data, the same for all the hotkeys of a structure.
     */
    uint8_t kind;

    /**
     * @brief Function of the key, as bits of the function bitmap of its
     * button group: the key of the communication function has all the bits
     * of the communication bitmap, e.g. those of WiFi and Bluetooth.
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
     * @brief Number of the key of the communication function, at offset
     * `0x0E`, where the structures holding hotkeys begin the first of them.
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
    dmi_acer_hotkey_entry_t *hotkeys;
};

/**
 * @brief Acer hotkey functions entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_acer_hotkeys_spec;

/**
 * @brief Acer hotkey functions entity specification of the structures of
 * `0x0F` bytes, which end with the number of the key of the communication
 * function and hold no hotkeys.
 *
 * Such structures hold the functions and the number of the key of the
 * communication function only, the way the Acer WMI driver reads them.
 */
extern __dmi_api const dmi_entity_spec_t dmi_acer_hotkeys_basic_spec;

#endif // !OPENDMI_ENTITY_ACER_HOTKEYS_H
