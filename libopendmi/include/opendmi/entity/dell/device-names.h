//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_DELL_DEVICE_NAMES_H
#define OPENDMI_ENTITY_DELL_DEVICE_NAMES_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_DELL_DEVICE_NAMES_T
#   define DMI_DELL_DEVICE_NAMES_T
    typedef struct dmi_dell_device_names dmi_dell_device_names_t;
#endif // !DMI_DELL_DEVICE_NAMES_T

#ifndef DMI_DELL_DEVICE_NAME_ENTRY_T
#   define DMI_DELL_DEVICE_NAME_ENTRY_T
    typedef struct dmi_dell_device_name_entry dmi_dell_device_name_entry_t;
#endif // !DMI_DELL_DEVICE_NAME_ENTRY_T

/**
 * @brief Name of a device, given to the structure describing it.
 */
struct dmi_dell_device_name_entry
{
    /**
     * @brief Fully qualified device descriptor (FQDD) of the device, by which
     * the management controller (iDRAC) names it, e.g. `CPU.Socket.1` or
     * `DIMM.Socket.A1`.
     */
    const char *fqdd;

    /**
     * @brief Handle of the structure describing the device, e.g. of
     * a processor (type 4) or of a memory device (type 17).
     */
    dmi_handle_t handle;

    /**
     * @brief Value whose meaning is not established, `0` in all known data.
     */
    uint8_t unknown;
};

/**
 * @brief Dell device names structure (type 225).
 *
 * Gives the devices of a kind, e.g. the processors or the memory devices,
 * the names the management controller (iDRAC) of a PowerEdge server knows
 * them by. Reverse engineered from the data corpus.
 */
struct dmi_dell_device_names
{
    /**
     * @brief Value whose meaning is not established, `1` in all known data.
     */
    uint8_t unknown;

    /**
     * @brief Number of devices.
     */
    size_t device_count;

    /**
     * @brief Names of the devices.
     */
    dmi_dell_device_name_entry_t *devices;
};

/**
 * @brief Dell device names entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_dell_device_names_spec;

#endif // !OPENDMI_ENTITY_DELL_DEVICE_NAMES_H
