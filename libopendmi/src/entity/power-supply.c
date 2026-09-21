//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/registry.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/power-supply.h>

static bool dmi_power_supply_decode(dmi_entity_t *entity);
static bool dmi_power_supply_link(dmi_entity_t *entity);

static const dmi_name_set_t dmi_power_supply_type_names =
{
    .code  = "power-supply-type",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_POWER_SUPPLY_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_POWER_SUPPLY_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_POWER_SUPPLY_TYPE_UNKNOWN),
        {
            .id   = DMI_POWER_SUPPLY_TYPE_LINEAR,
            .code = "linear",
            .name = "Linear"
        },
        {
            .id   = DMI_POWER_SUPPLY_TYPE_SWITCHING,
            .code = "switching",
            .name = "Switching"
        },
        {
            .id   = DMI_POWER_SUPPLY_TYPE_BATTERY,
            .code = "battery",
            .name = "Battery"
        },
        {
            .id   = DMI_POWER_SUPPLY_TYPE_UPS,
            .code = "ups",
            .name = "UPS"
        },
        {
            .id   = DMI_POWER_SUPPLY_TYPE_CONVERTER,
            .code = "converter",
            .name = "Converter"
        },
        {
            .id   = DMI_POWER_SUPPLY_TYPE_REGULATOR,
            .code = "regulator",
            .name = "Regulator"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_range_switching_type_names =
{
    .code  = "range-switching-type",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_RANGE_SWITCHING_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_RANGE_SWITCHING_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_RANGE_SWITCHING_TYPE_UNKNOWN),
        {
            .id   = DMI_RANGE_SWITCHING_TYPE_MANUAL,
            .code = "manual",
            .name = "Manual"
        },
        {
            .id   = DMI_RANGE_SWITCHING_TYPE_AUTO,
            .code = "auto",
            .name = "Auto-switch"
        },
        {
            .id   = DMI_RANGE_SWITCHING_TYPE_WIDE,
            .code = "wide-range",
            .name = "Wide range"
        },
        {
            .id   = DMI_RANGE_SWITCHING_TYPE_NOT_APPLICABLE,
            .code = "not-applicable",
            .name = "Not applicable"
        },
        DMI_NAME_NULL
    }
};

static void dmi_power_supply_lint_probes(dmi_lint_t *lint, const dmi_entity_t *entity);

static const dmi_lint_rule_t dmi_power_supply_probes_rule =
{
    .code              = "power-supply.probes",
    .name              = "Probes of the power supply are of the kinds it measures",
    .severity          = DMI_LINT_SEVERITY_WARNING,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_power_supply_lint_probes
};

