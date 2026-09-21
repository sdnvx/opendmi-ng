//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>
#include <opendmi/entity/memory-device.h>

#include <opendmi/entity/memory-channel-internal.h>

const dmi_entity_spec_t dmi_memory_channel_spec =
{
    .code            = "memory-channel",
    .name            = "Memory channel",
    .type            = DMI_TYPE(MEMORY_CHANNEL),
    .description     = (const char *[]){
        "The information in this structure provides the correlation between "
        "a Memory Channel and its associated Memory Devices. Each device "
        "presents one or more loads to the channel; the sum of all device "
        "loads cannot exceed the channel\'s defined maximum.",
        //
        nullptr
    },
    .params = {
        .minimum_version = DMI_VERSION(2, 3, 0),
        .minimum_length  = 0x0A,
        .decoded_length  = sizeof(dmi_memory_channel_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_memory_channel_t, type,         dmi_byte_t),
        DMI_FIELD(dmi_memory_channel_t, maximum_load, dmi_byte_t),

        DMI_FIELD_ARRAY(dmi_memory_channel_t, devices, device_count,
            .count_length = sizeof(dmi_byte_t),
            .fields       = DMI_FIELDS({
                DMI_FIELD(dmi_memory_channel_device_t, load,   dmi_byte_t),
                DMI_FIELD(dmi_memory_channel_device_t, handle, dmi_word_t),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_memory_channel_t, type, ENUM, {
            .code    = "type",
            .name    = "Type",
            .unspec  = dmi_value_ptr(DMI_MEMORY_CHANNEL_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_MEMORY_CHANNEL_TYPE_UNKNOWN),
            .values  = &dmi_memory_channel_type_names,
        }),
        DMI_ATTRIBUTE(dmi_memory_channel_t, maximum_load, INTEGER, {
            .code    = "maximum-load",
            .name    = "Maximum load"
        }),
        DMI_ATTRIBUTE(dmi_memory_channel_t, device_count, INTEGER, {
            .code    = "device-count",
            .name    = "Device count"
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_memory_channel_t, devices, device_count, STRUCT, {
            .code    = "devices",
            .name    = "Devices",
            .attrs   = DMI_ATTRIBUTES({
                DMI_ATTRIBUTE(dmi_memory_channel_device_t, load, INTEGER, {
                    .code = "load",
                    .name = "Load"
                }),
                DMI_ATTRIBUTE(dmi_memory_channel_device_t, handle, HANDLE, {
                    .code = "handle",
                    .name = "Handle",
                    .targets = dmi_types(DMI_TYPE_MEMORY_DEVICE),
                    .link = dmi_member(dmi_memory_channel_device_t, device)
                }),
                {}
            })
        }),
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("memory-channel.load", dmi_memory_channel_lint_load, {
            .name              = "Devices of the channel fit the load it supports",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    }),

    .handlers = {
        .link    = dmi_memory_channel_link,
        .cleanup = dmi_memory_channel_cleanup
    }
};
