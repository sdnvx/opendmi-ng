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
                DMI_ATTRIBUTE(dmi_acer_device_t, kind, INTEGER, {
                    .code  = "kind",
                    .name  = "Kind",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
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

static void dmi_acer_devices_cleanup(dmi_entity_t *entity)
{
    dmi_acer_devices_t *info = dmi_entity_info(entity, DMI_TYPE(acer_devices));
    if (info == nullptr)
        return;

    dmi_free(info->devices);
}
