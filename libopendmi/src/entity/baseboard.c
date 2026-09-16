//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/baseboard.h>

static bool dmi_baseboard_decode(dmi_entity_t *entity);
static bool dmi_baseboard_link(dmi_entity_t *entity);
static void dmi_baseboard_cleanup(dmi_entity_t *entity);

const dmi_name_set_t dmi_baseboard_type_names =
{
    .code  = "baseboard-types",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_BASEBOARD_TYPE_UNSPEC),
        DMI_NAME_UNKNOWN(DMI_BASEBOARD_TYPE_UNKNOWN),
        DMI_NAME_OTHER(DMI_BASEBOARD_TYPE_OTHER),
        {
            .id   = DMI_BASEBOARD_TYPE_SERVER_BLADE,
            .code = "server-blade",
            .name = "Server blade"
        },
        {
            .id   = DMI_BASEBOARD_TYPE_CONNECTIVITY_SWITCH,
            .code = "connectivity-switch",
            .name = "Connectivity switch"
        },
        {
            .id   =  DMI_BASEBOARD_TYPE_SYSTEM_MANAGEMENT_MODULE,
            .code = "system-management-module",
            .name = "System management module"
        },
        {
            .id   = DMI_BASEBOARD_TYPE_PROCESSOR_MODULE,
            .code = "processor-module",
            .name = "Processor module"
        },
        {
            .id   = DMI_BASEBOARD_TYPE_IO_MODULE,
            .code = "io-module",
            .name = "IO module"
        },
        {
            .id   = DMI_BASEBOARD_TYPE_MEMORY_MODULE,
            .code = "memory-module",
            .name = "Memory module"
        },
        {
            .id   = DMI_BASEBOARD_TYPE_DAUGHTERBOARD,
            .code = "daughterboard",
            .name = "Daughterboard"
        },
        {
            .id   = DMI_BASEBOARD_TYPE_MOTHERBOARD,
            .code = "motherboard",
            .name = "Motherboard"
        },
        {
            .id   = DMI_BASEBOARD_TYPE_PROCESSOR_MEMORY_MODULE,
            .code = "processor-memory-module",
            .name = "Processor/memory module"
        },
        {
            .id   = DMI_BASEBOARD_TYPE_PROCESSOR_IO_MODULE,
            .code = "processor-io-module",
            .name = "Processor/IO module"
        },
        {
            .id   = DMI_BASEBOARD_TYPE_INTERCONNECT_BOARD,
            .code = "interconnect-board",
            .name = "Interconnect board"
        },
        DMI_NAME_NULL
    }
};

