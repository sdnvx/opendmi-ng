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

#include <opendmi/entity/hpe/usb-device-internal.h>

const dmi_entity_spec_t dmi_hpe_usb_device_spec =
{
    .type        = DMI_TYPE(HPE_USB_DEVICE),
    .code        = "hpe-usb-device",
    .name        = "HP/HPE USB device correlation record",
    .description = (const char *[]){
        "Describes a USB device the firmware has found at boot on a USB "
        "port, and the UEFI boot entries naming it.",
        //
        nullptr
    },
    .params = {
        .generations    = { .minimum = DMI_HPE_GEN9 },
        .minimum_length = 0x17,
        .decoded_length = sizeof(dmi_hpe_usb_device_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_hpe_usb_device_t, port_handle, dmi_word_t),
        DMI_FIELD(dmi_hpe_usb_device_t, vendor_id,   dmi_word_t),

        DMI_FIELD_BITS(dmi_hpe_usb_device_t, is_sd_card_present, 1),
        DMI_FIELD_PAD(dmi_word_t),

        DMI_FIELD(dmi_hpe_usb_device_t, usb_class,    dmi_byte_t),
        DMI_FIELD(dmi_hpe_usb_device_t, usb_subclass, dmi_byte_t),
        DMI_FIELD(dmi_hpe_usb_device_t, usb_protocol, dmi_byte_t),
        DMI_FIELD(dmi_hpe_usb_device_t, product_id,   dmi_word_t),
        DMI_FIELD(dmi_hpe_usb_device_t, raw_capacity, dmi_dword_t),
        DMI_FIELD_STRING(dmi_hpe_usb_device_t, uefi_device_path),
        DMI_FIELD_STRING(dmi_hpe_usb_device_t, uefi_device_name),
        DMI_FIELD_STRING(dmi_hpe_usb_device_t, device_name),
        DMI_FIELD_STRING(dmi_hpe_usb_device_t, location),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_usb_device_t, port_handle, HANDLE, {
            .code    = "port-handle",
            .name    = "USB port handle",
            .targets = dmi_types(DMI_TYPE(HPE_USB_PORT))
        }),
        DMI_ATTRIBUTE(dmi_hpe_usb_device_t, vendor_id, INTEGER, {
            .code  = "vendor-id",
            .name  = "USB vendor ID",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_usb_device_t, product_id, INTEGER, {
            .code  = "product-id",
            .name  = "USB product ID",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_usb_device_t, is_sd_card_present, BOOL, {
            .code = "is-sd-card-present",
            .name = "Embedded SD card present"
        }),
        DMI_ATTRIBUTE(dmi_hpe_usb_device_t, usb_class, INTEGER, {
            .code  = "usb-class",
            .name  = "USB class",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_usb_device_t, usb_subclass, INTEGER, {
            .code  = "usb-subclass",
            .name  = "USB subclass",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_usb_device_t, usb_protocol, INTEGER, {
            .code  = "usb-protocol",
            .name  = "USB protocol",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_usb_device_t, capacity, SIZE, {
            .code   = "capacity",
            .name   = "USB capacity",
            .unspec = dmi_value_ptr((uint64_t)0)
        }),
        DMI_ATTRIBUTE(dmi_hpe_usb_device_t, uefi_device_path, STRING, {
            .code = "uefi-device-path",
            .name = "UEFI device path"
        }),
        DMI_ATTRIBUTE(dmi_hpe_usb_device_t, uefi_device_name, STRING, {
            .code = "uefi-device-name",
            .name = "UEFI device name"
        }),
        DMI_ATTRIBUTE(dmi_hpe_usb_device_t, device_name, STRING, {
            .code = "device-name",
            .name = "Device name"
        }),
        DMI_ATTRIBUTE(dmi_hpe_usb_device_t, location, STRING, {
            .code = "location",
            .name = "Device location"
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_hpe_usb_device_derive
    }
};

bool dmi_hpe_usb_device_derive(dmi_entity_t *entity)
{
    dmi_hpe_usb_device_t *info = dmi_entity_info(entity, DMI_TYPE(HPE_USB_DEVICE));
    if (info == nullptr)
        return false;

    info->capacity = (uint64_t)info->raw_capacity * 1024 * 1024;

    return true;
}
