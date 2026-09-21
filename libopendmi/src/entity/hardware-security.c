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

#include <opendmi/entity/hardware-security-internal.h>

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
    .params = {
        .minimum_version = DMI_VERSION(2, 2, 0),
        .minimum_length  = 0x05,
        .decoded_length  = sizeof(dmi_hardware_security_t)
    },

    .fields = DMI_FIELDS({
        // One byte holding the four settings, two bits each
        DMI_FIELD_BITS(dmi_hardware_security_t, front_panel_reset, 2),
        DMI_FIELD_BITS(dmi_hardware_security_t, admin_password,    2),
        DMI_FIELD_BITS(dmi_hardware_security_t, keyboard_password, 2),
        DMI_FIELD_BITS(dmi_hardware_security_t, poweron_password,  2),
        DMI_FIELD_PAD(dmi_byte_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
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
        {}
    }),
};
