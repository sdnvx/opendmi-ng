//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_ACER_DEVICES_H
#define OPENDMI_ENTITY_ACER_DEVICES_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_acer_devices dmi_acer_devices_t;
typedef struct dmi_acer_device  dmi_acer_device_t;

/**
 * @brief Device of an Acer laptop.
 */
struct dmi_acer_device
{
    /**
     * @brief Kind of the device, e.g. `1` for the integrated graphics, `2`
     * for the discrete graphics, `4` for the webcam, `5` for the audio and
     * `7` for the wireless network adapter, as the data corpus suggests.
     */
    uint8_t kind;

    /**
     * @brief PCI or USB vendor ID of the device, zero if the device is absent.
     */
    uint16_t vendor_id;

    /**
     * @brief PCI or USB device ID of the device, zero if the device is absent.
     */
    uint16_t device_id;
};

/**
 * @brief Acer device list structure (type 171).
 *
 * Lists the devices of a laptop by their vendor and device IDs, e.g. the
 * graphics, the webcam, the audio and the wireless network adapter. Reverse
 * engineered from the data corpus.
 */
struct dmi_acer_devices
{
    /**
     * @brief Number of the devices.
     */
    size_t device_count;

    /**
     * @brief Devices, which run to the end of the structure. May be
     * @c nullptr when `device_count` is 0.
     */
    dmi_acer_device_t *devices;
};

/**
 * @brief Acer device list entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_acer_devices_spec;

#endif // !OPENDMI_ENTITY_ACER_DEVICES_H