const dmi_name_set_t dmi_baseboard_feature_names =
{
    .code  = "baseboard-features",
    .names = (dmi_name_t[]){
        {
            .id   = 0,
            .code = "is-hosting-board",
            .name = "Hosting board"
        },
        {
            .id   = 1,
            .code = "require-daughter-board",
            .name = "Require daughter board"
        },
        {
            .id   = 2,
            .code = "is-removable",
            .name = "Removable"
        },
        {
            .id   = 3,
            .code = "is-replaceable",
            .name = "Replaceable"
        },
        {
            .id   = 4,
            .code = "is-hot-swappable",
            .name = "Hot-swappable"
        },
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_baseboard_spec =
{
    .code            = "baseboard",
    .name            = "Baseboard or module information",
    .description     = (const char *[]){
        "The information in this structure defines attributes of a system "
        "baseboard (for example, a motherboard, planar, server blade, or other "
        "standard system module).",
        //
        "Note: If more than one Type 2 structure is provided by an SMBIOS "
        "implementation, each structure shall include the Number of Contained "
        "Object Handles and Contained Object Handles fields to specify which "
        "system elements are contained on which boards. If a single Type 2 "
        "structure is provided and the contained object information is not "
        "present, or if no Type 2 structure is provided, all system elements "
        "identified by the SMBIOS implementation are associated with a single "
        "motherboard.",
        //
        nullptr
    },
    .type            = DMI_TYPE(BASEBOARD),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x08,
    .decoded_length  = sizeof(dmi_baseboard_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_baseboard_t, vendor, STRING, {
            .code    = "vendor",
            .name    = "Vendor"
        }),
        DMI_ATTRIBUTE(dmi_baseboard_t, product, STRING, {
            .code    = "product",
            .name    = "Product"
        }),
        DMI_ATTRIBUTE(dmi_baseboard_t, version, STRING, {
            .code    = "version",
            .name    = "Version"
        }),
        DMI_ATTRIBUTE(dmi_baseboard_t, serial_number, STRING, {
            .code    = "serial-number",
            .name    = "Serial number"
        }),
        DMI_ATTRIBUTE(dmi_baseboard_t, asset_tag, STRING, {
            .code    = "asset-tag",
            .name    = "Asset tag"
        }),
        DMI_ATTRIBUTE(dmi_baseboard_t, features, SET, {
            .code    = "features",
            .name    = "Features",
            .values  = &dmi_baseboard_feature_names
        }),
        DMI_ATTRIBUTE(dmi_baseboard_t, location, STRING, {
            .code    = "location",
            .name    = "Location"
        }),
        DMI_ATTRIBUTE(dmi_baseboard_t, chassis_handle, HANDLE, {
            .code    = "chassis-handle",
            .name    = "Chassis handle",
            .unspec  = dmi_value_ptr(DMI_HANDLE_INVALID)
        }),
        DMI_ATTRIBUTE(dmi_baseboard_t, type, ENUM, {
            .code    = "type",
            .name    = "Type",
            .unspec  = dmi_value_ptr(DMI_BASEBOARD_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_BASEBOARD_TYPE_UNKNOWN),
            .values  = &dmi_baseboard_type_names
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_baseboard_t, object_handles, object_count, HANDLE, {
            .code    = "contained-objects",
            .name    = "Contained objects"
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode  = dmi_baseboard_decode,
        .link    = dmi_baseboard_link,
        .cleanup = dmi_baseboard_cleanup
    }
};

const char *dmi_baseboard_type_name(dmi_baseboard_type_t value)
{
    return dmi_name_lookup(&dmi_baseboard_type_names, (int)value);
}

static bool dmi_baseboard_decode(dmi_entity_t *entity)
{
    const dmi_baseboard_data_t *data;
    dmi_baseboard_t *info;

    data = dmi_entity_data(entity, DMI_TYPE(BASEBOARD));
    if (data == nullptr)
        return false;

    info = dmi_entity_info(entity, DMI_TYPE(BASEBOARD));
    if (info == nullptr)
        return false;

    if (entity->body_length > 0x04u)
        info->vendor = dmi_entity_string(entity, data->vendor);
    if (entity->body_length > 0x05u)
        info->product = dmi_entity_string(entity, data->product);
    if (entity->body_length > 0x06u)
        info->version = dmi_entity_string(entity, data->version);
    if (entity->body_length > 0x07u)
        info->serial_number = dmi_entity_string(entity, data->serial_number);
    if (entity->body_length > 0x08u)
        info->asset_tag = dmi_entity_string(entity, data->asset_tag);

    if (entity->body_length > 0x09u)
        info->features.__value = dmi_decode(data->features);

    if (entity->body_length > 0x0Au)
        info->location = dmi_entity_string(entity, data->location);

    if (entity->body_length > 0x0Bu)
        info->chassis_handle = dmi_decode(data->chassis_handle);
    else
        info->chassis_handle = DMI_HANDLE_INVALID;

    if (entity->body_length > 0x0Du)
        info->type = dmi_decode(data->type);

    if (entity->body_length > 0x0Eu) {
        info->object_count  = dmi_decode(data->object_count);

        info->object_handles = dmi_alloc_array(entity->context, sizeof(dmi_handle_t), info->object_count);
        if (info->object_handles == nullptr)
            return false;

        for (size_t i = 0; i < info->object_count; i++)
            info->object_handles[i] = dmi_decode(data->object_handles[i]);
    }

    return true;
}

static bool dmi_baseboard_link(dmi_entity_t *entity)
{
    dmi_baseboard_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(BASEBOARD));
    if (info == nullptr)
        return false;

    dmi_context_t  *context  = entity->context;
    dmi_registry_t *registry = context->state.registry;

    if (info->chassis_handle != DMI_HANDLE_INVALID) {
        info->chassis = dmi_registry_get(registry, info->chassis_handle,
                                         DMI_TYPE(CHASSIS), false);
    }

    if (info->object_count > 0) {
        info->objects = dmi_alloc_array(context, sizeof(dmi_entity_t *), info->object_count);

        if (info->objects == nullptr) {
            dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
            return false;
        }

        for (size_t i = 0; i < info->object_count; i++) {
            dmi_handle_t handle = info->object_handles[i];

            if (handle == DMI_HANDLE_INVALID)
                continue;

            info->objects[i] = dmi_registry_get(registry, handle, DMI_TYPE_INVALID, false);
        }
    }

    return true;
}

static void dmi_baseboard_cleanup(dmi_entity_t *entity)
{
    dmi_baseboard_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(BASEBOARD));
    if (info == nullptr)
        return;

    dmi_free(info->object_handles);
    dmi_free(info->objects);
}
