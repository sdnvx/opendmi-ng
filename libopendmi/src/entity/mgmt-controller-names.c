//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>

#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/stream.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>
#include <opendmi/utils/endian.h>

#include <opendmi/entity/mgmt-controller-internal.h>

const dmi_name_set_t dmi_mgmt_if_type_names =
{
    .code  = "mgmt-if-type",
    .names = DMI_NAMES({
        {
            .id   = DMI_MGMT_IF_TYPE_MCTP_KCS,
            .code = "keyboard-controller-style",
            .name = "Keyboard Controller Style"
        },
        {
            .id   = DMI_MGMT_IF_TYPE_MCTP_8250_UART,
            .code = "8250-uart",
            .name = "8250 UART Register Compatible"
        },
        {
            .id   = DMI_MGMT_IF_TYPE_MCTP_16450_UART,
            .code = "16450-uart",
            .name = "16450 UART Register Compatible"
        },
        {
            .id   = DMI_MGMT_IF_TYPE_MCTP_16550_UART,
            .code = "16550-uart",
            .name = "16550/16550A UART Register Compatible"
        },
        {
            .id   = DMI_MGMT_IF_TYPE_MCTP_16650_UART,
            .code = "16650-uart",
            .name = "16650/16650A UART Register Compatible"
        },
        {
            .id   = DMI_MGMT_IF_TYPE_MCTP_16750_UART,
            .code = "16750-uart",
            .name = "16750/16750A UART Register Compatible"
        },
        {
            .id   = DMI_MGMT_IF_TYPE_MCTP_16850_UART,
            .code = "16850-uart",
            .name = "16850/16850A UART Register Compatible"
        },
        {
            .id   = DMI_MGMT_IF_TYPE_MCTP_I2C_SMBUS,
            .code = "i2c-smbus",
            .name = "I2C/SMBUS"
        },
        {
            .id   = DMI_MGMT_IF_TYPE_MCTP_I3C,
            .code = "i3c",
            .name = "I3C"
        },
        {
            .id   = DMI_MGMT_IF_TYPE_MCTP_PCIE_VDM,
            .code = "pcie-vdm",
            .name = "PCIe VDM"
        },
        {
            .id   = DMI_MGMT_IF_TYPE_MCTP_MMBI,
            .code = "mmbi",
            .name = "MMBI"
        },
        {
            .id   = DMI_MGMT_IF_TYPE_MCTP_PCC,
            .code = "pcc",
            .name = "PCC"
        },
        {
            .id   = DMI_MGMT_IF_TYPE_MCTP_UCIE,
            .code = "ucie",
            .name = "UCIe"
        },
        {
            .id   = DMI_MGMT_IF_TYPE_MCTP_USB,
            .code = "usb",
            .name = "USB"
        },
        {
            .id   = DMI_MGMT_IF_TYPE_NETWORK_HOST_IF,
            .code = "network-host-interface",
            .name = "Network Host Interface"
        },
        {
            .id   = DMI_MGMT_IF_TYPE_OEM,
            .code = "oem-defined",
            .name = "OEM-defined"
        },
        {}
    })
};

const dmi_name_set_t dmi_mgmt_proto_names =
{
    .code  = "mgmt-protocol",
    .names = DMI_NAMES({
        {
            .id   = DMI_MGMT_PROTO_IPMI,
            .code = "ipmi",
            .name = "IPMI"
        },
        {
            .id   = DMI_MGMT_PROTO_MCTP,
            .code = "mctp",
            .name = "MCTP"
        },
        {
            .id   = DMI_MGMT_PROTO_REDFISH_OVER_IP,
            .code = "redfish-over-ip",
            .name = "Redfish over IP"
        },
        {
            .id   = DMI_MGMT_PROTO_OEM,
            .code = "oem-defined",
            .name = "OEM-defined"
        },
        {}
    })
};

const dmi_name_set_t dmi_mgmt_nhi_device_type_names =
{
    .code   = "mgmt-nhi-device-type",
    .names  = DMI_NAMES({
        {
            .id   = DMI_MGMT_NHI_DEVICE_TYPE_USB,
            .code = "usb",
            .name = "USB network interface"
        },
        {
            .id   = DMI_MGMT_NHI_DEVICE_TYPE_PCI,
            .code = "pci",
            .name = "PCI/PCIe network interface"
        },
        {
            .id   = DMI_MGMT_NHI_DEVICE_TYPE_USB_V2,
            .code = "usb-v2",
            .name = "USB network interface v2"
        },
        {
            .id   = DMI_MGMT_NHI_DEVICE_TYPE_PCI_V2,
            .code = "pci-v2",
            .name = "PCI/PCIe network interface v2"
        },
        {}
    }),
    .ranges = DMI_NAME_RANGES({
        {
            .start_id = DMI_MGMT_NHI_DEVICE_TYPE_OEM_START,
            .end_id   = DMI_MGMT_NHI_DEVICE_TYPE_OEM_END,
            .code     = "oem-defined",
            .name     = "OEM-defined"
        },
        {}
    })
};

const dmi_name_set_t dmi_mgmt_nhi_characteristic_names =
{
    .code  = "mgmt-nhi-characteristic",
    .names = DMI_NAMES({
        {
            .id   = DMI_MGMT_NHI_CHAR_CREDENTIAL_BOOTSTRAPPING,
            .code = "credential-bootstrapping",
            .name = "Credential bootstrapping via IPMI commands"
        },
        {}
    })
};

const dmi_name_set_t dmi_mgmt_redfish_ip_assignment_names =
{
    .code  = "mgmt-redfish-ip-assignment",
    .names = DMI_NAMES({
        {
            .id   = DMI_MGMT_REDFISH_IP_ASSIGNMENT_UNKNOWN,
            .code = "unknown",
            .name = "Unknown"
        },
        {
            .id   = DMI_MGMT_REDFISH_IP_ASSIGNMENT_STATIC,
            .code = "static",
            .name = "Static"
        },
        {
            .id   = DMI_MGMT_REDFISH_IP_ASSIGNMENT_DHCP,
            .code = "dhcp",
            .name = "DHCP"
        },
        {
            .id   = DMI_MGMT_REDFISH_IP_ASSIGNMENT_AUTO_CONFIGURE,
            .code = "auto-configure",
            .name = "AutoConfigure"
        },
        {
            .id   = DMI_MGMT_REDFISH_IP_ASSIGNMENT_HOST_SELECTED,
            .code = "host-selected",
            .name = "HostSelected"
        },
        {}
    })
};

const dmi_name_set_t dmi_mgmt_redfish_ip_format_names =
{
    .code  = "mgmt-redfish-ip-format",
    .names = DMI_NAMES({
        {
            .id   = DMI_MGMT_REDFISH_IP_FORMAT_UNKNOWN,
            .code = "unknown",
            .name = "Unknown"
        },
        {
            .id   = DMI_MGMT_REDFISH_IP_FORMAT_IPV4,
            .code = "ipv4",
            .name = "IPv4"
        },
        {
            .id   = DMI_MGMT_REDFISH_IP_FORMAT_IPV6,
            .code = "ipv6",
            .name = "IPv6"
        },
        {}
    })
};
