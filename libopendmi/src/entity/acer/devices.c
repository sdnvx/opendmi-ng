//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/acer.h>

#include <opendmi/entity/acer/devices.h>

static void dmi_acer_devices_cleanup(dmi_entity_t *entity);

// Kinds are reverse engineered from the PCI and USB IDs of the devices of the
// data corpus: the ones given to the devices of a single class are named by
// it, and the other ones found in it by their values
static const dmi_name_set_t dmi_acer_device_kind_names =
{
    .code  = "acer-device-kind",
    .names = DMI_NAMES({
        {
            .id   = DMI_ACER_DEVICE_KIND_UNKNOWN_1,
            .code = "unknown-1",
            .name = "Unknown 1"
        },
        {
            .id   = DMI_ACER_DEVICE_KIND_UNKNOWN_2,
            .code = "unknown-2",
            .name = "Unknown 2"
        },
        {
            .id   = DMI_ACER_DEVICE_KIND_UNKNOWN_3,
            .code = "unknown-3",
            .name = "Unknown 3"
        },
        {
            .id   = DMI_ACER_DEVICE_KIND_CAMERA,
            .code = "camera",
            .name = "Webcam"
        },
        {
            .id   = DMI_ACER_DEVICE_KIND_AUDIO,
            .code = "audio",
            .name = "Audio"
        },
        {
            .id   = DMI_ACER_DEVICE_KIND_WLAN,
            .code = "wlan",
            .name = "Wireless network adapter"
        },
        {
            .id   = DMI_ACER_DEVICE_KIND_BLUETOOTH,
            .code = "bluetooth",
            .name = "Bluetooth adapter"
        },
        {
            .id   = DMI_ACER_DEVICE_KIND_UNKNOWN_13,
            .code = "unknown-13",
            .name = "Unknown 13"
        },
        {
            .id   = DMI_ACER_DEVICE_KIND_UNKNOWN_17,
            .code = "unknown-17",
            .name = "Unknown 17"
        },
        {
            .id   = DMI_ACER_DEVICE_KIND_UNKNOWN_19,
            .code = "unknown-19",
            .name = "Unknown 19"
        },
        {
            .id   = DMI_ACER_DEVICE_KIND_UNKNOWN_21,
            .code = "unknown-21",
            .name = "Unknown 21"
        },
        {
            .id   = DMI_ACER_DEVICE_KIND_UNKNOWN_22,
            .code = "unknown-22",
            .name = "Unknown 22"
        },
        {
            .id   = DMI_ACER_DEVICE_KIND_UNKNOWN_25,
            .code = "unknown-25",
            .name = "Unknown 25"
        },
        {}
    })
};

const dmi_entity_spec_t dmi_acer_devices_spec =
{
    .type        = DMI_TYPE(acer_devices),
    .code        = "acer-devices",
    .name        = "Acer device list",
    .description = (const char *[]){
        "Lists the devices of a laptop by their vendor and device IDs.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x04,
        .decoded_length = sizeof(dmi_acer_devices_t)
    },

    // Devices run to the end of the structure, which carries no number of
    // them of its own
    .fields = DMI_FIELDS({
        DMI_FIELD_ARRAY(dmi_acer_devices_t, devices, device_count,
            .stride = 5,
            .fields = DMI_FIELDS({
                DMI_FIELD(dmi_acer_device_t, kind,      dmi_byte_t),
                DMI_FIELD(dmi_acer_device_t, vendor_id, dmi_word_t),
                DMI_FIELD(dmi_acer_device_t, device_id, dmi_word_t),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE_ARRAY(dmi_acer_devices_t, devices, device_count, STRUCT, {
            .code  = "devices",
            .name  = "Devices",
            .attrs = DMI_ATTRIBUTES({
                DMI_ATTRIBUTE(dmi_acer_device_t, kind, ENUM, {
                    .code   = "kind",
                    .name   = "Kind",
                    .values = &dmi_acer_device_kind_names,
                    .flags  = DMI_ATTRIBUTE_FLAG_OPEN
                }),
                DMI_ATTRIBUTE(dmi_acer_device_t, vendor_id, INTEGER, {
                    .code   = "vendor-id",
                    .name   = "Vendor ID",
                    .unspec = dmi_value_ptr((uint16_t)0),
                    .flags  = DMI_ATTRIBUTE_FLAG_HEX
                }),
                DMI_ATTRIBUTE(dmi_acer_device_t, device_id, INTEGER, {
                    .code   = "device-id",
                    .name   = "Device ID",
                    .unspec = dmi_value_ptr((uint16_t)0),
                    .flags  = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        {}
    }),

    .handlers = {
        .cleanup = dmi_acer_devices_cleanup
    }
};

const char *dmi_acer_device_kind_name(dmi_acer_device_kind_t value)
{
    return dmi_name_lookup(&dmi_acer_device_kind_names, (int)value);
}

static void dmi_acer_devices_cleanup(dmi_entity_t *entity)
{
    dmi_acer_devices_t *info = dmi_entity_info(entity, DMI_TYPE(acer_devices));
    if (info == nullptr)
        return;

    dmi_free(info->devices);
}
