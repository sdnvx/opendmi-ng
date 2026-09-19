//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MGMT_CONTROLLER_H
#define OPENDMI_ENTITY_MGMT_CONTROLLER_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/utils/uuid.h>

typedef struct dmi_mgmt_controller      dmi_mgmt_controller_t;
typedef struct dmi_mgmt_nhi             dmi_mgmt_nhi_t;
typedef struct dmi_mgmt_nhi_usb         dmi_mgmt_nhi_usb_t;
typedef struct dmi_mgmt_nhi_pci         dmi_mgmt_nhi_pci_t;
typedef struct dmi_mgmt_nhi_usb_v2      dmi_mgmt_nhi_usb_v2_t;
typedef struct dmi_mgmt_nhi_pci_v2      dmi_mgmt_nhi_pci_v2_t;
typedef struct dmi_mgmt_nhi_oem         dmi_mgmt_nhi_oem_t;
typedef struct dmi_mgmt_proto_record    dmi_mgmt_proto_record_t;
typedef struct dmi_mgmt_redfish_over_ip dmi_mgmt_redfish_over_ip_t;

/**
 * @brief Management interface types.
 */
typedef enum dmi_mgmt_if_type {
    DMI_MGMT_IF_TYPE_MCTP_KCS        = 0x02, ///< Keyboard Controller Style
    DMI_MGMT_IF_TYPE_MCTP_8250_UART  = 0x03, ///< 8250 UART Register Compatible
    DMI_MGMT_IF_TYPE_MCTP_16450_UART = 0x04, ///< 16450 UART Register Compatible
    DMI_MGMT_IF_TYPE_MCTP_16550_UART = 0x05, ///< 16550/16550A UART Register Compatible
    DMI_MGMT_IF_TYPE_MCTP_16650_UART = 0x06, ///< 16650/16650A UART Register Compatible
    DMI_MGMT_IF_TYPE_MCTP_16750_UART = 0x07, ///< 16750/16750A UART Register Compatible
    DMI_MGMT_IF_TYPE_MCTP_16850_UART = 0x08, ///< 16850/16850A UART Register Compatible
    DMI_MGMT_IF_TYPE_MCTP_I2C_SMBUS  = 0x09, ///< I2C/SMBUS
    DMI_MGMT_IF_TYPE_MCTP_I3C        = 0x0A, ///< I3C
    DMI_MGMT_IF_TYPE_MCTP_PCIE_VDM   = 0x0B, ///< PCIe VDM
    DMI_MGMT_IF_TYPE_MCTP_MMBI       = 0x0C, ///< MMBI
    DMI_MGMT_IF_TYPE_MCTP_PCC        = 0x0D, ///< PCC
    DMI_MGMT_IF_TYPE_MCTP_UCIE       = 0x0E, ///< UCIe
    DMI_MGMT_IF_TYPE_MCTP_USB        = 0x0F, ///< USB
    DMI_MGMT_IF_TYPE_NETWORK_HOST_IF = 0x40, ///< Network Host Interface (DSP0270)
    DMI_MGMT_IF_TYPE_OEM             = 0xF0, ///< OEM-defined
} dmi_mgmt_if_type_t;

/**
 * @brief Management protocol types.
 */
typedef enum dmi_mgmt_proto {
    DMI_MGMT_PROTO_IPMI            = 0x02, ///< IPMI: Intelligent Platform Management Interface
    DMI_MGMT_PROTO_MCTP            = 0x03, ///< MCTP: Management Component Transport Protoco
    DMI_MGMT_PROTO_REDFISH_OVER_IP = 0x04, ///< Redfish over IP
    DMI_MGMT_PROTO_OEM             = 0xF0  ///< OEM-defined
} dmi_mgmt_proto_t;

/**
 * @brief Network host interface device types (DSP0270).
 */
typedef enum dmi_mgmt_nhi_device_type {
    DMI_MGMT_NHI_DEVICE_TYPE_USB       = 0x02, ///< USB network interface
    DMI_MGMT_NHI_DEVICE_TYPE_PCI       = 0x03, ///< PCI/PCIe network interface
    DMI_MGMT_NHI_DEVICE_TYPE_USB_V2    = 0x04, ///< USB network interface v2
    DMI_MGMT_NHI_DEVICE_TYPE_PCI_V2    = 0x05, ///< PCI/PCIe network interface v2
    DMI_MGMT_NHI_DEVICE_TYPE_OEM_START = 0x80, ///< OEM-defined, first value
    DMI_MGMT_NHI_DEVICE_TYPE_OEM_END   = 0xFF  ///< OEM-defined, last value
} dmi_mgmt_nhi_device_type_t;

/**
 * @brief Network host interface device descriptor formats.
 */
