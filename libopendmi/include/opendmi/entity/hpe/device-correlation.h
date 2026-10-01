//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_DEVICE_CORRELATION_H
#define OPENDMI_ENTITY_HPE_DEVICE_CORRELATION_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_HPE_DEVICE_CORRELATION_T
#   define DMI_HPE_DEVICE_CORRELATION_T
    typedef struct dmi_hpe_device_correlation dmi_hpe_device_correlation_t;
#endif // !DMI_HPE_DEVICE_CORRELATION_T

/**
 * @brief Type of a device, as UEFI boot entries name it.
 */
typedef enum dmi_hpe_device_type
{
    DMI_HPE_DEVICE_TYPE_UNKNOWN          = 0x00, ///< Unknown
    DMI_HPE_DEVICE_TYPE_FLEXIBLE_LOM     = 0x03, ///< Flexible LOM
    DMI_HPE_DEVICE_TYPE_EMBEDDED_LOM     = 0x04, ///< Embedded LOM
    DMI_HPE_DEVICE_TYPE_SLOT_NIC         = 0x05, ///< NIC in a slot
    DMI_HPE_DEVICE_TYPE_STORAGE          = 0x06, ///< Storage controller
    DMI_HPE_DEVICE_TYPE_SMART_ARRAY      = 0x07, ///< Smart Array storage controller
    DMI_HPE_DEVICE_TYPE_USB_DISK         = 0x08, ///< USB hard disk
    DMI_HPE_DEVICE_TYPE_OTHER_PCI        = 0x09, ///< Other PCI device
    DMI_HPE_DEVICE_TYPE_RAM_DISK         = 0x0A, ///< RAM disk
    DMI_HPE_DEVICE_TYPE_FIRMWARE_VOLUME  = 0x0B, ///< Firmware volume
    DMI_HPE_DEVICE_TYPE_UEFI_SHELL       = 0x0C, ///< UEFI shell
    DMI_HPE_DEVICE_TYPE_USB_BOOT         = 0x0D, ///< Generic UEFI USB boot entry
    DMI_HPE_DEVICE_TYPE_DYNAMIC_ARRAY    = 0x0E, ///< Dynamic Smart Array controller
    DMI_HPE_DEVICE_TYPE_FILE             = 0x0F, ///< File
    DMI_HPE_DEVICE_TYPE_NVME             = 0x10, ///< NVMe hard drive
    DMI_HPE_DEVICE_TYPE_NVDIMM           = 0x11, ///< NVDIMM
    DMI_HPE_DEVICE_TYPE_EMBEDDED_GPU     = 0x12  ///< Embedded GPU
} dmi_hpe_device_type_t;

/**
 * @brief Location of a device.
 */
typedef enum dmi_hpe_device_location
{
    DMI_HPE_DEVICE_LOCATION_UNKNOWN          = 0x00, ///< Unknown
    DMI_HPE_DEVICE_LOCATION_EMBEDDED         = 0x01, ///< Embedded
    DMI_HPE_DEVICE_LOCATION_ILO_MEDIA        = 0x02, ///< iLO virtual media
    DMI_HPE_DEVICE_LOCATION_FRONT_USB        = 0x03, ///< Front USB port
    DMI_HPE_DEVICE_LOCATION_REAR_USB         = 0x04, ///< Rear USB port
    DMI_HPE_DEVICE_LOCATION_INTERNAL_USB     = 0x05, ///< Internal USB
    DMI_HPE_DEVICE_LOCATION_INTERNAL_SD      = 0x06, ///< Internal SD card
    DMI_HPE_DEVICE_LOCATION_VIRTUAL_USB      = 0x07, ///< Internal virtual USB (embedded NAND)
    DMI_HPE_DEVICE_LOCATION_EMBEDDED_SATA    = 0x08, ///< Embedded SATA port
    DMI_HPE_DEVICE_LOCATION_EMBEDDED_ARRAY   = 0x09, ///< Embedded Smart Array
    DMI_HPE_DEVICE_LOCATION_PCI_SLOT         = 0x0A, ///< PCI slot
    DMI_HPE_DEVICE_LOCATION_RAM              = 0x0B, ///< RAM memory
    DMI_HPE_DEVICE_LOCATION_USB              = 0x0C, ///< USB
    DMI_HPE_DEVICE_LOCATION_DYNAMIC_ARRAY    = 0x0D, ///< Dynamic Smart Array controller
    DMI_HPE_DEVICE_LOCATION_URL              = 0x0E, ///< URL
    DMI_HPE_DEVICE_LOCATION_NVME_BAY         = 0x0F, ///< NVMe drive bay
    DMI_HPE_DEVICE_LOCATION_NVDIMM_PROCESSOR = 0x10, ///< NVDIMM processor
    DMI_HPE_DEVICE_LOCATION_NVDIMM_BOARD     = 0x11, ///< NVDIMM board
    DMI_HPE_DEVICE_LOCATION_NVME_RISER       = 0x12, ///< NVMe riser
    DMI_HPE_DEVICE_LOCATION_NVDIMM_NAMESPACE = 0x13, ///< NVDIMM namespace
    DMI_HPE_DEVICE_LOCATION_VROC_SATA        = 0x14, ///< VROC SATA
    DMI_HPE_DEVICE_LOCATION_VROC_NVME        = 0x15  ///< VROC NVMe
} dmi_hpe_device_location_t;

