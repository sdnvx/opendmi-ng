//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/value.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/cooling-device.h>

static bool dmi_cooling_device_decode(dmi_entity_t *entity);
static bool dmi_cooling_device_link(dmi_entity_t *entity);

static const dmi_name_set_t dmi_cooling_device_type_names =
{
    .code  = "cooling-device-types",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_COOLING_DEVICE_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_COOLING_DEVICE_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_COOLING_DEVICE_TYPE_UNKNOWN),
        {
            .id   = DMI_COOLING_DEVICE_TYPE_FAN,
            .code = "fan",
            .name = "Fan"
        },
        {
            .id   = DMI_COOLING_DEVICE_TYPE_CENTRIFUGAL_BLOWER,
            .code = "centrifugal-blower",
            .name = "Centrifugal blower"
        },
        {
            .id   = DMI_COOLING_DEVICE_TYPE_CHIP_FAN,
            .code = "chip-fan",
            .name = "Chip fan"
        },
        {
            .id   = DMI_COOLING_DEVICE_TYPE_CABINET_FAN,
            .code = "cabinet-fan",
            .name = "Cabinet fan"
        },
        {
            .id   = DMI_COOLING_DEVICE_TYPE_POWER_SUPPLY_FAN,
            .code = "power-supply-fan",
            .name = "Power supply fan"
        },
        {
            .id   = DMI_COOLING_DEVICE_TYPE_HEAT_PIPE,
            .code = "heat-pipe",
            .name = "Heat pipe"
        },
        {
            .id   = DMI_COOLING_DEVICE_TYPE_INTEGRATED_REFRIGERATION,
            .code = "integrated-refrigeration",
            .name = "Integrated refrigeration"
        },
        {
            .id   = DMI_COOLING_DEVICE_TYPE_ACTIVE_COOLING,
            .code = "active-cooling",
            .name = "Active cooling"
        },
        {
            .id   = DMI_COOLING_DEVICE_TYPE_PASSIVE_COOLING,
            .code = "passive-cooling",
            .name = "Passive cooling"
        },
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_cooling_device_spec =
{
    .code            = "cooling-device",
    .name            = "Cooling device",
    .description     = (const char *[]){
        "This structure describes the attributes for a cooling device in the "
        "system. Each structure describes a single cooling device.",
        //
        nullptr
    },
    .type            = DMI_TYPE(COOLING_DEVICE),
    .minimum_version = DMI_VERSION(2, 2, 0),
    .minimum_length  = 0x0C,
    .decoded_length  = sizeof(dmi_cooling_device_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_cooling_device_t, probe_handle, HANDLE, {
            .code    = "probe-handle",
            .name    = "Temperature probe handle"
        }),
        DMI_ATTRIBUTE(dmi_cooling_device_t, type, ENUM, {
            .code    = "type",
            .name    = "Type",
            .unspec  = dmi_value_ptr(DMI_COOLING_DEVICE_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_COOLING_DEVICE_TYPE_UNKNOWN),
            .values  = &dmi_cooling_device_type_names
        }),
        DMI_ATTRIBUTE(dmi_cooling_device_t, status, ENUM, {
            .code    = "status",
            .name    = "Status",
            .unspec  = dmi_value_ptr(DMI_STATUS_UNSPEC),
            .unknown = dmi_value_ptr(DMI_STATUS_UNKNOWN),
            .values  = &dmi_status_names
        }),
        DMI_ATTRIBUTE(dmi_cooling_device_t, group, INTEGER, {
            .code    = "group",
            .name    = "Group"
        }),
        DMI_ATTRIBUTE(dmi_cooling_device_t, oem_defined, INTEGER, {
            .code    = "oem-defined",
            .name    = "OEM-defined",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_cooling_device_t, nominal_speed, INTEGER, {
            .code    = "nominal-speed",
            .name    = "Nominal speed",
            .unit    = DMI_UNIT_REVOLUTION,
            .unknown = dmi_value_ptr((short)SHRT_MIN)
        }),
        DMI_ATTRIBUTE(dmi_cooling_device_t, description, STRING, {
            .code    = "description",
            .name    = "Description",
            .level   = DMI_VERSION(2, 7, 0)
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_cooling_device_decode,
        .link   = dmi_cooling_device_link
    }
};

const char *dmi_cooling_device_type_name(dmi_cooling_device_type_t value)
{
    return dmi_name_lookup(&dmi_cooling_device_type_names, (int)value);
}

static bool dmi_cooling_device_decode(dmi_entity_t *entity)
{
    dmi_cooling_device_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(COOLING_DEVICE));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    if (not dmi_stream_decode(stream, dmi_handle_t, &info->probe_handle))
        return false;

    dmi_cooling_device_details_t details;
    if (not dmi_stream_decode(stream, dmi_byte_t, &details))
        return false;

    info->type   = details.type;
    info->status = details.status;

    bool status =
        dmi_stream_decode(stream, dmi_byte_t, &info->group) and
        dmi_stream_decode(stream, dmi_dword_t, &info->oem_defined);
    if (not status)
        return false;

    info->nominal_speed = SHRT_MIN;

    // Nominal speed
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    dmi_word_t nominal_speed = 0;
    if (not dmi_stream_decode(stream, dmi_word_t, &nominal_speed))
        return dmi_entity_incomplete(entity);

    info->nominal_speed = nominal_speed != 0x8000u
                        ? (short)(nominal_speed & 0x7FFFu)
                        : SHRT_MIN;

    // SMBIOS 2.7 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 7, 0);

    if (not dmi_stream_decode_str(stream, &info->description))
        return dmi_entity_incomplete(entity);

    return true;
}

static bool dmi_cooling_device_link(dmi_entity_t *entity)
{
    dmi_cooling_device_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(COOLING_DEVICE));
    if (info == nullptr)
        return false;

    dmi_registry_t *registry = entity->context->state.registry;

    return dmi_registry_resolve(registry, info->probe_handle, DMI_TYPE(TEMPERATURE_PROBE), &info->probe);
}
