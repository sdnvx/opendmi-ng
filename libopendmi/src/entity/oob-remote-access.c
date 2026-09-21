//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/stream.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/oob-remote-access-internal.h>

const dmi_entity_spec_t dmi_oob_remote_access_spec =
{
    .code            = "oob-remote-access",
    .name            = "Out-of-band remote access",
    .description     = (const char *[]){
        "This structure describes the attributes and policy settings of a "
        "hardware facility that may be used to gain remote access to a "
        "hardware system when the operating system is not available due to "
        "power-down status, hardware failures, or boot failures.",
        //
        nullptr
    },
    .type            = DMI_TYPE(OOB_REMOTE_ACCESS),
    .params = {
        .minimum_version = DMI_VERSION(2, 2, 0),
        .minimum_length  = 0x06,
        .decoded_length  = sizeof(dmi_oob_remote_access_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_STRING(dmi_oob_remote_access_t, vendor),
        DMI_FIELD(dmi_oob_remote_access_t, connections.__value, dmi_byte_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_oob_remote_access_t, vendor, STRING, {
            .code = "vendor",
            .name = "Vendor"
        }),
        DMI_ATTRIBUTE(dmi_oob_remote_access_t, connections, SET, {
            .code   = "connections",
            .name   = "Connections",
            .values = &dmi_oob_connection_names
        }),
        {}
    }),
};
