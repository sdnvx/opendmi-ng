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

#include <opendmi/entity/pointing-device-internal.h>

const dmi_name_set_t dmi_pointing_device_type_names =
{
    .code  = "pointing-device-type",
    .names = DMI_NAMES({
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
        {}
    })
};

const dmi_name_set_t dmi_pointing_device_iface_names =
{
    .code  = "pointing-device-interface",
    .names = DMI_NAMES({
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
        {}
    })
};

const char *dmi_pointing_device_type_name(dmi_pointing_device_type_t value)
{
    return dmi_name_lookup(&dmi_pointing_device_type_names, (int)value);
}

const char *dmi_pointing_device_iface_name(dmi_pointing_device_iface_t value)
{
    return dmi_name_lookup(&dmi_pointing_device_iface_names, (int)value);
}
