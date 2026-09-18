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

#include <opendmi/entity/memory-channel.h>
#include <opendmi/entity/memory-device.h>

static bool dmi_memory_channel_decode(dmi_entity_t *entity);
static bool dmi_memory_channel_link(dmi_entity_t *entity);
static void dmi_memory_channel_cleanup(dmi_entity_t *entity);

static const dmi_name_set_t dmi_memory_channel_type_names =
{
    .code  = "memory-channel-types",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_MEMORY_CHANNEL_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_MEMORY_CHANNEL_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_MEMORY_CHANNEL_TYPE_UNKNOWN),
        {
            .id   = DMI_MEMORY_CHANNEL_TYPE_RAMBUS,
            .code = "rambus",
            .name = "RamBus"
        },
        {
            .id   = DMI_MEMORY_CHANNEL_TYPE_SYNCLINK,
            .code = "synclink",
            .name = "SyncLink"
        },
        DMI_NAME_NULL
    }
};

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
    .minimum_version = DMI_VERSION(2, 3, 0),
    .minimum_length  = 0x0A,
    .decoded_length  = sizeof(dmi_memory_channel_t),
    .attributes      = (const dmi_attribute_t[]){
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
            .attrs   = (const dmi_attribute_t[]){
                DMI_ATTRIBUTE(dmi_memory_channel_device_t, load, INTEGER, {
                    .code = "load",
                    .name = "Load"
                }),
                DMI_ATTRIBUTE(dmi_memory_channel_device_t, handle, HANDLE, {
                    .code = "handle",
                    .name = "Handle"
                }),
                DMI_ATTRIBUTE_NULL
            }
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode  = dmi_memory_channel_decode,
        .link    = dmi_memory_channel_link,
        .cleanup = dmi_memory_channel_cleanup
    }
};

const char *dmi_memory_channel_type_name(dmi_memory_channel_type_t value)
{
    return dmi_name_lookup(&dmi_memory_channel_type_names, (int)value);
}

static bool dmi_memory_channel_decode(dmi_entity_t *entity)
{
    dmi_memory_channel_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_CHANNEL));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    bool status =
        dmi_stream_decode(stream, dmi_byte_t, &info->type) and
        dmi_stream_decode(stream, dmi_byte_t, &info->maximum_load) and
        dmi_stream_decode(stream, dmi_byte_t, &info->device_count);
    if (not status)
        return false;

    if (info->device_count > 0) {
        info->devices = dmi_alloc_array(entity->context, sizeof(dmi_memory_channel_device_t),
                                        info->device_count);
        if (info->devices == nullptr)
            return false;

        for (size_t i = 0; i < info->device_count; i++) {
            dmi_memory_channel_device_t *device = &info->devices[i];

            status =
                dmi_stream_decode(stream, dmi_byte_t, &device->load) and
                dmi_stream_decode(stream, dmi_word_t, &device->handle);
            if (not status)
                return false;
        }
    }

    return true;
}

static bool dmi_memory_channel_link(dmi_entity_t *entity)
{
    dmi_memory_channel_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_CHANNEL));
    if (info == nullptr)
        return false;

    dmi_registry_t *registry = entity->context->state.registry;
    bool success = true;

    for (size_t i = 0; i < info->device_count; i++) {
        dmi_entity_t *device;

        if (not dmi_registry_resolve(registry, info->devices[i].handle, DMI_TYPE(MEMORY_DEVICE), &device)) {
            success = false;
            continue;
        }

        info->devices[i].device = device;
        if (device == nullptr)
            continue;

        // Memory device may be left undecoded
        dmi_memory_device_t *device_info = dmi_entity_info(device, DMI_TYPE(MEMORY_DEVICE));
        if (device_info != nullptr)
            device_info->channel = entity;
    }

    return success;
}

static void dmi_memory_channel_cleanup(dmi_entity_t *entity)
{
    dmi_memory_channel_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_CHANNEL));
    if (info == nullptr)
        return;

    dmi_free(info->devices);
}
