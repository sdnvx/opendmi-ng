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

#ifndef DMI_HPE_USB_DEVICE_T
#   define DMI_HPE_USB_DEVICE_T
    typedef struct dmi_hpe_usb_device dmi_hpe_usb_device_t;
#endif // !DMI_HPE_USB_DEVICE_T

/**
 * @brief Subclasses of USB mass storage devices, the command sets they use.
 */
typedef enum dmi_hpe_usb_storage_subclass
{
    DMI_HPE_USB_STORAGE_SUBCLASS_UNREPORTED = 0x00, ///< SCSI command set not reported
    DMI_HPE_USB_STORAGE_SUBCLASS_RBC        = 0x01, ///< RBC
    DMI_HPE_USB_STORAGE_SUBCLASS_ATAPI      = 0x02, ///< ATAPI
    DMI_HPE_USB_STORAGE_SUBCLASS_QIC_157    = 0x03, ///< QIC-157, obsolete
    DMI_HPE_USB_STORAGE_SUBCLASS_UFI        = 0x04, ///< UFI
    DMI_HPE_USB_STORAGE_SUBCLASS_SFF_8070I  = 0x05, ///< SFF-8070i, obsolete
    DMI_HPE_USB_STORAGE_SUBCLASS_SCSI       = 0x06, ///< SCSI transparent command set
    DMI_HPE_USB_STORAGE_SUBCLASS_LSD_FS     = 0x07, ///< LSD FS
    DMI_HPE_USB_STORAGE_SUBCLASS_IEEE_1667  = 0x08, ///< IEEE 1667
    // Reserved: 0x09 .. 0xFE
    DMI_HPE_USB_STORAGE_SUBCLASS_VENDOR     = 0xFF  ///< Vendor-specific
} dmi_hpe_usb_storage_subclass_t;

/**
 * @brief Transport protocols of USB mass storage devices.
 */
typedef enum dmi_hpe_usb_storage_proto
{
    DMI_HPE_USB_STORAGE_PROTO_CBI_INT  = 0x00, ///< CBI with command completion interrupt
    DMI_HPE_USB_STORAGE_PROTO_CBI      = 0x01, ///< CBI without command completion interrupt
    DMI_HPE_USB_STORAGE_PROTO_OBSOLETE = 0x02, ///< Obsolete
    DMI_HPE_USB_STORAGE_PROTO_BOT      = 0x50, ///< Bulk-only transport
    DMI_HPE_USB_STORAGE_PROTO_UAS      = 0x62, ///< USB attached SCSI
    DMI_HPE_USB_STORAGE_PROTO_VENDOR   = 0xFF  ///< Vendor-specific
} dmi_hpe_usb_storage_proto_t;

/**
 * @brief Protocols of USB hubs, the speeds and the transaction translators
 * they have.
 */
typedef enum dmi_hpe_usb_hub_proto
{
    DMI_HPE_USB_HUB_PROTO_FULL_SPEED = 0x00, ///< Full speed
    DMI_HPE_USB_HUB_PROTO_SINGLE_TT  = 0x01, ///< Hi-speed with a single transaction translator
    DMI_HPE_USB_HUB_PROTO_MULTI_TT   = 0x02  ///< Hi-speed with multiple transaction translators
} dmi_hpe_usb_hub_proto_t;

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
     * @brief USB subclass code of the device, see
     * `dmi_hpe_usb_storage_subclass_t` for mass storage devices.
     */
    uint8_t usb_subclass;

    /**
     * @brief USB protocol code of the device, see
     * `dmi_hpe_usb_storage_proto_t` for mass storage devices and
     * `dmi_hpe_usb_hub_proto_t` for hubs.
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

__BEGIN_DECLS

__dmi_api const char *dmi_hpe_usb_storage_subclass_name(dmi_hpe_usb_storage_subclass_t value);
__dmi_api const char *dmi_hpe_usb_storage_proto_name(dmi_hpe_usb_storage_proto_t value);
__dmi_api const char *dmi_hpe_usb_hub_proto_name(dmi_hpe_usb_hub_proto_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_HPE_USB_DEVICE_H
