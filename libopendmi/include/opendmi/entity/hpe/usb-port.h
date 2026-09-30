//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_USB_PORT_H
#define OPENDMI_ENTITY_HPE_USB_PORT_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_hpe_usb_port dmi_hpe_usb_port_t;

/**
 * @brief Location of a USB port.
 */
typedef enum dmi_hpe_usb_location
{
    DMI_HPE_USB_LOCATION_INTERNAL    = 0x00, ///< Internal
    DMI_HPE_USB_LOCATION_FRONT       = 0x01, ///< Front of the server
    DMI_HPE_USB_LOCATION_REAR        = 0x02, ///< Rear of the server
    DMI_HPE_USB_LOCATION_SD_CARD     = 0x03, ///< Embedded internal SD card
    DMI_HPE_USB_LOCATION_ILO         = 0x04, ///< iLO USB
    DMI_HPE_USB_LOCATION_NAND_HUB    = 0x05, ///< USB hub of the NAND controller
    DMI_HPE_USB_LOCATION_DEBUG       = 0x07, ///< Debug port
    DMI_HPE_USB_LOCATION_OCP         = 0x09  ///< OCP USB
} dmi_hpe_usb_location_t;

/**
 * @brief Sharing of a USB port with the management controller.
 */
typedef enum dmi_hpe_usb_sharing
{
    DMI_HPE_USB_SHARING_NONE      = 0x00, ///< Not shared
    DMI_HPE_USB_SHARING_SWITCH    = 0x01, ///< Shared with a physical switch
    DMI_HPE_USB_SHARING_AUTOMATIC = 0x02  ///< Shared with automatic control
} dmi_hpe_usb_sharing_t;

/**
 * @brief Speed a USB port is configured for.
 */
typedef enum dmi_hpe_usb_speed
{
    DMI_HPE_USB_SPEED_FULL  = 0x01, ///< USB 1.1 full speed
    DMI_HPE_USB_SPEED_HIGH  = 0x02, ///< USB 2.0 high speed
    DMI_HPE_USB_SPEED_SUPER = 0x03  ///< USB 3.0 super speed
} dmi_hpe_usb_speed_t;

/**
 * @brief HP/HPE USB port connector correlation record (type 238), from Gen9
 * onwards.
 *
 * Correlates a USB port connector (type 8) with its USB controller, its
 * location and the UEFI device path of its endpoint.
 */
struct dmi_hpe_usb_port
{
    /**
     * @brief Handle of the port connector structure (type 8).
     */
    dmi_handle_t port_handle;

    /**
     * @brief PCI bus of the USB controller of the port.
     */
    uint8_t bus;

    /**
     * @brief PCI device and function of the USB controller of the port,
     * `(device << 3) | function`.
     */
    uint8_t devfn;

    /**
     * @brief Location of the port.
     */
    dmi_hpe_usb_location_t location;

    /**
     * @brief Sharing of the port with the management controller.
     */
    dmi_hpe_usb_sharing_t sharing;

    /**
     * @brief Instance of the port among the ports of its type.
     */
    uint8_t instance;

    /**
     * @brief Instance of the internal hub the port is behind. Set to `0xFE`
     * when not applicable.
     */
    uint8_t hub_instance;

    /**
     * @brief Speed the firmware configures the port for.
     */
    dmi_hpe_usb_speed_t speed;

    /**
     * @brief UEFI device path of the USB endpoint.
     */
    const char *uefi_device_path;

    /**
     * @brief PCI segment group of the USB controller.
     */
    uint16_t segment;
};

/**
 * @brief HP/HPE USB port connector correlation record entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_usb_port_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_hpe_usb_location_name(dmi_hpe_usb_location_t value);
__dmi_api const char *dmi_hpe_usb_sharing_name(dmi_hpe_usb_sharing_t value);
__dmi_api const char *dmi_hpe_usb_speed_name(dmi_hpe_usb_speed_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_HPE_USB_PORT_H
