//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/calling-iface-internal.h>

const dmi_entity_spec_t dmi_dell_calling_iface_spec =
{
    .type            = DMI_TYPE(DELL_CALLING_IFACE),
    .code            = "dell-calling-iface",
    .name            = "Dell calling interface",
    .description     = (const char *[]){
        "Describes the interface for issuing commands to the firmware through "
        "an I/O port, and tokens, which are stored in non-volatile storage "
        "and accessed with these commands.",
        //
        nullptr
    },
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x0B,
        .decoded_length  = sizeof(dmi_dell_calling_iface_t)
    },

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_dell_calling_iface_t, cmd_io_address, INTEGER, {
            .code   = "command-io-address",
            .name   = "Command I/O address",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_calling_iface_t, cmd_io_code, INTEGER, {
            .code   = "command-io-code",
            .name   = "Command I/O code",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_calling_iface_t, supported_cmds, INTEGER, {
            .code   = "supported-commands",
            .name   = "Supported commands",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_dell_calling_iface_t, tokens, token_count, STRUCT, {
            .code   = "tokens",
            .name   = "Tokens",
            .attrs  = dmi_dell_calling_iface_token_attrs
        }),
        {}
    }),

    .handlers = {
        .decode  = dmi_dell_calling_iface_decode,
        .encode  = dmi_dell_calling_iface_encode,
        .cleanup = dmi_dell_calling_iface_cleanup
    }
};