const dmi_entity_spec_t dmi_power_supply_spec =
{
    .code            = "power-supply",
    .name            = "System power supply",
    .description     = (const char *[]){
        "This structure identifies attributes of a system power supply. One "
        "instance of this structure is present for each possible power supply "
        "in a system.",
        //
        nullptr
    },
    .type            = DMI_TYPE(POWER_SUPPLY),
    .minimum_version = DMI_VERSION(2, 3, 1),
    .minimum_length  = 0x10,
    .decoded_length  = sizeof(dmi_power_supply_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_power_supply_t, group, INTEGER, {
            .code    = "group",
            .name    = "Power unit group"
        }),
        DMI_ATTRIBUTE(dmi_power_supply_t, location, STRING, {
            .code    = "location",
            .name    = "Location"
        }),
        DMI_ATTRIBUTE(dmi_power_supply_t, name, STRING, {
            .code    = "name",
            .name    = "Name"
        }),
        DMI_ATTRIBUTE(dmi_power_supply_t, vendor, STRING, {
            .code    = "vendor",
            .name    = "Manufacturer"
        }),
        DMI_ATTRIBUTE(dmi_power_supply_t, serial_number, STRING, {
            .code    = "serial-number",
            .name    = "Serial number"
        }),
        DMI_ATTRIBUTE(dmi_power_supply_t, asset_tag, STRING, {
            .code    = "asset-tag",
            .name    = "Asset tag"
        }),
        DMI_ATTRIBUTE(dmi_power_supply_t, part_number, STRING, {
            .code    = "part-number",
            .name    = "Part number"
        }),
        DMI_ATTRIBUTE(dmi_power_supply_t, revision, STRING, {
            .code    = "revision",
            .name    = "Revision"
        }),
        DMI_ATTRIBUTE(dmi_power_supply_t, maximum_capacity, INTEGER, {
            .code    = "maximum-capacity",
            .name    = "Maximum capacity",
            .unit    = DMI_UNIT_WATT,
            .unknown = dmi_value_ptr((short)SHRT_MIN),
            .flags   = DMI_ATTRIBUTE_FLAG_SIGNED
        }),
        DMI_ATTRIBUTE(dmi_power_supply_t, hot_swappable, BOOL, {
            .code    = "hot-swappable",
            .name    = "Hot-swappable"
        }),
        DMI_ATTRIBUTE(dmi_power_supply_t, present, BOOL, {
            .code    = "present",
            .name    = "Present"
        }),
        DMI_ATTRIBUTE(dmi_power_supply_t, unplugged, BOOL, {
            .code    = "unplugged",
            .name    = "Unplugged"
        }),
        DMI_ATTRIBUTE(dmi_power_supply_t, range_switching, ENUM, {
            .code    = "range-switching",
            .name    = "Input voltage range switching",
            .values  = &dmi_range_switching_type_names
        }),
        DMI_ATTRIBUTE(dmi_power_supply_t, status, ENUM, {
            .code    = "status",
            .name    = "Status",
            .values  = &dmi_status_names
        }),
        DMI_ATTRIBUTE(dmi_power_supply_t, type, ENUM, {
            .code    = "type",
            .name    = "Type",
            .values  = &dmi_power_supply_type_names
        }),
        DMI_ATTRIBUTE(dmi_power_supply_t, voltage_probe_handle, HANDLE, {
            .code    = "voltage-probe-handle",
            .name    = "Input voltage probe handle",
            .targets = dmi_targets(DMI_TYPE_VOLTAGE_PROBE),
        }),
        DMI_ATTRIBUTE(dmi_power_supply_t, cooling_device_handle, HANDLE, {
            .code    = "cooling-device-handle",
            .name    = "Cooling device handle",
            .targets = dmi_targets(DMI_TYPE_COOLING_DEVICE),
        }),
        DMI_ATTRIBUTE(dmi_power_supply_t, current_probe_handle, HANDLE, {
            .code    = "current-probe-handle",
            .name    = "Input current probe handle",
            .targets = dmi_targets(DMI_TYPE_CURRENT_PROBE),
        }),
        DMI_ATTRIBUTE_NULL
    },
    .lint_rules      = (const dmi_lint_rule_t *const[]){
        &dmi_power_supply_probes_rule,
        nullptr
    },

    .handlers = {
        .decode = dmi_power_supply_decode,
        .link   = dmi_power_supply_link
    }
};

const char *dmi_power_supply_type_name(dmi_power_supply_type_t value)
{
    return dmi_name_lookup(&dmi_power_supply_type_names, (int)value);
}

const char *dmi_range_switching_type_name(dmi_range_switching_type_t value)
{
    return dmi_name_lookup(&dmi_range_switching_type_names, (int)value);
}