typedef enum dmi_mgmt_nhi_format {
    DMI_MGMT_NHI_FORMAT_RAW,    ///< Unknown or malformed descriptor, raw data
    DMI_MGMT_NHI_FORMAT_USB,    ///< USB network interface
    DMI_MGMT_NHI_FORMAT_PCI,    ///< PCI/PCIe network interface
    DMI_MGMT_NHI_FORMAT_USB_V2, ///< USB network interface v2
    DMI_MGMT_NHI_FORMAT_PCI_V2, ///< PCI/PCIe network interface v2
    DMI_MGMT_NHI_FORMAT_OEM     ///< OEM-defined device
} dmi_mgmt_nhi_format_t;

/**
 * @brief Network host interface device characteristics (bit numbers).
 */
typedef enum dmi_mgmt_nhi_characteristic {
    DMI_MGMT_NHI_CHAR_CREDENTIAL_BOOTSTRAPPING = 0 ///< Credential bootstrapping via IPMI is supported
} dmi_mgmt_nhi_characteristic_t;

/**
 * @brief Redfish host IP address assignment and service IP address
 * discovery types (DSP0270).
 */
typedef enum dmi_mgmt_redfish_ip_assignment {
    DMI_MGMT_REDFISH_IP_ASSIGNMENT_UNKNOWN        = 0x00, ///< Unknown
    DMI_MGMT_REDFISH_IP_ASSIGNMENT_STATIC         = 0x01, ///< Static
    DMI_MGMT_REDFISH_IP_ASSIGNMENT_DHCP           = 0x02, ///< DHCP
    DMI_MGMT_REDFISH_IP_ASSIGNMENT_AUTO_CONFIGURE = 0x03, ///< AutoConfigure
    DMI_MGMT_REDFISH_IP_ASSIGNMENT_HOST_SELECTED  = 0x04  ///< HostSelected
} dmi_mgmt_redfish_ip_assignment_t;

/**
 * @brief Redfish IP address formats (DSP0270).
 */
typedef enum dmi_mgmt_redfish_ip_format {
    DMI_MGMT_REDFISH_IP_FORMAT_UNKNOWN = 0x00, ///< Unknown
    DMI_MGMT_REDFISH_IP_FORMAT_IPV4    = 0x01, ///< IPv4
    DMI_MGMT_REDFISH_IP_FORMAT_IPV6    = 0x02  ///< IPv6
} dmi_mgmt_redfish_ip_format_t;

/**
 * @brief USB network interface device descriptor (device type 02h).
 */
struct dmi_mgmt_nhi_usb
{
    /**
     * @brief Vendor ID, as read from the idVendor field of the USB descriptor.
     */
    uint16_t vendor_id;

    /**
     * @brief Product ID, as read from the idProduct field of the USB
     * descriptor.
     */
    uint16_t product_id;

    /**
     * @brief Serial number, converted from the UTF-16 string of the USB
     * descriptor to UTF-8.
     */
    char *serial_number;
};

/**
 * @brief PCI/PCIe network interface device descriptor (device type 03h).
 */
struct dmi_mgmt_nhi_pci
{
    uint16_t vendor_id;        ///< Vendor ID
    uint16_t device_id;        ///< Device ID
    uint16_t subsys_vendor_id; ///< Subsystem vendor ID
    uint16_t subsys_id;        ///< Subsystem ID
};

/**
 * @brief USB network interface v2 device descriptor (device type 04h).
 */
struct dmi_mgmt_nhi_usb_v2
{
    /**
     * @brief Vendor ID, as read from the idVendor field of the USB descriptor.
     */
    uint16_t vendor_id;

    /**
     * @brief Product ID, as read from the idProduct field of the USB
     * descriptor.
     */
    uint16_t product_id;

    /**
     * @brief Serial number.
     */
    const char *serial_number;

    /**
     * @brief MAC address of the USB network device.
     */
    dmi_binary_t mac_address;

    /**
     * @brief Device characteristics, see `dmi_mgmt_nhi_characteristic_t`.
     *
     * @since DSP0270 1.3
     */
    uint16_t characteristics;

    /**
     * @brief Handle of the interface to be used for credential bootstrapping
     * via IPMI commands, `DMI_HANDLE_INVALID` if not supported.
     *
     * @since DSP0270 1.3
     */
    dmi_handle_t credential_handle;
};

/**
 * @brief PCI/PCIe network interface v2 device descriptor (device type 05h).
 */
struct dmi_mgmt_nhi_pci_v2
{
    uint16_t vendor_id;        ///< Vendor ID
    uint16_t device_id;        ///< Device ID
    uint16_t subsys_vendor_id; ///< Subsystem vendor ID
    uint16_t subsys_id;        ///< Subsystem ID

    /**
     * @brief MAC address of the PCI/PCIe network device.
     */
    dmi_binary_t mac_address;

    /**
     * @brief Segment group number, zero for a single-segment topology.
     */
    uint16_t segment_group;

    uint8_t bus_number;      ///< Bus number
    uint8_t device_number;   ///< Device number
    uint8_t function_number; ///< Function number

