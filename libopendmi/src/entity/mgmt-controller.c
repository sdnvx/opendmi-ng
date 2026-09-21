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

#include <opendmi/entity/mgmt-controller.h>

static bool dmi_mgmt_controller_decode(dmi_entity_t *entity);
static void dmi_mgmt_controller_cleanup(dmi_entity_t *entity);

const dmi_name_set_t dmi_mgmt_if_type_names =
{
    .code  = "mgmt-if-type",
    .names = (dmi_name_t[]){
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
        DMI_NAME_NULL
    }
};

const dmi_name_set_t dmi_mgmt_proto_names =
{
    .code  = "mgmt-protocol",
    .names = (dmi_name_t[]){
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
        DMI_NAME_NULL
    }
};

const dmi_name_set_t dmi_mgmt_nhi_device_type_names =
{
    .code   = "mgmt-nhi-device-type",
    .names  = (dmi_name_t[]){
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
        DMI_NAME_NULL
    },
    .ranges = (dmi_name_range_t[]){
        {
            .start_id = DMI_MGMT_NHI_DEVICE_TYPE_OEM_START,
            .end_id   = DMI_MGMT_NHI_DEVICE_TYPE_OEM_END,
            .code     = "oem-defined",
            .name     = "OEM-defined"
        },
        DMI_NAME_RANGE_NULL
    }
};

const dmi_name_set_t dmi_mgmt_nhi_characteristic_names =
{
    .code  = "mgmt-nhi-characteristic",
    .names = (dmi_name_t[]){
        {
            .id   = DMI_MGMT_NHI_CHAR_CREDENTIAL_BOOTSTRAPPING,
            .code = "credential-bootstrapping",
            .name = "Credential bootstrapping via IPMI commands"
        },
        DMI_NAME_NULL
    }
};

const dmi_name_set_t dmi_mgmt_redfish_ip_assignment_names =
{
    .code  = "mgmt-redfish-ip-assignment",
    .names = (dmi_name_t[]){
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
        DMI_NAME_NULL
    }
};

const dmi_name_set_t dmi_mgmt_redfish_ip_format_names =
{
    .code  = "mgmt-redfish-ip-format",
    .names = (dmi_name_t[]){
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
        DMI_NAME_NULL
    }
};

#define DMI_MGMT_NHI_PCI_ATTRS(__type)                                        \
    DMI_ATTRIBUTE(__type, vendor_id, INTEGER, {                               \
        .code  = "vendor-id",                                                 \
        .name  = "Vendor ID",                                                 \
        .flags = DMI_ATTRIBUTE_FLAG_HEX                                       \
    }),                                                                       \
    DMI_ATTRIBUTE(__type, device_id, INTEGER, {                               \
        .code  = "device-id",                                                 \
        .name  = "Device ID",                                                 \
        .flags = DMI_ATTRIBUTE_FLAG_HEX                                       \
    }),                                                                       \
    DMI_ATTRIBUTE(__type, subsys_vendor_id, INTEGER, {                        \
        .code  = "subsystem-vendor-id",                                       \
        .name  = "Subsystem vendor ID",                                       \
        .flags = DMI_ATTRIBUTE_FLAG_HEX                                       \
    }),                                                                       \
    DMI_ATTRIBUTE(__type, subsys_id, INTEGER, {                               \
        .code  = "subsystem-id",                                              \
        .name  = "Subsystem ID",                                              \
        .flags = DMI_ATTRIBUTE_FLAG_HEX                                       \
    })

#define DMI_MGMT_NHI_USB_ATTRS(__type)                                        \
    DMI_ATTRIBUTE(__type, vendor_id, INTEGER, {                               \
        .code  = "vendor-id",                                                 \
        .name  = "Vendor ID",                                                 \
        .flags = DMI_ATTRIBUTE_FLAG_HEX                                       \
    }),                                                                       \
    DMI_ATTRIBUTE(__type, product_id, INTEGER, {                              \
        .code  = "product-id",                                                \
        .name  = "Product ID",                                                \
        .flags = DMI_ATTRIBUTE_FLAG_HEX                                       \
    }),                                                                       \
    DMI_ATTRIBUTE(__type, serial_number, STRING, {                            \
        .code  = "serial-number",                                             \
        .name  = "Serial number"                                              \
    })

