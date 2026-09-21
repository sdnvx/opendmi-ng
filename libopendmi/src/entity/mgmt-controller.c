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
    {}
};

static const dmi_attribute_t dmi_mgmt_nhi_pci_attrs[] =
{
    DMI_MGMT_NHI_PCI_ATTRS(dmi_mgmt_nhi_pci_t),
    {}
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
    {}
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
    {}
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
    {}
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
        .variants = DMI_VARIANTS({
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
            {}
        })
    }),
    {}
};

// IP addresses are formatted as such only if their format is known
#define DMI_MGMT_REDFISH_IP_ATTR(__member, __format, __code, __name)          \
    DMI_ATTRIBUTE_VARIANT(dmi_mgmt_redfish_over_ip_t, __format, {             \
        .code     = __code,                                                   \
        .name     = __name,                                                   \
        .variants = DMI_VARIANTS({                                            \
            DMI_VARIANT(DMI_MGMT_REDFISH_IP_FORMAT_IPV4,                      \
                        dmi_mgmt_redfish_over_ip_t, __member, BINARY, {       \
                .flags = DMI_ATTRIBUTE_FLAG_IP                                \
            }),                                                               \
            DMI_VARIANT(DMI_MGMT_REDFISH_IP_FORMAT_IPV6,                      \
                        dmi_mgmt_redfish_over_ip_t, __member, BINARY, {       \
                .flags = DMI_ATTRIBUTE_FLAG_IP                                \
            }),                                                               \
            DMI_VARIANT_DEFAULT(dmi_mgmt_redfish_over_ip_t, __member, BINARY, {}), \
            {}                                                                \
        })                                                                    \
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
    {}
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
        .variants = DMI_VARIANTS({
            DMI_VARIANT(true, dmi_mgmt_proto_record_t, redfish, STRUCT, {
                .attrs = dmi_mgmt_redfish_over_ip_attrs
            }),
            DMI_VARIANT_DEFAULT(dmi_mgmt_proto_record_t, data, BINARY, {}),
            {}
        })
    }),
    {}
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
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x06,
        .decoded_length  = sizeof(dmi_mgmt_controller_t)
    },

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_mgmt_controller_t, if_type, ENUM, {
            .code    = "if-type",
            .name    = "Interface type",
            .values  = &dmi_mgmt_if_type_names
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_mgmt_controller_t, has_nhi, {
            .code     = "if-data",
            .name     = "Interface-specific data",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_mgmt_controller_t, nhi, STRUCT, {
                    .attrs = dmi_mgmt_nhi_attrs
                }),
                DMI_VARIANT_DEFAULT(dmi_mgmt_controller_t, if_data, BINARY, {}),
                {}
            })
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_mgmt_controller_t, proto_records, proto_records_count, STRUCT, {
            .code    = "protocol-records",
            .name    = "Protocol records",
            .attrs   = dmi_mgmt_proto_record_attrs,
            .level   = DMI_VERSION(3, 2, 0)
        }),
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("mgmt-controller-host-if.records", dmi_mgmt_controller_lint_records, {
            .name              = "Protocol records fit the structure holding them",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    }),

    .handlers = {
        .decode  = dmi_mgmt_controller_decode,
        .cleanup = dmi_mgmt_controller_cleanup
    }
};
