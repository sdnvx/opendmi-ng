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
 * @brief Kinds of the devices of Acer laptops.
 *
 * The kinds are reverse engineered from the PCI and USB IDs of the devices of
 * the data corpus. The ones named are given to the devices of a single class
 * on all the laptops, while the other kinds found in it are given to devices
 * of different classes, e.g. `1` to the graphics on one and to the card reader
 * on another, to a single laptop, or to absent devices only, and are named by
 * their values. Other values may be found in the data, and are kept as they
 * are.
 */
typedef enum dmi_acer_device_kind
{
    DMI_ACER_DEVICE_KIND_UNKNOWN_1  = 0x01, ///< Unknown kind 1
    DMI_ACER_DEVICE_KIND_UNKNOWN_2  = 0x02, ///< Unknown kind 2
    DMI_ACER_DEVICE_KIND_UNKNOWN_3  = 0x03, ///< Unknown kind 3
    DMI_ACER_DEVICE_KIND_CAMERA     = 0x04, ///< Webcam
    DMI_ACER_DEVICE_KIND_AUDIO      = 0x05, ///< Audio
    DMI_ACER_DEVICE_KIND_WLAN       = 0x07, ///< Wireless network adapter
    DMI_ACER_DEVICE_KIND_BLUETOOTH  = 0x08, ///< Bluetooth adapter
    DMI_ACER_DEVICE_KIND_UNKNOWN_13 = 0x0D, ///< Unknown kind 13
    DMI_ACER_DEVICE_KIND_UNKNOWN_17 = 0x11, ///< Unknown kind 17
    DMI_ACER_DEVICE_KIND_UNKNOWN_19 = 0x13, ///< Unknown kind 19
    DMI_ACER_DEVICE_KIND_UNKNOWN_21 = 0x15, ///< Unknown kind 21
    DMI_ACER_DEVICE_KIND_UNKNOWN_22 = 0x16, ///< Unknown kind 22
    DMI_ACER_DEVICE_KIND_UNKNOWN_25 = 0x19  ///< Unknown kind 25
} dmi_acer_device_kind_t;

/**
 * @brief Device of an Acer laptop.
 */
struct dmi_acer_device
{
    /**
     * @brief Kind of the device, whose meaning is not established, see
     * `dmi_acer_device_kind_t`.
     */
    dmi_acer_device_kind_t kind;

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

__BEGIN_DECLS

__dmi_api const char *dmi_acer_device_kind_name(dmi_acer_device_kind_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_ACER_DEVICES_H