#define DMI_MGMT_NHI_V2_ATTRS(__type)                                         \
    DMI_ATTRIBUTE(__type, characteristics, SET, {                             \
        .code   = "characteristics",                                          \
        .name   = "Device characteristics",                                   \
        .values = &dmi_mgmt_nhi_characteristic_names                          \
    }),                                                                       \
    DMI_ATTRIBUTE(__type, credential_handle, HANDLE, {                        \
        .code   = "credential-bootstrapping-handle",                          \
        .name   = "Credential bootstrapping handle"                           \
    })

static const dmi_attribute_t dmi_mgmt_nhi_usb_attrs[] =
{
    DMI_MGMT_NHI_USB_ATTRS(dmi_mgmt_nhi_usb_t),
    DMI_ATTRIBUTE_NULL
};

static const dmi_attribute_t dmi_mgmt_nhi_pci_attrs[] =
{
    DMI_MGMT_NHI_PCI_ATTRS(dmi_mgmt_nhi_pci_t),
    DMI_ATTRIBUTE_NULL
};

static const dmi_attribute_t dmi_mgmt_nhi_usb_v2_attrs[] =
{
    DMI_MGMT_NHI_USB_ATTRS(dmi_mgmt_nhi_usb_v2_t),
    DMI_ATTRIBUTE(dmi_mgmt_nhi_usb_v2_t, mac_address, BINARY, {
        .code  = "mac-address",
        .name  = "MAC address",
        .flags = DMI_ATTRIBUTE_FLAG_MAC
    }),
    DMI_MGMT_NHI_V2_ATTRS(dmi_mgmt_nhi_usb_v2_t),
    DMI_ATTRIBUTE_NULL
};

