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
    .type        = DMI_TYPE(hpe_usb_device),
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
            .targets = dmi_types(DMI_TYPE(hpe_usb_port))
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
        // Subclass and protocol codes are named for mass storage devices and
        // hubs, and shown as they are for the other classes
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_usb_device_t, usb_class, {
            .code     = "usb-subclass",
            .name     = "USB subclass",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(DMI_HPE_USB_CLASS_STORAGE, dmi_hpe_usb_device_t, usb_subclass, ENUM, {
                    .code   = "usb-subclass",
                    .name   = "USB subclass",
                    .values = &dmi_hpe_usb_storage_subclass_names
                }),
                DMI_VARIANT_DEFAULT(dmi_hpe_usb_device_t, usb_subclass, INTEGER, {
                    .code  = "usb-subclass",
                    .name  = "USB subclass",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_usb_device_t, usb_class, {
            .code     = "usb-protocol",
            .name     = "USB protocol",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(DMI_HPE_USB_CLASS_STORAGE, dmi_hpe_usb_device_t, usb_protocol, ENUM, {
                    .code   = "usb-protocol",
                    .name   = "USB protocol",
                    .values = &dmi_hpe_usb_storage_proto_names
                }),
                DMI_VARIANT(DMI_HPE_USB_CLASS_HUB, dmi_hpe_usb_device_t, usb_protocol, ENUM, {
                    .code   = "usb-protocol",
                    .name   = "USB protocol",
                    .values = &dmi_hpe_usb_hub_proto_names
                }),
                DMI_VARIANT_DEFAULT(dmi_hpe_usb_device_t, usb_protocol, INTEGER, {
                    .code  = "usb-protocol",
                    .name  = "USB protocol",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
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
    dmi_hpe_usb_device_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_usb_device));
    if (info == nullptr)
        return false;

    info->capacity = (uint64_t)info->raw_capacity * 1024 * 1024;

    return true;
}

const dmi_name_set_t dmi_hpe_usb_storage_subclass_names =
{
    .code  = "hpe-usb-storage-subclass",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_USB_STORAGE_SUBCLASS_UNREPORTED,
            .code = "unreported",
            .name = "SCSI command set not reported"
        },
        {
            .id   = DMI_HPE_USB_STORAGE_SUBCLASS_RBC,
            .code = "rbc",
            .name = "RBC"
        },
        {
            .id   = DMI_HPE_USB_STORAGE_SUBCLASS_ATAPI,
            .code = "atapi",
            .name = "ATAPI"
        },
        {
            .id   = DMI_HPE_USB_STORAGE_SUBCLASS_QIC_157,
            .code = "qic-157",
            .name = "QIC-157 (obsolete)"
        },
        {
            .id   = DMI_HPE_USB_STORAGE_SUBCLASS_UFI,
            .code = "ufi",
            .name = "UFI"
        },
        {
            .id   = DMI_HPE_USB_STORAGE_SUBCLASS_SFF_8070I,
            .code = "sff-8070i",
            .name = "SFF-8070i (obsolete)"
        },
        {
            .id   = DMI_HPE_USB_STORAGE_SUBCLASS_SCSI,
            .code = "scsi",
            .name = "SCSI transparent command set"
        },
        {
            .id   = DMI_HPE_USB_STORAGE_SUBCLASS_LSD_FS,
            .code = "lsd-fs",
            .name = "LSD FS"
        },
        {
            .id   = DMI_HPE_USB_STORAGE_SUBCLASS_IEEE_1667,
            .code = "ieee-1667",
            .name = "IEEE 1667"
        },
        {
            .id   = DMI_HPE_USB_STORAGE_SUBCLASS_VENDOR,
            .code = "vendor-specific",
            .name = "Vendor-specific"
        },
        {}
    }),
    .ranges = DMI_NAME_RANGES({
        {
            .start_id = DMI_HPE_USB_STORAGE_SUBCLASS_IEEE_1667 + 1,
            .end_id   = DMI_HPE_USB_STORAGE_SUBCLASS_VENDOR - 1,
            .code     = "reserved",
            .name     = "Reserved"
        },
        {}
    })
};

const dmi_name_set_t dmi_hpe_usb_storage_proto_names =
{
    .code  = "hpe-usb-storage-protocol",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_USB_STORAGE_PROTO_CBI_INT,
            .code = "cbi-interrupt",
            .name = "CBI with command completion interrupt"
        },
        {
            .id   = DMI_HPE_USB_STORAGE_PROTO_CBI,
            .code = "cbi",
            .name = "CBI without command completion interrupt"
        },
        {
            .id   = DMI_HPE_USB_STORAGE_PROTO_OBSOLETE,
            .code = "obsolete",
            .name = "Obsolete"
        },
        {
            .id   = DMI_HPE_USB_STORAGE_PROTO_BOT,
            .code = "bulk-only",
            .name = "Bulk-only transport"
        },
        {
            .id   = DMI_HPE_USB_STORAGE_PROTO_UAS,
            .code = "uas",
            .name = "USB attached SCSI"
        },
        {
            .id   = DMI_HPE_USB_STORAGE_PROTO_VENDOR,
            .code = "vendor-specific",
            .name = "Vendor-specific"
        },
        {}
    })
};

const dmi_name_set_t dmi_hpe_usb_hub_proto_names =
{
    .code  = "hpe-usb-hub-protocol",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_USB_HUB_PROTO_FULL_SPEED,
            .code = "full-speed",
            .name = "Full speed"
        },
        {
            .id   = DMI_HPE_USB_HUB_PROTO_SINGLE_TT,
            .code = "single-tt",
            .name = "Hi-speed with a single transaction translator"
        },
        {
            .id   = DMI_HPE_USB_HUB_PROTO_MULTI_TT,
            .code = "multi-tt",
            .name = "Hi-speed with multiple transaction translators"
        },
        {}
    })
};

const char *dmi_hpe_usb_storage_subclass_name(dmi_hpe_usb_storage_subclass_t value)
{
    return dmi_name_lookup(&dmi_hpe_usb_storage_subclass_names, (int)value);
}

const char *dmi_hpe_usb_storage_proto_name(dmi_hpe_usb_storage_proto_t value)
{
    return dmi_name_lookup(&dmi_hpe_usb_storage_proto_names, (int)value);
}

const char *dmi_hpe_usb_hub_proto_name(dmi_hpe_usb_hub_proto_t value)
{
    return dmi_name_lookup(&dmi_hpe_usb_hub_proto_names, (int)value);
}