    /**
     * @brief Device characteristics, see `dmi_mgmt_nhi_characteristic_t`.
     *
     * @since DSP0270 1.3
     */
    uint16_t characteristics;

    /**
     * @brief Handle of the interface to be used for credential bootstrapping
     * via IPMI commands, `DMI_HANDLE_INVALID` if not supported.
     *
     * @since DSP0270 1.3
     */
    dmi_handle_t credential_handle;
};

/**
 * @brief OEM-defined device descriptor (device types 80h-FFh).
 */
struct dmi_mgmt_nhi_oem
{
    /**
     * @brief IANA enterprise number of the vendor.
     */
    uint32_t vendor_iana;

    /**
     * @brief OEM-defined data.
     */
    dmi_binary_t vendor_data;
};

/**
 * @brief Network host interface data (interface type 40h, DSP0270).
 */
struct dmi_mgmt_nhi
{
    /**
     * @brief Device type.
     */
    dmi_mgmt_nhi_device_type_t device_type;

    /**
     * @brief Device descriptor format, selects the descriptor below.
     */
    dmi_mgmt_nhi_format_t format;

    /**
     * @brief Device descriptor data, as stored.
     */
    dmi_binary_t descriptor;

    dmi_mgmt_nhi_usb_t    usb;    ///< USB network interface
    dmi_mgmt_nhi_pci_t    pci;    ///< PCI/PCIe network interface
    dmi_mgmt_nhi_usb_v2_t usb_v2; ///< USB network interface v2
    dmi_mgmt_nhi_pci_v2_t pci_v2; ///< PCI/PCIe network interface v2
    dmi_mgmt_nhi_oem_t    oem;    ///< OEM-defined device
};

/**
 * @brief Redfish over IP protocol record data (DSP0270).
 */
struct dmi_mgmt_redfish_over_ip
{
    /**
     * @brief Redfish service UUID, all zeroes if not supported or unknown.
     */
    dmi_uuid_t service_uuid;

    /**
     * @brief Host IP address assignment type.
     */
    dmi_mgmt_redfish_ip_assignment_t host_ip_assignment;

    /**
     * @brief Host IP address format.
     */
    dmi_mgmt_redfish_ip_format_t host_ip_format;

    /**
     * @brief Host IP address, 4 bytes for IPv4 and 16 bytes otherwise.
     */
    dmi_binary_t host_ip_address;

    /**
     * @brief Host IP mask, 4 bytes for IPv4 and 16 bytes otherwise.
     */
    dmi_binary_t host_ip_mask;

    /**
     * @brief Redfish service IP address discovery type.
     */
    dmi_mgmt_redfish_ip_assignment_t service_ip_discovery;

    /**
     * @brief Redfish service IP address format.
     */
    dmi_mgmt_redfish_ip_format_t service_ip_format;

    /**
     * @brief Redfish service IP address, 4 bytes for IPv4 and 16 bytes
     * otherwise.
     */
    dmi_binary_t service_ip_address;

    /**
     * @brief Redfish service IP mask, 4 bytes for IPv4 and 16 bytes
     * otherwise.
     */
    dmi_binary_t service_ip_mask;

    /**
     * @brief Redfish service IP port.
     */
    uint16_t service_ip_port;

    /**
     * @brief Redfish service VLAN ID.
     */
    uint32_t service_vlan_id;

    /**
     * @brief Redfish service hostname, @c nullptr if empty.
     */
    char *service_hostname;
};

/**
 * @brief Management controller host interface structure (type 42).
 */
struct dmi_mgmt_controller
{
    /**
     * @brief Management controller interface type.
     */
    dmi_mgmt_if_type_t if_type;

    /**
     * @brief Interface type-specific data, as stored.
     */
    dmi_binary_t if_data;

    /**
     * @brief Set if interface-specific data is decoded as network host
     * interface data.
     */
    bool has_nhi;

    /**
     * @brief Network host interface data.
     */
    dmi_mgmt_nhi_t nhi;

    /**
     * @brief Number of completely present protocol records.
     *
     * @since SMBIOS 3.2
     */
    size_t proto_records_count;

    /**
     * @brief Protocol records.
     *
     * @since SMBIOS 3.2
     */
    dmi_mgmt_proto_record_t *proto_records;
};

/**
 * @brief Protocol record.
 */
struct dmi_mgmt_proto_record
{
    /**
     * @brief Protocol type.
     */
    dmi_mgmt_proto_t type;

    /**
     * @brief Protocol-specific data, as stored.
     */
    dmi_binary_t data;

    /**
     * @brief Set if protocol-specific data is decoded as Redfish over IP
     * record data.
     */
    bool has_redfish;

    /**
     * @brief Redfish over IP record data.
     */
    dmi_mgmt_redfish_over_ip_t redfish;
};

/**
 * @brief Management controller host interface entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_mgmt_controller_host_if_spec;

#endif // !OPENDMI_ENTITY_MGMT_CONTROLLER_H