/**
 * @brief HP/HPE device correlation record (type 203), from Gen9 onwards.
 *
 * Correlates a device with the system slot (type 9) or the onboard device
 * (type 41) it is found at, and with the UEFI boot entries naming it.
 */
struct dmi_hpe_device_correlation
{
    /**
     * @brief Handle of the system slot (type 9) or the onboard device
     * (type 41) structure. Set to `0xFFFE` when not applicable.
     */
    dmi_handle_t device_handle;

    /**
     * @brief Handle of the SMBus segment record (type 228). Set to `0xFFFE`
     * when not applicable.
     */
    dmi_handle_t smbus_handle;

    /**
     * @brief PCI vendor ID of the device. Set to `0xFFFF` when the device is
     * not present.
     */
    uint16_t pci_vendor_id;

    /**
     * @brief PCI device ID of the device. Set to `0xFFFF` when the device is
     * not present.
     */
    uint16_t pci_device_id;

    /**
     * @brief PCI subsystem vendor ID of the device. Set to `0xFFFF` when the
     * device is not present.
     */
    uint16_t pci_subvendor_id;

    /**
     * @brief PCI subsystem ID of the device. Set to `0xFFFF` when the device
     * is not present.
     */
    uint16_t pci_subdevice_id;

    /**
     * @brief PCI class code of the device. Set to `0xFF` when the device is
     * not present.
     */
    uint8_t pci_class;

    /**
     * @brief PCI subclass code of the device. Set to `0xFF` when the device
     * is not present.
     */
    uint8_t pci_subclass;

    /**
     * @brief Handle of the parent device. Set to `0xFFFE` when not
     * applicable. The structure it refers to is not documented, and is
     * presumably the device correlation record of the parent device.
     */
    dmi_handle_t parent_handle;

    /**
     * @brief Whether the device is a peer bifurcated one, whose physical
     * slot `physical_handle` tells.
     */
    bool is_peer_bifurcated;

    /**
     * @brief Whether the device is an upstream one.
     */
    bool is_upstream;

    /**
     * @brief Type of the device, UEFI only.
     */
    dmi_hpe_device_type_t device_type;

    /**
     * @brief Location of the device.
     */
    dmi_hpe_device_location_t device_location;

    /**
     * @brief Instance of the device.
     */
    uint8_t instance;

    /**
     * @brief Sub-instance of the device: the port of a NIC, or the bay of an
     * NVMe drive.
     */
    uint8_t sub_instance;

    /**
     * @brief Bay of the device, zero if unknown. Set to `0xFF` when not
     * shown.
     */
    uint8_t bay;

    /**
     * @brief Enclosure of the device, zero if unknown. Set to `0xFF` when
     * not shown.
     */
    uint8_t enclosure;

    /**
     * @brief UEFI device path.
     */
    const char *uefi_device_path;

    /**
     * @brief UEFI device structured name.
     */
    const char *uefi_device_name;

    /**
     * @brief UEFI device name.
     */
    const char *device_name;

    /**
     * @brief UEFI location.
     */
    const char *uefi_location;

    /**
     * @brief Handle of the system slot structure (type 9) of the real
     * physical slot, valid only if `is_peer_bifurcated` is set.
     */
    dmi_handle_t physical_handle;

    /**
     * @brief Part number of the PCI device.
     */
    const char *part_number;

    /**
     * @brief Serial number of the PCI device.
     */
    const char *serial_number;

    /**
     * @brief PCI segment group, zero for a single group topology.
     */
    uint16_t segment;

    /**
     * @brief PCI bus.
     */
    uint8_t bus;

    /**
     * @brief PCI device and function, `(device << 3) | function`.
     */
    uint8_t devfn;

    /**
     * @brief Whether the structure holds the PCI segment group, the bus, the
     * device and the function, which the structures shorter than 40 bytes
     * leave out. The fields are not shown then.
     */
    bool has_pci_location;
};

/**
 * @brief HP/HPE device correlation record entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_device_correlation_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_hpe_device_type_name(dmi_hpe_device_type_t value);
__dmi_api const char *dmi_hpe_device_location_name(dmi_hpe_device_location_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_HPE_DEVICE_CORRELATION_H
