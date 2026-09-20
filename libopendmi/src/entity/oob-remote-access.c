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

#include <opendmi/entity/oob-remote-access.h>

static bool dmi_oob_remote_access_decode(dmi_entity_t *entity);

static const dmi_name_set_t dmi_oob_connection_names =
{
    .code  = "oob-connections",
    .names = (dmi_name_t[]){
        {
            .id   = 0,
            .code = "is-inbound-enabled",
            .name = "Inbound connections enabled"
        },
        {
            .id   = 1,
            .code = "is-outbound-enabled",
            .name = "Outbound connections enabled"
        },
        DMI_NAME_NULL
    }
};

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
    .minimum_version = DMI_VERSION(2, 2, 0),
    .minimum_length  = 0x06,
    .decoded_length  = sizeof(dmi_oob_remote_access_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_oob_remote_access_t, vendor, STRING, {
            .code = "vendor",
            .name = "Vendor"
        }),
        DMI_ATTRIBUTE(dmi_oob_remote_access_t, connections, SET, {
            .code   = "connections",
            .name   = "Connections",
            .values = &dmi_oob_connection_names
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_oob_remote_access_decode
    }
};

static bool dmi_oob_remote_access_decode(dmi_entity_t *entity)
{
    dmi_oob_remote_access_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(OOB_REMOTE_ACCESS));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = dmi_entity_stream(entity);

    return
        dmi_stream_decode_str(stream, &info->vendor) and
        dmi_stream_decode(stream, dmi_byte_t, &info->connections.__value);
}
