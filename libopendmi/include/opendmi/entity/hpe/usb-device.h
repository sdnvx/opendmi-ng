//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_USB_DEVICE_H
#define OPENDMI_ENTITY_HPE_USB_DEVICE_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_hpe_usb_device dmi_hpe_usb_device_t;

/**
 * @brief HP/HPE USB device correlation record (type 239), from Gen9 onwards.
 *
 * Describes a USB device the firmware has found at boot on a USB port
 * (type 238), and the UEFI boot entries naming it. Devices plugged in later
 * are not described. UEFI firmware only.
 */
struct dmi_hpe_usb_device
{
    /**
     * @brief Handle of the USB port connector correlation record (type 238).
     */
    dmi_handle_t port_handle;

    /**
     * @brief USB vendor ID of the device.
     */
    uint16_t vendor_id;

    /**
     * @brief Whether an embedded SD card is present.
     */
    bool is_sd_card_present;

    /**
     * @brief USB class code of the device.
     */
    uint8_t usb_class;

    /**
     * @brief USB subclass code of the device.
     */
    uint8_t usb_subclass;

    /**
     * @brief USB protocol code of the device.
     */
    uint8_t usb_protocol;

    /**
     * @brief USB product ID of the device.
     */
    uint16_t product_id;

    /**
     * @brief Capacity of the device in megabytes, zero if not applicable.
     */
    uint32_t raw_capacity;

    /**
     * @brief Capacity of the device in bytes, zero if not applicable.
     */
    uint64_t capacity;

    /**
     * @brief UEFI device path.
     */
    const char *uefi_device_path;

    /**
     * @brief UEFI device structured name.
     */
    const char *uefi_device_name;

    /**
     * @brief Device name.
     */
    const char *device_name;

    /**
     * @brief Location of the device.
     */
    const char *location;
};

/**
 * @brief HP/HPE USB device correlation record entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_usb_device_spec;

#endif // !OPENDMI_ENTITY_HPE_USB_DEVICE_H
