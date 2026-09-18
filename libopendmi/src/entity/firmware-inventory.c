//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/firmware-inventory.h>

static bool dmi_firmware_inventory_decode(dmi_entity_t *entity);
static bool dmi_firmware_inventory_link(dmi_entity_t *entity);
static void dmi_firmware_inventory_cleanup(dmi_entity_t *entity);

static const dmi_name_set_t dmi_version_format_names =
{
    .code  = "version-formats",
    .names = (dmi_name_t[]){
        {
            .id   = DMI_VERSION_FORMAT_FREE,
            .code = "free-form",
            .name = "Free form"
        },
        {
            .id   = DMI_VERSION_FORMAT_SEMANTIC,
            .code = "semantic",
            .name = "Semantic"
        },
        {
            .id   = DMI_VERSION_FORMAT_HEX_32,
            .code = "hexadecimal-32",
            .name = "Hexadecimal (32-bit)"
        },
        {
            .id   = DMI_VERSION_FORMAT_HEX_64,
            .code = "hexadecimal-64",
            .name = "Hexadecimal (64-bit)"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_firmware_ident_format_names =
{
    .code  = "firmware-ident-formats",
    .names = (dmi_name_t[]){
        {
            .id   = DMI_FIRMWARE_IDENT_FORMAT_FREE,
            .code = "free-form",
            .name = "Free form"
        },
        {
            .id   = DMI_FIRMWARE_IDENT_FORMAT_GUID,
            .code = "guid",
            .name = "GUID"
        },
        DMI_NAME_NULL
    },
    .ranges = (dmi_name_range_t[]){
        {
            .start_id = __DMI_FIRMWARE_IDENT_FORMAT_RESERVED_START,
            .end_id   = __DMI_FIRMWARE_IDENT_FORMAT_RESERVED_END,
            .code     = "reserved",
            .name     = "Reserved"
        },
        {
            .start_id = __DMI_FIRMWARE_IDENT_FORMAT_VENDOR_SPECIFIC_START,
            .end_id   = __DMI_FIRMWARE_IDENT_FORMAT_VENDOR_SPECIFIC_END,
            .code     = "vendor-specific",
            .name     = "Vendor/OEM-specific"
        },
        DMI_NAME_RANGE_NULL
    }
};

static const dmi_name_set_t dmi_firmware_inventory_feature_names =
{
    .code  = "firmware-inventory-features",
    .names = (dmi_name_t[]){
        {
            .id   = 0,
            .code = "is-updatable",
            .name = "Updatable"
        },
        {
            .id   = 1,
            .code = "is-write-protected",
            .name = "Write-protected"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_firmware_inventory_state_names =
{
    .code  = "firmware-inventory-states",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_FIRMWARE_INVENTORY_STATE_UNSPEC),
        DMI_NAME_OTHER(DMI_FIRMWARE_INVENTORY_STATE_OTHER),
        DMI_NAME_UNKNOWN(DMI_FIRMWARE_INVENTORY_STATE_UNKNOWN),
        {
            .id   = DMI_FIRMWARE_INVENTORY_STATE_DISABLED,
            .code = "disabled",
            .name = "Disabled"
        },
        {
            .id   = DMI_FIRMWARE_INVENTORY_STATE_ENABLED,
            .code = "enabled",
            .name = "Enabled"
        },
        {
            .id   = DMI_FIRMWARE_INVENTORY_STATE_ABSENT,
            .code = "absent",
            .name = "Absent"
        },
        {
            .id   = DMI_FIRMWARE_INVENTORY_STATE_STANDBY_OFFLINE,
            .code = "standby-offline",
            .name = "Standby offline"
        },
        {
            .id   = DMI_FIRMWARE_INVENTORY_STATE_STANDBY_SPARE,
            .code = "standby-spare",
            .name = "Standby spare"
        },
        {
            .id   = DMI_FIRMWARE_INVENTORY_STATE_UNAVAIL_OFFLINE,
            .code = "unavail-offline",
            .name = "Unavailable offline"
        },
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_firmware_inventory_spec =
{
    .code            = "firmware-inventory",
    .name            = "Firmware inventory information",
    .type            = DMI_TYPE(FIRMWARE_INVENTORY),
    .description     = (const char *[]){
        "The information in this structure defines an inventory of firmware "
        "components in the system. This can include firmware components such "
        "as platform firmware, BMC, as well as firmware for other devices in "
        "the system.",
        //
        "The information can be used by software to display the firmware "
        "inventory in a uniform manner. It can also be used by a management "
        "controller, such as a BMC, for remote system management. This "
        "structure is not intended to replace other standard programmatic "
        "interfaces for firmware updates.",
        //
        "One Type 45 structure is provided for each firmware component.",
        //
        nullptr
    },
    .minimum_version = DMI_VERSION(3, 5, 0),
    .minimum_length  = 0x17,
    .decoded_length  = sizeof(dmi_firmware_inventory_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, name, STRING, {
            .code    = "name",
            .name    = "Name"
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, version, STRING, {
            .code    = "version",
            .name    = "Version"
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, version_format, ENUM, {
            .code    = "version-format",
            .name    = "Version format",
            .values  = &dmi_version_format_names
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, ident, STRING, {
            .code    = "ident",
            .name    = "Identifier"
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, ident_format, ENUM, {
            .code    = "ident-format",
            .name    = "Identifier format",
            .values  = &dmi_firmware_ident_format_names
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, release_date, STRING, {
            .code    = "release-date",
            .name    = "Release date"
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, vendor, STRING, {
            .code    = "vendor",
            .name    = "Vendor"
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, lowest_version, STRING, {
            .code    = "lowest-version",
            .name    = "Lowest version"
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, image_size, SIZE, {
            .code    = "image-size",
            .name    = "Image size",
            .unknown = dmi_value_ptr(DMI_SIZE_MAX)
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, features, SET, {
            .code    = "characteristics",
            .name    = "Characteristics",
            .values  = &dmi_firmware_inventory_feature_names
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, state, ENUM, {
            .code    = "state",
            .name    = "State",
            .unspec  = dmi_value_ptr(DMI_FIRMWARE_INVENTORY_STATE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_FIRMWARE_INVENTORY_STATE_UNSPEC),
            .values  = &dmi_firmware_inventory_state_names
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, component_count, INTEGER, {
            .code    = "component-count",
            .name    = "Associated components count"
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_firmware_inventory_t, components, component_count, STRUCT, {
            .code    = "components",
            .name    = "Associated components",
            .attrs   = (const dmi_attribute_t[]){
                DMI_ATTRIBUTE(dmi_firmware_inventory_component_t, handle, HANDLE, {
                    .code = "handle",
                    .name = "Handle"
                }),
                {}
            }
        }),
        {}
    },
    .handlers = {
        .decode  = dmi_firmware_inventory_decode,
        .link    = dmi_firmware_inventory_link,
        .cleanup = dmi_firmware_inventory_cleanup
    }
};

const char *dmi_version_format_name(dmi_version_format_t value)
{
    return dmi_name_lookup(&dmi_version_format_names, (int)value);
}

const char *dmi_firmware_ident_format_name(dmi_firmware_ident_format_t value)
{
    return dmi_name_lookup(&dmi_firmware_ident_format_names, (int)value);
}

const char *dmi_firmware_inventory_state_name(dmi_firmware_inventory_state_t value)
{
    return dmi_name_lookup(&dmi_firmware_inventory_state_names, (int)value);
}

static bool dmi_firmware_inventory_decode(dmi_entity_t *entity)
{
    dmi_firmware_inventory_t *info;

    assert(entity != nullptr);

    info = dmi_entity_info(entity, DMI_TYPE(FIRMWARE_INVENTORY));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    bool status =
        dmi_stream_decode_str(stream, &info->name) and
        dmi_stream_decode_str(stream, &info->version) and
        dmi_stream_decode(stream, dmi_byte_t, &info->version_format) and
        dmi_stream_decode_str(stream, &info->ident) and
        dmi_stream_decode(stream, dmi_byte_t, &info->ident_format) and
        dmi_stream_decode_str(stream, &info->release_date) and
        dmi_stream_decode_str(stream, &info->vendor) and
        dmi_stream_decode_str(stream, &info->lowest_version) and
        dmi_stream_decode(stream, dmi_qword_t, &info->image_size) and
        dmi_stream_decode(stream, dmi_word_t, &info->features.__value) and
        dmi_stream_decode(stream, dmi_byte_t, &info->state);

    if (not status)
        return false;

    // Associated components
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    dmi_byte_t component_count = 0;
    if (not dmi_stream_decode(stream, dmi_byte_t, &component_count))
        return false;

    if (component_count == 0)
        return true;

    info->components = dmi_alloc_array(entity->context,
                                       sizeof(dmi_firmware_inventory_component_t),
                                       component_count);
    if (info->components == nullptr)
        return false;

    // Only completely present component handles are counted
    for (size_t i = 0; i < component_count; i++) {
        dmi_firmware_inventory_component_t *component = &info->components[i];

        if (not dmi_stream_decode(stream, dmi_word_t, &component->handle))
            return dmi_entity_incomplete(entity);

        info->component_count++;
    }

    return true;
}

static bool dmi_firmware_inventory_link(dmi_entity_t *entity)
{
    dmi_firmware_inventory_t *info;

    assert(entity != nullptr);

    info = dmi_entity_info(entity, DMI_TYPE(FIRMWARE_INVENTORY));
    if (info == nullptr)
        return false;

    dmi_registry_t *registry = entity->context->state.registry;
    bool success = true;

    for (size_t i = 0; i < info->component_count; i++) {
        dmi_firmware_inventory_component_t *component = &info->components[i];

        if (not dmi_registry_resolve(registry, component->handle, DMI_TYPE_INVALID, &component->entity))
            success = false;
    }

    return success;
}

static void dmi_firmware_inventory_cleanup(dmi_entity_t *entity)
{
    dmi_firmware_inventory_t *info;

    assert(entity != nullptr);

    info = dmi_entity_info(entity, DMI_TYPE(FIRMWARE_INVENTORY));
    if (info == nullptr)
        return;

    dmi_free(info->components);
}
