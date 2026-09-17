//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DMI_ENTITY_INTEL_RSD_STORAGE_DEVICE_H
#define DMI_ENTITY_INTEL_RSD_STORAGE_DEVICE_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/entity/common.h>

typedef struct dmi_intel_rsd_storage_device dmi_intel_rsd_storage_device_t;

/**
 * @brief Intel RSD storage connector types.
 */
typedef enum dmi_intel_rsd_storage_connector
{
    DMI_INTEL_RSD_STORAGE_CONNECTOR_UNKNOWN = 0x00, ///< Unknown
    DMI_INTEL_RSD_STORAGE_CONNECTOR_SATA    = 0x01, ///< SATA
    DMI_INTEL_RSD_STORAGE_CONNECTOR_SAS     = 0x02, ///< SAS
    DMI_INTEL_RSD_STORAGE_CONNECTOR_PCIE    = 0x03, ///< PCIe
    DMI_INTEL_RSD_STORAGE_CONNECTOR_M2      = 0x04, ///< M.2
    DMI_INTEL_RSD_STORAGE_CONNECTOR_USB     = 0x05, ///< USB
    DMI_INTEL_RSD_STORAGE_CONNECTOR_U2      = 0x06, ///< U.2
    __DMI_INTEL_RSD_STORAGE_CONNECTOR_COUNT
} dmi_intel_rsd_storage_connector_t;

/**
 * @brief Intel RSD storage protocol types.
 */
typedef enum dmi_intel_rsd_storage_proto
{
    DMI_INTEL_RSD_STORAGE_PROTO_UNKNOWN = 0x00, ///< Unknown
    DMI_INTEL_RSD_STORAGE_PROTO_IDE     = 0x01, ///< IDE
    DMI_INTEL_RSD_STORAGE_PROTO_AHCI    = 0x02, ///< AHCI
    DMI_INTEL_RSD_STORAGE_PROTO_NVME    = 0x03, ///< NVMe
    DMI_INTEL_RSD_STORAGE_PROTO_USB     = 0x04, ///< USB
    __DMI_INTEL_RSD_STORAGE_PROTO_COUNT
} dmi_intel_rsd_storage_proto_t;

/**
 * @brief Intel RSD storage device types.
 */
typedef enum dmi_intel_rsd_storage_device_type
{
    DMI_INTEL_RSD_STORAGE_DEVICE_TYPE_UNKNOWN = 0x00, ///< Unknown
    DMI_INTEL_RSD_STORAGE_DEVICE_TYPE_HDD     = 0x01, ///< HDD
    DMI_INTEL_RSD_STORAGE_DEVICE_TYPE_SSD     = 0x02, ///< SSD
    DMI_INTEL_RSD_STORAGE_DEVICE_TYPE_DVD     = 0x03, ///< Optical - DVD
    DMI_INTEL_RSD_STORAGE_DEVICE_TYPE_BLURAY  = 0x04, ///< Optical - Blu-ray
    DMI_INTEL_RSD_STORAGE_DEVICE_TYPE_USB     = 0x05, ///< USB
    __DMI_INTEL_RSD_STORAGE_DEVICE_TYPE_COUNT
} dmi_intel_rsd_storage_device_type_t;

/**
 * @brief Intel RSD Storage device information (type 194).
 */
struct dmi_intel_rsd_storage_device
{
    /**
     * @brief String that identifies the physically labelled port or board
     * position where the storage device is located. Example: "SATA Port 0"
     * or "PCIe Port 1".
     */
    const char *port;

    /**
     * @brief Index of this device in the IPMI Get/Set Boot Options command
     * as provided by BIOS. This is used by the PSME to identify the boot
     * device index for use in Set Boot Options IPMI command when a change
     * of the boot device is required.
     */
    unsigned index;

    /**
     * @brief Connector type indicator.
     */
    dmi_intel_rsd_storage_connector_t connector;

    /**
     * @brief Protocol type indicator.
     */
    dmi_intel_rsd_storage_proto_t protocol;

    /**
     * @brief Device type indicator.
     */
    dmi_intel_rsd_storage_device_type_t type;

    /**
     * @brief Device capacity in Gb.
     */
    uint32_t capacity;

    /**
     * @brief Device RPM (applicable for HDDs, zero for non-HDD).
     */
    uint16_t rpm;

    /**
     * @brief The string that identifies the device's model.
     */
    const char *model;

    /**
     * @brief The string that identifies the device's serial number.
     */
    const char *serial_number;

    /**
     * @brief PCI class code in case of PCIe storage device, zero otherwise.
     */
    dmi_pci_class_t pci_class;

    /**
     * @brief PCI vendor identifier in case of PCIe storage device, zero
     * otherwise.
     */
    dmi_pci_vendor_id_t vendor_id;

    /**
     * @brief PCI device identifier in case of PCIe storage device, zero
     * otherwise.
     */
    dmi_pci_device_id_t device_id;

    /**
     * @brief PCI sub-vendor identifier in case of PCIe storage device, zero
     * otherwise.
     */
    dmi_pci_vendor_id_t sub_vendor_id;

    /**
     * @brief PCI sub-device identifier in case of PCIe storage device, zero
     * otherwise.
     */
    dmi_pci_device_id_t sub_device_id;

    /**
     * @brief The string that identifies the device's firmware version.
     */
    const char *firmware_version;
};

/**
 * @brief Intel RSD storage device information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_intel_rsd_storage_device_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_intel_rsd_storage_connector_name(dmi_intel_rsd_storage_connector_t value);
__dmi_api const char *dmi_intel_rsd_storage_proto_name(dmi_intel_rsd_storage_proto_t value);
__dmi_api const char *dmi_intel_rsd_storage_device_type_name(dmi_intel_rsd_storage_device_type_t value);

__END_DECLS

#endif // !DMI_ENTITY_INTEL_RSD_STORAGE_DEVICE_H
