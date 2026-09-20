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

#include <opendmi/entity/hardware-security.h>

static bool dmi_hardware_security_decode(dmi_entity_t *entity);

static const dmi_name_set_t dmi_hardware_security_status_names =
{
    .code  = "hardware-security-status",
    .names = (dmi_name_t[]){
        {
            .id   = DMI_HARDWARE_SECURITY_STATUS_DISABLED,
            .code = "disabled",
            .name = "Disabled"
        },
        {
            .id   = DMI_HARDWARE_SECURITY_STATUS_ENABLED,
            .code = "enabled",
            .name = "Enabled"
        },
        {
            .id   = DMI_HARDWARE_SECURITY_STATUS_NOT_IMPLEMENTED,
            .code = "not-implemented",
            .name = "Not implemented"
        },
        DMI_NAME_UNKNOWN(DMI_HARDWARE_SECURITY_STATUS_UNKNOWN),
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_hardware_security_spec =
{
    .code            = "hardware-security",
    .name            = "Hardware security",
    .description     = (const char *[]){
        "This structure describes the system-wide hardware security settings.",
        //
        nullptr
    },
    .type            = DMI_TYPE(HARDWARE_SECURITY),
    .minimum_version = DMI_VERSION(2, 2, 0),
    .minimum_length  = 0x05,
    .decoded_length  = sizeof(dmi_hardware_security_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_hardware_security_t, front_panel_reset, ENUM, {
            .code    = "front-panel-reset",
            .name    = "Front panel reset status",
            .values  = &dmi_hardware_security_status_names,
            .unknown = dmi_value_ptr(DMI_HARDWARE_SECURITY_STATUS_UNKNOWN)
        }),
        DMI_ATTRIBUTE(dmi_hardware_security_t, admin_password, ENUM, {
            .code    = "admin-password",
            .name    = "Administrator password status",
            .values  = &dmi_hardware_security_status_names,
            .unknown = dmi_value_ptr(DMI_HARDWARE_SECURITY_STATUS_UNKNOWN)
        }),
        DMI_ATTRIBUTE(dmi_hardware_security_t, keyboard_password, ENUM, {
            .code    = "keyboard-password",
            .name    = "Keyboard password status",
            .values  = &dmi_hardware_security_status_names,
            .unknown = dmi_value_ptr(DMI_HARDWARE_SECURITY_STATUS_UNKNOWN)
        }),
        DMI_ATTRIBUTE(dmi_hardware_security_t, poweron_password, ENUM, {
            .code    = "poweron-password",
            .name    = "Power-on password status",
            .values  = &dmi_hardware_security_status_names,
            .unknown = dmi_value_ptr(DMI_HARDWARE_SECURITY_STATUS_UNKNOWN)
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_hardware_security_decode
    }
};

const char *dmi_hardware_security_status_name(dmi_hardware_security_status_t value)
{
    return dmi_name_lookup(&dmi_hardware_security_status_names, (int)value);
}

static bool dmi_hardware_security_decode(dmi_entity_t *entity)
{
    dmi_hardware_security_t *info;
    dmi_hardware_security_settings_t settings;

    info = dmi_entity_info(entity, DMI_TYPE(HARDWARE_SECURITY));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = dmi_entity_stream(entity);

    if (not dmi_stream_decode(stream, dmi_byte_t, &settings.__value))
        return false;

    info->front_panel_reset = settings.front_panel_reset;
    info->admin_password    = settings.admin_password;
    info->keyboard_password = settings.keyboard_password;
    info->poweron_password  = settings.poweron_password;

    return true;
}
