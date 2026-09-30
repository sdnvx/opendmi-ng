//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include <opendmi/entity/hpe/usb-port-internal.h>

const dmi_entity_spec_t dmi_hpe_usb_port_spec =
{
    .type        = DMI_TYPE(hpe_usb_port),
    .code        = "hpe-usb-port",
    .name        = "HP/HPE USB port connector correlation record",
    .description = (const char *[]){
        "Correlates a USB port connector (type 8) with its USB controller, "
        "its location and the UEFI device path of its endpoint.",
        //
        nullptr
    },
    .params = {
        .generations    = { .minimum = DMI_HPE_GEN9 },
        .minimum_length = 0x0F,
        .decoded_length = sizeof(dmi_hpe_usb_port_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_hpe_usb_port_t, port_handle,  dmi_word_t),
        DMI_FIELD(dmi_hpe_usb_port_t, bus,          dmi_byte_t),
        DMI_FIELD(dmi_hpe_usb_port_t, devfn,        dmi_byte_t),
        DMI_FIELD(dmi_hpe_usb_port_t, location,     dmi_byte_t),
        DMI_FIELD(dmi_hpe_usb_port_t, sharing,      dmi_word_t),
        DMI_FIELD(dmi_hpe_usb_port_t, instance,     dmi_byte_t),
        DMI_FIELD(dmi_hpe_usb_port_t, hub_instance, dmi_byte_t),
        DMI_FIELD(dmi_hpe_usb_port_t, speed,        dmi_byte_t),
        DMI_FIELD_STRING(dmi_hpe_usb_port_t, uefi_device_path),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_hpe_usb_port_t, segment, dmi_word_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_usb_port_t, port_handle, HANDLE, {
            .code    = "port-handle",
            .name    = "Port connector handle",
            .targets = dmi_types(DMI_TYPE(port_connector))
        }),
        DMI_ATTRIBUTE(dmi_hpe_usb_port_t, segment, INTEGER, {
            .code  = "segment",
            .name  = "PCI segment group",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_usb_port_t, bus, INTEGER, {
            .code  = "bus",
            .name  = "PCI bus",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_usb_port_t, devfn, INTEGER, {
            .code  = "devfn",
            .name  = "PCI device and function",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_usb_port_t, location, ENUM, {
            .code   = "location",
            .name   = "Location",
            .values = &dmi_hpe_usb_location_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_usb_port_t, sharing, ENUM, {
            .code   = "sharing",
            .name   = "Management port",
            .values = &dmi_hpe_usb_sharing_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_usb_port_t, instance, INTEGER, {
            .code = "instance",
            .name = "Port instance"
        }),
        DMI_ATTRIBUTE(dmi_hpe_usb_port_t, hub_instance, INTEGER, {
            .code   = "hub-instance",
            .name   = "Parent hub port instance",
            .unspec = dmi_value_ptr((uint8_t)0xFE)
        }),
        DMI_ATTRIBUTE(dmi_hpe_usb_port_t, speed, ENUM, {
            .code   = "speed",
            .name   = "Port speed capability",
            .values = &dmi_hpe_usb_speed_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_usb_port_t, uefi_device_path, STRING, {
            .code = "uefi-device-path",
            .name = "UEFI device path"
        }),
        {}
    })
};

const dmi_name_set_t dmi_hpe_usb_location_names =
{
    .code  = "hpe-usb-location",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_USB_LOCATION_INTERNAL,
            .code = "internal",
            .name = "Internal"
        },
        {
            .id   = DMI_HPE_USB_LOCATION_FRONT,
            .code = "front",
            .name = "Front of the server"
        },
        {
            .id   = DMI_HPE_USB_LOCATION_REAR,
            .code = "rear",
            .name = "Rear of the server"
        },
        {
            .id   = DMI_HPE_USB_LOCATION_SD_CARD,
            .code = "sd-card",
            .name = "Embedded internal SD card"
        },
        {
            .id   = DMI_HPE_USB_LOCATION_ILO,
            .code = "ilo",
            .name = "iLO USB"
        },
        {
            .id   = DMI_HPE_USB_LOCATION_NAND_HUB,
            .code = "nand-hub",
            .name = "USB hub of the NAND controller"
        },
        {
            .id   = DMI_HPE_USB_LOCATION_DEBUG,
            .code = "debug",
            .name = "Debug port"
        },
        {
            .id   = DMI_HPE_USB_LOCATION_OCP,
            .code = "ocp",
            .name = "OCP USB"
        },
        {}
    })
};

const char *dmi_hpe_usb_location_name(dmi_hpe_usb_location_t value)
{
    return dmi_name_lookup(&dmi_hpe_usb_location_names, (int)value);
}

const dmi_name_set_t dmi_hpe_usb_sharing_names =
{
    .code  = "hpe-usb-sharing",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_USB_SHARING_NONE,
            .code = "none",
            .name = "Not shared"
        },
        {
            .id   = DMI_HPE_USB_SHARING_SWITCH,
            .code = "switch",
            .name = "Shared with a physical switch"
        },
        {
            .id   = DMI_HPE_USB_SHARING_AUTOMATIC,
            .code = "automatic",
            .name = "Shared with automatic control"
        },
        {}
    })
};

const char *dmi_hpe_usb_sharing_name(dmi_hpe_usb_sharing_t value)
{
    return dmi_name_lookup(&dmi_hpe_usb_sharing_names, (int)value);
}

const dmi_name_set_t dmi_hpe_usb_speed_names =
{
    .code  = "hpe-usb-speed",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_USB_SPEED_FULL,
            .code = "full",
            .name = "USB 1.1 full speed"
        },
        {
            .id   = DMI_HPE_USB_SPEED_HIGH,
            .code = "high",
            .name = "USB 2.0 high speed"
        },
        {
            .id   = DMI_HPE_USB_SPEED_SUPER,
            .code = "super",
            .name = "USB 3.0 super speed"
        },
        {}
    })
};

const char *dmi_hpe_usb_speed_name(dmi_hpe_usb_speed_t value)
{
    return dmi_name_lookup(&dmi_hpe_usb_speed_names, (int)value);
}