static const dmi_attribute_t dmi_mgmt_nhi_pci_v2_attrs[] =
{
    DMI_MGMT_NHI_PCI_ATTRS(dmi_mgmt_nhi_pci_v2_t),
    DMI_ATTRIBUTE(dmi_mgmt_nhi_pci_v2_t, mac_address, BINARY, {
        .code  = "mac-address",
        .name  = "MAC address",
        .flags = DMI_ATTRIBUTE_FLAG_MAC
    }),
    DMI_ATTRIBUTE(dmi_mgmt_nhi_pci_v2_t, segment_group, INTEGER, {
        .code  = "segment-group",
        .name  = "Segment group number"
    }),
    DMI_ATTRIBUTE(dmi_mgmt_nhi_pci_v2_t, bus_number, INTEGER, {
        .code  = "bus-number",
        .name  = "Bus number",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_mgmt_nhi_pci_v2_t, device_number, INTEGER, {
        .code  = "device-number",
        .name  = "Device number",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_mgmt_nhi_pci_v2_t, function_number, INTEGER, {
        .code  = "function-number",
        .name  = "Function number"
    }),
    DMI_MGMT_NHI_V2_ATTRS(dmi_mgmt_nhi_pci_v2_t),
    DMI_ATTRIBUTE_NULL
};

static const dmi_attribute_t dmi_mgmt_nhi_oem_attrs[] =
{
    DMI_ATTRIBUTE(dmi_mgmt_nhi_oem_t, vendor_iana, INTEGER, {
        .code  = "vendor-iana",
        .name  = "Vendor IANA enterprise number"
    }),
    DMI_ATTRIBUTE(dmi_mgmt_nhi_oem_t, vendor_data, BINARY, {
        .code  = "vendor-data",
        .name  = "Vendor data"
    }),
    DMI_ATTRIBUTE_NULL
};

static const dmi_attribute_t dmi_mgmt_nhi_attrs[] =
{
    DMI_ATTRIBUTE(dmi_mgmt_nhi_t, device_type, ENUM, {
        .code   = "device-type",
        .name   = "Device type",
        .values = &dmi_mgmt_nhi_device_type_names
    }),
    DMI_ATTRIBUTE_VARIANT(dmi_mgmt_nhi_t, format, {
        .code     = "device-descriptor",
        .name     = "Device descriptor",
        .variants = (const dmi_attribute_variant_t[]){
            DMI_VARIANT(DMI_MGMT_NHI_FORMAT_USB, dmi_mgmt_nhi_t, usb, STRUCT, {
                .attrs = dmi_mgmt_nhi_usb_attrs
            }),
            DMI_VARIANT(DMI_MGMT_NHI_FORMAT_PCI, dmi_mgmt_nhi_t, pci, STRUCT, {
                .attrs = dmi_mgmt_nhi_pci_attrs
            }),
            DMI_VARIANT(DMI_MGMT_NHI_FORMAT_USB_V2, dmi_mgmt_nhi_t, usb_v2, STRUCT, {
                .attrs = dmi_mgmt_nhi_usb_v2_attrs
            }),
            DMI_VARIANT(DMI_MGMT_NHI_FORMAT_PCI_V2, dmi_mgmt_nhi_t, pci_v2, STRUCT, {
                .attrs = dmi_mgmt_nhi_pci_v2_attrs
            }),
            DMI_VARIANT(DMI_MGMT_NHI_FORMAT_OEM, dmi_mgmt_nhi_t, oem, STRUCT, {
                .attrs = dmi_mgmt_nhi_oem_attrs
            }),
            DMI_VARIANT_DEFAULT(dmi_mgmt_nhi_t, descriptor, BINARY, {}),
            DMI_VARIANT_NULL
        }
    }),
    DMI_ATTRIBUTE_NULL
};

// IP addresses are formatted as such only if their format is known
#define DMI_MGMT_REDFISH_IP_ATTR(__member, __format, __code, __name)          \
    DMI_ATTRIBUTE_VARIANT(dmi_mgmt_redfish_over_ip_t, __format, {             \
        .code     = __code,                                                   \
        .name     = __name,                                                   \
        .variants = (const dmi_attribute_variant_t[]){                        \
            DMI_VARIANT(DMI_MGMT_REDFISH_IP_FORMAT_IPV4,                      \
                        dmi_mgmt_redfish_over_ip_t, __member, BINARY, {       \
                .flags = DMI_ATTRIBUTE_FLAG_IP                                \
            }),                                                               \
            DMI_VARIANT(DMI_MGMT_REDFISH_IP_FORMAT_IPV6,                      \
                        dmi_mgmt_redfish_over_ip_t, __member, BINARY, {       \
                .flags = DMI_ATTRIBUTE_FLAG_IP                                \
            }),                                                               \
            DMI_VARIANT_DEFAULT(dmi_mgmt_redfish_over_ip_t, __member, BINARY, {}), \
            DMI_VARIANT_NULL                                                  \
        }                                                                     \
    })

static const dmi_attribute_t dmi_mgmt_redfish_over_ip_attrs[] =
{
    DMI_ATTRIBUTE(dmi_mgmt_redfish_over_ip_t, service_uuid, UUID, {
        .code   = "service-uuid",
        .name   = "Service UUID"
    }),
    DMI_ATTRIBUTE(dmi_mgmt_redfish_over_ip_t, host_ip_assignment, ENUM, {
        .code   = "host-ip-assignment",
        .name   = "Host IP assignment type",
        .values = &dmi_mgmt_redfish_ip_assignment_names
    }),
    DMI_ATTRIBUTE(dmi_mgmt_redfish_over_ip_t, host_ip_format, ENUM, {
        .code   = "host-ip-format",
        .name   = "Host IP address format",
        .values = &dmi_mgmt_redfish_ip_format_names
    }),
    DMI_MGMT_REDFISH_IP_ATTR(host_ip_address, host_ip_format, "host-ip-address", "Host IP address"),
    DMI_MGMT_REDFISH_IP_ATTR(host_ip_mask, host_ip_format, "host-ip-mask", "Host IP mask"),
    DMI_ATTRIBUTE(dmi_mgmt_redfish_over_ip_t, service_ip_discovery, ENUM, {
        .code   = "service-ip-discovery",
        .name   = "Service IP discovery type",
        .values = &dmi_mgmt_redfish_ip_assignment_names
    }),
    DMI_ATTRIBUTE(dmi_mgmt_redfish_over_ip_t, service_ip_format, ENUM, {
        .code   = "service-ip-format",
        .name   = "Service IP address format",
        .values = &dmi_mgmt_redfish_ip_format_names
    }),
    DMI_MGMT_REDFISH_IP_ATTR(service_ip_address, service_ip_format, "service-ip-address", "Service IP address"),
    DMI_MGMT_REDFISH_IP_ATTR(service_ip_mask, service_ip_format, "service-ip-mask", "Service IP mask"),
    DMI_ATTRIBUTE(dmi_mgmt_redfish_over_ip_t, service_ip_port, INTEGER, {
        .code   = "service-ip-port",
        .name   = "Service IP port"
    }),
    DMI_ATTRIBUTE(dmi_mgmt_redfish_over_ip_t, service_vlan_id, INTEGER, {
        .code   = "service-vlan-id",
        .name   = "Service VLAN ID"
    }),
    DMI_ATTRIBUTE(dmi_mgmt_redfish_over_ip_t, service_hostname, STRING, {
        .code   = "service-hostname",
        .name   = "Service hostname"
    }),
    DMI_ATTRIBUTE_NULL
};

static const dmi_attribute_t dmi_mgmt_proto_record_attrs[] =
{
    DMI_ATTRIBUTE(dmi_mgmt_proto_record_t, type, ENUM, {
        .code   = "type",
        .name   = "Protocol type",
        .values = &dmi_mgmt_proto_names
    }),
    DMI_ATTRIBUTE_VARIANT(dmi_mgmt_proto_record_t, has_redfish, {
        .code     = "data",
        .name     = "Protocol-specific data",
        .variants = (const dmi_attribute_variant_t[]){
            DMI_VARIANT(true, dmi_mgmt_proto_record_t, redfish, STRUCT, {
                .attrs = dmi_mgmt_redfish_over_ip_attrs
            }),
            DMI_VARIANT_DEFAULT(dmi_mgmt_proto_record_t, data, BINARY, {}),
            DMI_VARIANT_NULL
        }
    }),
    DMI_ATTRIBUTE_NULL
};

static void dmi_mgmt_controller_lint_records(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Offset of the length of the interface-specific data, which the
 * protocol records follow.
 */
#define DMI_MGMT_CONTROLLER_IF_LENGTH_OFFSET 0x05

static const dmi_lint_rule_t dmi_mgmt_controller_records_rule =
{
    .code              = "mgmt-controller-host-if.records",
    .name              = "Protocol records fit the structure holding them",
    .severity          = DMI_LINT_SEVERITY_WARNING,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_mgmt_controller_lint_records
};

const dmi_entity_spec_t dmi_mgmt_controller_host_if_spec =
{
    .code            = "mgmt-controller-host-if",
    .name            = "Management controller host interface",
    .description     = (const char *[]){
        "The information in this structure defines the attributes of a "
        "Management Controller Host Interface that is not discoverable by "
        "\"Plug and Play\" mechanisms. The Type 42 structure can be used to "
        "describe a physical management controller host interface and one or "
        "more protocols that share that interface.",
        //
        "Type 42 should be used for management controller host interfaces "
        "that use protocols other than IPMI or that use multiple protocols on "
        "a single host interface type.",
        //
        "This structure should also be provided if IPMI is shared with other "
        "protocols over the same interface hardware. If IPMI is not shared "
        "with other protocols, either the Type 38 or the Type 42 structures "
        "can be used.",
        //
        "Providing Type 38 is recommended for backward compatibility. The "
        "structures are not required to be mutually exclusive. Type 38 and "
        "Type 42 structures may be implemented simultaneously to provide "
        "backward compatibility with IPMI applications or drivers that do not "
        "yet recognize the Type 42 structure.",
        //
        "See the Intelligent Platform Management Interface (IPMI) Interface "
        "Specification for full documentation of IPMI and additional "
        "information on the use of this structure with IPMI.",
        //
        nullptr
    },
    .type            = DMI_TYPE(MGMT_CONTROLLER_HOST_IF),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x06,
    .decoded_length  = sizeof(dmi_mgmt_controller_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_mgmt_controller_t, if_type, ENUM, {
            .code    = "if-type",
            .name    = "Interface type",
            .values  = &dmi_mgmt_if_type_names
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_mgmt_controller_t, has_nhi, {
            .code     = "if-data",
            .name     = "Interface-specific data",
            .variants = (const dmi_attribute_variant_t[]){
                DMI_VARIANT(true, dmi_mgmt_controller_t, nhi, STRUCT, {
                    .attrs = dmi_mgmt_nhi_attrs
                }),
                DMI_VARIANT_DEFAULT(dmi_mgmt_controller_t, if_data, BINARY, {}),
                DMI_VARIANT_NULL
            }
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_mgmt_controller_t, proto_records, proto_records_count, STRUCT, {
            .code    = "protocol-records",
            .name    = "Protocol records",
            .attrs   = dmi_mgmt_proto_record_attrs,
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE_NULL
    },
    .lint_rules      = (const dmi_lint_rule_t *const[]){
        &dmi_mgmt_controller_records_rule,
        nullptr
    },

    .handlers = {
        .decode  = dmi_mgmt_controller_decode,
        .cleanup = dmi_mgmt_controller_cleanup
    }
};

/**
 * @brief Convert UTF-16LE string to UTF-8.
 *
 * Unpaired surrogates are replaced with U+FFFD.
 *
 * @return Allocated string, or @c nullptr if @p length is zero or on
 *         allocation failure.
 */
static char *dmi_mgmt_utf16_decode(dmi_context_t *context, const dmi_byte_t *data, size_t length)
{
    size_t count = length / 2;
    if (count == 0)
        return nullptr;

    // Every UTF-16 code unit takes at most 3 bytes in UTF-8
    char *str = dmi_alloc(context, count * 3 + 1);
    if (str == nullptr)
        return nullptr;

    unsigned char *pos = (unsigned char *)str;

    for (size_t i = 0; i < count; i++) {
        uint32_t code = (uint32_t)(data[i * 2] | (data[i * 2 + 1] << 8));

        if ((code >= 0xD800) and (code <= 0xDBFF) and (i + 1 < count)) {
            uint32_t low = (uint32_t)(data[i * 2 + 2] | (data[i * 2 + 3] << 8));
            if ((low >= 0xDC00) and (low <= 0xDFFF)) {
                code = 0x10000 + ((code - 0xD800) << 10) + (low - 0xDC00);
                i++;
            }
        }
        if ((code >= 0xD800) and (code <= 0xDFFF))
            code = 0xFFFD;

        // Surrogate pairs take 4 bytes in UTF-8, as two code units do
        if (code < 0x80) {
            *pos++ = (unsigned char)code;
        } else if (code < 0x800) {
            *pos++ = (unsigned char)(0xC0 | (code >> 6));
            *pos++ = (unsigned char)(0x80 | (code & 0x3F));
        } else if (code < 0x10000) {
            *pos++ = (unsigned char)(0xE0 | (code >> 12));
            *pos++ = (unsigned char)(0x80 | ((code >> 6) & 0x3F));
            *pos++ = (unsigned char)(0x80 | (code & 0x3F));
        } else {
            *pos++ = (unsigned char)(0xF0 | (code >> 18));
            *pos++ = (unsigned char)(0x80 | ((code >> 12) & 0x3F));
            *pos++ = (unsigned char)(0x80 | ((code >> 6) & 0x3F));
            *pos++ = (unsigned char)(0x80 | (code & 0x3F));
        }
    }

    *pos = 0;

    return str;
}

/**
 * @brief Decode device descriptor of network host interface.
 *
 * Descriptors that do not fit into @p length are left in raw format.
 *
 * @return `false` on allocation failure, `true` otherwise.
 */
static bool dmi_mgmt_nhi_decode(dmi_entity_t *entity, dmi_mgmt_nhi_t *nhi, size_t length)
{
    dmi_context_t *context = dmi_entity_context(entity);
    dmi_stream_t  *stream  = dmi_entity_stream(entity);

    dmi_byte_t device_type = 0;

    if (not dmi_stream_decode(stream, dmi_byte_t, &device_type))
        return true;

    nhi->device_type = dmi_cast(nhi->device_type, device_type);
    nhi->format      = DMI_MGMT_NHI_FORMAT_RAW;

    size_t start = stream->position;
    size_t size  = length - 1;

    if (not dmi_stream_decode_bin(stream, size, &nhi->descriptor))
        return true;

    dmi_stream_seek(stream, start);

    // Length of v2 descriptors includes device type and length fields
    dmi_byte_t v2_length = 0;
    bool status = true;

    switch (nhi->device_type) {
    case DMI_MGMT_NHI_DEVICE_TYPE_USB: {
        dmi_mgmt_nhi_usb_t *usb = &nhi->usb;
        dmi_byte_t serial_length = 0;

        status =
            (size >= 6) and
            dmi_stream_decode(stream, dmi_word_t, &usb->vendor_id) and
            dmi_stream_decode(stream, dmi_word_t, &usb->product_id) and
            dmi_stream_decode(stream, dmi_byte_t, &serial_length);
        if (not status)
            break;

        // Serial number descriptor length includes its length and type
        if ((serial_length < 2) or (serial_length > size - 4))
            break;

        // Serial number is a UTF-16 string, that follows the descriptor
        // length and type
        if (serial_length > 2) {
            usb->serial_number = dmi_mgmt_utf16_decode(context, nhi->descriptor.data + 6,
                                                       serial_length - 2);
            if (usb->serial_number == nullptr)
                return false;
        }

        nhi->format = DMI_MGMT_NHI_FORMAT_USB;
        break;
    }

    case DMI_MGMT_NHI_DEVICE_TYPE_PCI: {
        dmi_mgmt_nhi_pci_t *pci = &nhi->pci;

        status =
            (size >= 8) and
            dmi_stream_decode(stream, dmi_word_t, &pci->vendor_id) and
            dmi_stream_decode(stream, dmi_word_t, &pci->device_id) and
            dmi_stream_decode(stream, dmi_word_t, &pci->subsys_vendor_id) and
            dmi_stream_decode(stream, dmi_word_t, &pci->subsys_id);
        if (status)
            nhi->format = DMI_MGMT_NHI_FORMAT_PCI;
        break;
    }

    case DMI_MGMT_NHI_DEVICE_TYPE_USB_V2: {
        dmi_mgmt_nhi_usb_v2_t *usb = &nhi->usb_v2;

        status =
            dmi_stream_decode(stream, dmi_byte_t, &v2_length) and
            (v2_length >= 0x0D) and
            (v2_length <= length) and
            dmi_stream_decode(stream, dmi_word_t, &usb->vendor_id) and
            dmi_stream_decode(stream, dmi_word_t, &usb->product_id) and
            dmi_stream_decode_str(stream, &usb->serial_number) and
            dmi_stream_decode_bin(stream, DMI_MAC_ADDRESS_LENGTH, &usb->mac_address);
        if (not status)
            break;

        // Device characteristics are present since DSP0270 1.3
        usb->credential_handle = DMI_HANDLE_INVALID;
        if (v2_length >= 0x11) {
            status =
                dmi_stream_decode(stream, dmi_word_t, &usb->characteristics) and
                dmi_stream_decode(stream, dmi_word_t, &usb->credential_handle);
            if (not status)
                break;
        }

        nhi->format = DMI_MGMT_NHI_FORMAT_USB_V2;
        break;
    }

    case DMI_MGMT_NHI_DEVICE_TYPE_PCI_V2: {
        dmi_mgmt_nhi_pci_v2_t *pci = &nhi->pci_v2;
        dmi_byte_t devfn = 0;

        status =
            dmi_stream_decode(stream, dmi_byte_t, &v2_length) and
            (v2_length >= 0x14) and
            (v2_length <= length) and
            dmi_stream_decode(stream, dmi_word_t, &pci->vendor_id) and
            dmi_stream_decode(stream, dmi_word_t, &pci->device_id) and
            dmi_stream_decode(stream, dmi_word_t, &pci->subsys_vendor_id) and
            dmi_stream_decode(stream, dmi_word_t, &pci->subsys_id) and
            dmi_stream_decode_bin(stream, DMI_MAC_ADDRESS_LENGTH, &pci->mac_address) and
            dmi_stream_decode(stream, dmi_word_t, &pci->segment_group) and
            dmi_stream_decode(stream, dmi_byte_t, &pci->bus_number) and
            dmi_stream_decode(stream, dmi_byte_t, &devfn);
        if (not status)
            break;

        pci->device_number   = devfn >> 3;
        pci->function_number = devfn & 0x07;

        // Device characteristics are present since DSP0270 1.3
        pci->credential_handle = DMI_HANDLE_INVALID;
        if (v2_length >= 0x18) {
            status =
                dmi_stream_decode(stream, dmi_word_t, &pci->characteristics) and
                dmi_stream_decode(stream, dmi_word_t, &pci->credential_handle);
            if (not status)
                break;
        }

        nhi->format = DMI_MGMT_NHI_FORMAT_PCI_V2;
        break;
    }

    default: {
        if ((nhi->device_type < DMI_MGMT_NHI_DEVICE_TYPE_OEM_START) or (size < 4))
            break;

        dmi_mgmt_nhi_oem_t *oem = &nhi->oem;
        dmi_dword_t vendor_iana = 0;

        // Vendor IANA code is stored with the most significant byte first
        status =
            dmi_stream_read(stream, &vendor_iana) and
            dmi_stream_decode_bin(stream, size - 4, &oem->vendor_data);
        if (not status)
            break;

        oem->vendor_iana = dmi_ntoh(vendor_iana);
        nhi->format = DMI_MGMT_NHI_FORMAT_OEM;
        break;
    }
    }

    return true;
}

/**
 * @brief Decode Redfish over IP protocol record data.
 *
 * @return `false` on allocation failure, `true` otherwise.
 */
static bool dmi_mgmt_redfish_decode(dmi_entity_t *entity, dmi_mgmt_proto_record_t *record)
{
    dmi_stream_t               *stream  = dmi_entity_stream(entity);
    dmi_mgmt_redfish_over_ip_t *redfish = &record->redfish;

    dmi_byte_t host_ip_assignment   = 0;
    dmi_byte_t host_ip_format       = 0;
    dmi_byte_t service_ip_discovery = 0;
    dmi_byte_t service_ip_format    = 0;
    dmi_byte_t hostname_length      = 0;

    // Hostname is the only variable-length field
    size_t length = record->data.length;
    if (length < 0x5B)
        return true;

    bool status =
        dmi_stream_decode_uuid(stream, &redfish->service_uuid) and
        dmi_stream_decode(stream, dmi_byte_t, &host_ip_assignment) and
        dmi_stream_decode(stream, dmi_byte_t, &host_ip_format) and
        dmi_stream_decode_bin(stream, 16, &redfish->host_ip_address) and
        dmi_stream_decode_bin(stream, 16, &redfish->host_ip_mask) and
        dmi_stream_decode(stream, dmi_byte_t, &service_ip_discovery) and
        dmi_stream_decode(stream, dmi_byte_t, &service_ip_format) and
        dmi_stream_decode_bin(stream, 16, &redfish->service_ip_address) and
        dmi_stream_decode_bin(stream, 16, &redfish->service_ip_mask) and
        dmi_stream_decode(stream, dmi_word_t, &redfish->service_ip_port) and
        dmi_stream_decode(stream, dmi_dword_t, &redfish->service_vlan_id) and
        dmi_stream_decode(stream, dmi_byte_t, &hostname_length) and
        (hostname_length <= length - 0x5B);
    if (not status)
        return true;

    redfish->host_ip_assignment   = dmi_cast(redfish->host_ip_assignment, host_ip_assignment);
    redfish->host_ip_format       = dmi_cast(redfish->host_ip_format, host_ip_format);
    redfish->service_ip_discovery = dmi_cast(redfish->service_ip_discovery, service_ip_discovery);
    redfish->service_ip_format    = dmi_cast(redfish->service_ip_format, service_ip_format);

    // IPv4 addresses take the first 4 bytes of the fields
    if (redfish->host_ip_format == DMI_MGMT_REDFISH_IP_FORMAT_IPV4) {
        redfish->host_ip_address.length = 4;
        redfish->host_ip_mask.length    = 4;
    }
    if (redfish->service_ip_format == DMI_MGMT_REDFISH_IP_FORMAT_IPV4) {
        redfish->service_ip_address.length = 4;
        redfish->service_ip_mask.length    = 4;
    }

    // Hostname is not an SMBIOS string, and may be padded with NULL
    // characters
    const char *hostname = (const char *)record->data.data + 0x5B;
    size_t hostname_size = strnlen(hostname, hostname_length);

    if (hostname_size > 0) {
        redfish->service_hostname = dmi_alloc(dmi_entity_context(entity), hostname_size + 1);
        if (redfish->service_hostname == nullptr)
            return false;

        memcpy(redfish->service_hostname, hostname, hostname_size);
        redfish->service_hostname[hostname_size] = 0;
    }

    record->has_redfish = true;

    return true;
}

static bool dmi_mgmt_controller_decode(dmi_entity_t *entity)
{
    dmi_mgmt_controller_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);
    dmi_stream_t  *stream  = dmi_entity_stream(entity);

    dmi_byte_t if_type        = 0;
    dmi_byte_t if_data_length = 0;

    bool status =
        dmi_stream_decode(stream, dmi_byte_t, &if_type) and
        dmi_stream_decode(stream, dmi_byte_t, &if_data_length);
    if (not status)
        return false;

    info->if_type = dmi_cast(info->if_type, if_type);

    // Some implementations use different structure layout (e.g. the one from
    // SMBIOS versions prior to 3.2), so stop decoding there as dmidecode does
    if (not dmi_stream_has(stream, if_data_length))
        return dmi_entity_incomplete(entity);

    // Interface data is read as a whole, and then read again by its type, so
    // the cursor ends up past it either way
    dmi_stream_mark_t if_data_start = dmi_stream_mark(stream);

    if (not dmi_stream_decode_bin(stream, if_data_length, &info->if_data))
        return false;

    if ((info->if_type == DMI_MGMT_IF_TYPE_NETWORK_HOST_IF) and (if_data_length > 0)) {
        dmi_stream_rewind(stream, if_data_start);

        if (not dmi_mgmt_nhi_decode(entity, &info->nhi, if_data_length))
            return false;

        info->has_nhi = true;

        dmi_stream_skip_ex(stream, if_data_start, if_data_length);
    }

    // Protocol records are present since SMBIOS 3.2
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    dmi_byte_t proto_records_count = 0;
    if (not dmi_stream_decode(stream, dmi_byte_t, &proto_records_count))
        return dmi_entity_incomplete(entity);

    entity->level = dmi_version(3, 2, 0);

    if (proto_records_count == 0)
        return true;

    info->proto_records = dmi_alloc_array(context, sizeof(*info->proto_records),
                                          proto_records_count);
    if (info->proto_records == nullptr)
        return false;

    // Records count is incremented only for completely decoded records
    for (size_t i = 0; i < proto_records_count; i++) {
        dmi_byte_t type   = 0;
        dmi_byte_t length = 0;

        status =
            dmi_stream_decode(stream, dmi_byte_t, &type) and
            dmi_stream_decode(stream, dmi_byte_t, &length) and
            dmi_stream_has(stream, length);
        if (not status)
            return dmi_entity_incomplete(entity);

        dmi_mgmt_proto_record_t *record = &info->proto_records[i];
        dmi_stream_mark_t data_start = dmi_stream_mark(stream);

        record->type = dmi_cast(record->type, type);
        if (not dmi_stream_decode_bin(stream, length, &record->data))
            return false;

        if (record->type == DMI_MGMT_PROTO_REDFISH_OVER_IP) {
            dmi_stream_rewind(stream, data_start);

            if (not dmi_mgmt_redfish_decode(entity, record))
                return false;
        }

        // Record may be longer than the fields it is known to hold, so the
        // next one is found by the length rather than by counting
        dmi_stream_skip_ex(stream, data_start, length);

        info->proto_records_count++;
    }

    return true;
}

static void dmi_mgmt_controller_cleanup(dmi_entity_t *entity)
{
    dmi_mgmt_controller_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    if (info == nullptr)
        return;

    dmi_free(info->nhi.usb.serial_number);

    for (size_t i = 0; i < info->proto_records_count; i++)
        dmi_free(info->proto_records[i].redfish.service_hostname);

    dmi_free(info->proto_records);
}

//
// Records follow each other, each one carrying its own length, so they all
// fit the structure only if the lengths agree with it.
//
static void dmi_mgmt_controller_lint_records(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_stream_t stream;
    dmi_byte_t length;

    if (not dmi_stream_initialize(&stream, entity))
        return;

    if (not dmi_stream_read_data_at(&stream, &length, DMI_MGMT_CONTROLLER_IF_LENGTH_OFFSET,
                                    sizeof(length)))
        return;

    // Number of the records follows the interface-specific data
    size_t offset = DMI_MGMT_CONTROLLER_IF_LENGTH_OFFSET + sizeof(length) + length;
    dmi_byte_t count;

    if (not dmi_stream_read_data_at(&stream, &count, offset, sizeof(count))) {
        dmi_lint_issue(lint, entity, "if-data", dmi_lint_entity_offset(lint, entity) +
                       DMI_MGMT_CONTROLLER_IF_LENGTH_OFFSET,
                       "interface data of %u bytes leaves no room for the protocol records",
                       (unsigned)length);
        return;
    }

    offset += sizeof(count);

    for (unsigned i = 0; i < count; i++) {
        dmi_byte_t record[2];

        // Every record starts with its type and the length of its data
        if (dmi_stream_read_data_at(&stream, record, offset, sizeof(record))) {
            offset += sizeof(record) + record[1];

            if (offset <= entity->body_length)
                continue;
        }

        dmi_lint_issue(lint, entity, "protocol-records", dmi_lint_entity_offset(lint, entity),
                       "record %u of %u does not fit the structure of %zu bytes",
                       i + 1, (unsigned)count, entity->body_length);
        return;
    }
}