static bool dmi_power_supply_decode(dmi_entity_t *entity)
{
    dmi_power_supply_t *info;
    dmi_power_supply_details_t details;

    info = dmi_entity_info(entity, DMI_TYPE(POWER_SUPPLY));
    if (info == nullptr)
        return false;

    // Pre-initialize optional fields
    info->voltage_probe_handle  = DMI_HANDLE_INVALID;
    info->cooling_device_handle = DMI_HANDLE_INVALID;
    info->current_probe_handle  = DMI_HANDLE_INVALID;

    dmi_stream_t *stream = dmi_entity_stream(entity);

    // Decode mandatory fields
    bool status =
        dmi_stream_decode(stream, dmi_byte_t, &info->group) and
        dmi_stream_decode_str(stream, &info->location) and
        dmi_stream_decode_str(stream, &info->name) and
        dmi_stream_decode_str(stream, &info->vendor) and
        dmi_stream_decode_str(stream, &info->serial_number) and
        dmi_stream_decode_str(stream, &info->asset_tag) and
        dmi_stream_decode_str(stream, &info->part_number) and
        dmi_stream_decode_str(stream, &info->revision) and
        dmi_stream_decode(stream, dmi_word_t, &info->maximum_capacity) and
        dmi_stream_decode(stream, dmi_word_t, &details);
    if (not status)
        return false;

    // Decode details
    info->hot_swappable   = details.hot_swappable;
    info->present         = details.present;
    info->unplugged       = details.unplugged;
    info->range_switching = details.range_switching;
    info->status          = details.status;
    info->type            = details.type;

    // Decode optional fields, status is ignored.
    dmi_stream_decode(stream, dmi_handle_t, &info->voltage_probe_handle) and
    dmi_stream_decode(stream, dmi_handle_t, &info->cooling_device_handle) and
    dmi_stream_decode(stream, dmi_handle_t, &info->current_probe_handle);

    return true;
}

static bool dmi_power_supply_link(dmi_entity_t *entity)
{
    dmi_power_supply_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(POWER_SUPPLY));
    if (info == nullptr)
        return false;

    dmi_context_t  *context  = dmi_entity_context(entity);
    dmi_registry_t *registry = dmi_get_registry(context);

    bool success = true;
    if (not dmi_registry_resolve(registry, info->voltage_probe_handle, DMI_TYPE(VOLTAGE_PROBE), &info->voltage_probe))
        success = false;
    if (not dmi_registry_resolve(registry, info->cooling_device_handle, DMI_TYPE(COOLING_DEVICE), &info->cooling_device))
        success = false;
    if (not dmi_registry_resolve(registry, info->current_probe_handle, DMI_TYPE(CURRENT_PROBE), &info->current_probe))
        success = false;

    return success;
}

static void dmi_power_supply_lint_probes(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_power_supply_t *info = dmi_entity_info(entity, DMI_TYPE(POWER_SUPPLY));
    if (info == nullptr)
        return;

    const struct
    {
        dmi_handle_t handle;
        dmi_type_t   type;
        const char  *code;
    } probes[] =
    {
        { info->voltage_probe_handle,  DMI_TYPE_VOLTAGE_PROBE,  "voltage-probe-handle"  },
        { info->cooling_device_handle, DMI_TYPE_COOLING_DEVICE, "cooling-device-handle" },
        { info->current_probe_handle,  DMI_TYPE_CURRENT_PROBE,  "current-probe-handle"  }
    };

    dmi_registry_t *registry = dmi_get_registry(dmi_lint_context(lint));

    for (size_t i = 0; i < countof(probes); i++) {
        if ((probes[i].handle == DMI_HANDLE_INVALID) or
            (probes[i].handle == DMI_HANDLE_UNSUPPORTED))
            continue;

        const dmi_entity_t *probe =
                dmi_registry_lookup(registry, probes[i].handle, DMI_TYPE_ANY, true);

        if ((probe == nullptr) or (dmi_entity_type(probe) == probes[i].type))
            continue;

        dmi_lint_issue(lint, entity, probes[i].code, dmi_lint_entity_offset(lint, entity),
                       "handle 0x%04X refers to a structure of type %d, expected type %d",
                       (unsigned)probes[i].handle, (int)dmi_entity_type(probe),
                       (int)probes[i].type);
    }
}
