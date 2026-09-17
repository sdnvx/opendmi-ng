//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/pointing-device.h>

static const dmi_name_set_t dmi_pointing_device_type_names =
{
    .code  = "pointing-device-types",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_POINTING_DEVICE_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_POINTING_DEVICE_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_POINTING_DEVICE_TYPE_UNKNOWN),
        {
            .id   = DMI_POINTING_DEVICE_TYPE_MOUSE,
            .code = "mouse",
            .name = "Mouse"
        },
        {
            .id   = DMI_POINTING_DEVICE_TYPE_TRACK_BALL,
            .code = "track-ball",
            .name = "Track ball"
        },
        {
            .id   = DMI_POINTING_DEVICE_TYPE_TRACK_POINT,
            .code = "track-point",
            .name = "Track point"
        },
        {
            .id   = DMI_POINTING_DEVICE_TYPE_GLIDE_POINT,
            .code = "glide-point",
            .name = "Glide point"
        },
        {
            .id   = DMI_POINTING_DEVICE_TYPE_TOUCH_PAD,
            .code = "touch-pad",
            .name = "Touch pad"
        },
        {
            .id   = DMI_POINTING_DEVICE_TYPE_TOUCH_SCREEN,
            .code = "touch-screen",
            .name = "Touch screen"
        },
        {
            .id   = DMI_POINTING_DEVICE_TYPE_OPTICAL_SENSOR,
            .code = "optical-sensor",
            .name = "Optical sensor"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_pointing_device_iface_names =
{
    .code  = "pointing-device-interfaces",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_POINTING_DEVICE_IFACE_UNSPEC),
        DMI_NAME_OTHER(DMI_POINTING_DEVICE_IFACE_OTHER),
        DMI_NAME_UNKNOWN(DMI_POINTING_DEVICE_IFACE_UNKNOWN),
        {
            .id   = DMI_POINTING_DEVICE_IFACE_SERIAL,
            .code = "serial",
            .name = "Serial"
        },
        {
            .id   = DMI_POINTING_DEVICE_IFACE_PS2,
            .code = "ps2",
            .name = "PS/2"
        },
        {
            .id   = DMI_POINTING_DEVICE_IFACE_INFRARED,
            .code = "infrared",
            .name = "Infrared"
        },
        {
            .id   = DMI_POINTING_DEVICE_IFACE_HP_HIL,
            .code = "hp-hil",
            .name = "HP-HIL"
        },
        {
            .id   = DMI_POINTING_DEVICE_IFACE_BUS_MOUSE,
            .code = "bus-mouse",
            .name = "Bus mouse"
        },
        {
            .id   = DMI_POINTING_DEVICE_IFACE_ADB,
            .code = "adb",
            .name = "ADB (Apple Desktop Bus)"
        },
        {
            .id   = DMI_POINTING_DEVICE_IFACE_BUS_MOUSE_DB9,
            .code = "bus-mouse-db9",
            .name = "Bus mouse DB-9"
        },
        {
            .id   = DMI_POINTING_DEVICE_IFACE_BUS_MOUSE_DIN,
            .code = "bus-mouse-din",
            .name = "Bus mouse micro-DIN"
        },
        {
            .id   = DMI_POINTING_DEVICE_IFACE_USB,
            .code = "usb",
            .name = "USB"
        },
        {
            .id   = DMI_POINTING_DEVICE_IFACE_I2C,
            .code = "i2c",
            .name = "I2C"
        },
        {
            .id   = DMI_POINTING_DEVICE_IFACE_SPI,
            .code = "spi",
            .name = "SPI"
        },
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_pointing_device_spec =
{
    .code            = "pointing-device",
    .name            = "Built-in pointing device",
    .description     = (const char *[]){
        "This structure describes the attributes of the built-in pointing "
        "device for the system. The presence of this structure does not imply "
        "that the built-in pointing device is active for the system\'s use.",
        //
        nullptr
    },
    .type            = DMI_TYPE(POINTING_DEVICE),
    .minimum_version = DMI_VERSION(2, 1, 0),
    .minimum_length  = 0x07,
    .decoded_length  = sizeof(dmi_pointing_device_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_pointing_device_t, type, ENUM, {
            .code    = "type",
            .name    = "Type",
            .unspec  = dmi_value_ptr(DMI_POINTING_DEVICE_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_POINTING_DEVICE_TYPE_UNKNOWN),
            .values  = &dmi_pointing_device_type_names
        }),
        DMI_ATTRIBUTE(dmi_pointing_device_t, interface, ENUM, {
            .code    = "interface",
            .name    = "Interface type",
            .unspec  = dmi_value_ptr(DMI_POINTING_DEVICE_IFACE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_POINTING_DEVICE_IFACE_UNKNOWN),
            .values  = &dmi_pointing_device_iface_names
        }),
        DMI_ATTRIBUTE(dmi_pointing_device_t, button_count, INTEGER, {
            .code    = "button-count",
            .name    = "Button count"
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_pointing_device_decode
    }
};

const char *dmi_pointing_device_type_name(dmi_pointing_device_type_t value)
{
    return dmi_name_lookup(&dmi_pointing_device_type_names, (int)value);
}

const char *dmi_pointing_device_iface_name(dmi_pointing_device_iface_t value)
{
    return dmi_name_lookup(&dmi_pointing_device_iface_names, (int)value);
}

bool dmi_pointing_device_decode(dmi_entity_t *entity)
{
    dmi_pointing_device_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(POINTING_DEVICE));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    return
        dmi_stream_decode(stream, dmi_byte_t, &info->type) and
        dmi_stream_decode(stream, dmi_byte_t, &info->interface) and
        dmi_stream_decode(stream, dmi_byte_t, &info->button_count);
}
