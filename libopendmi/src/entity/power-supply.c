//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/value.h>

#include <opendmi/entity/power-supply-internal.h>

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
    .params = {
        .minimum_version = DMI_VERSION(2, 3, 1),
        .minimum_length  = 0x10,
        .decoded_length  = sizeof(dmi_power_supply_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_power_supply_t, group, dmi_byte_t),
        DMI_FIELD_STRING(dmi_power_supply_t, location),
        DMI_FIELD_STRING(dmi_power_supply_t, name),
        DMI_FIELD_STRING(dmi_power_supply_t, vendor),
        DMI_FIELD_STRING(dmi_power_supply_t, serial_number),
        DMI_FIELD_STRING(dmi_power_supply_t, asset_tag),
        DMI_FIELD_STRING(dmi_power_supply_t, part_number),
        DMI_FIELD_STRING(dmi_power_supply_t, revision),
        DMI_FIELD(dmi_power_supply_t, maximum_capacity, dmi_word_t),

        // Characteristics: one word holding six fields of its own
        DMI_FIELD_OFFSET(0x0E),
        DMI_FIELD_BITS(dmi_power_supply_t, hot_swappable,   1),
        DMI_FIELD_BITS(dmi_power_supply_t, present,         1),
        DMI_FIELD_BITS(dmi_power_supply_t, unplugged,       1),
        DMI_FIELD_BITS(dmi_power_supply_t, range_switching, 4),
        DMI_FIELD_BITS(dmi_power_supply_t, status,          3),
        DMI_FIELD_BITS(dmi_power_supply_t, type,            4),
        DMI_FIELD_PAD(dmi_word_t),

        DMI_FIELD_OFFSET(0x10),
        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_power_supply_t, voltage_probe_handle,  dmi_word_t,
                  .absent = dmi_value_ptr(DMI_HANDLE_INVALID)),
        DMI_FIELD(dmi_power_supply_t, cooling_device_handle, dmi_word_t,
                  .absent = dmi_value_ptr(DMI_HANDLE_INVALID)),
        DMI_FIELD(dmi_power_supply_t, current_probe_handle,  dmi_word_t,
                  .absent = dmi_value_ptr(DMI_HANDLE_INVALID)),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
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
            .targets = dmi_types(DMI_TYPE_VOLTAGE_PROBE),
            .link    = dmi_member(dmi_power_supply_t, voltage_probe)
        }),
        DMI_ATTRIBUTE(dmi_power_supply_t, cooling_device_handle, HANDLE, {
            .code    = "cooling-device-handle",
            .name    = "Cooling device handle",
            .targets = dmi_types(DMI_TYPE_COOLING_DEVICE),
            .link    = dmi_member(dmi_power_supply_t, cooling_device)
        }),
        DMI_ATTRIBUTE(dmi_power_supply_t, current_probe_handle, HANDLE, {
            .code    = "current-probe-handle",
            .name    = "Input current probe handle",
            .targets = dmi_types(DMI_TYPE_CURRENT_PROBE),
            .link    = dmi_member(dmi_power_supply_t, current_probe)
        }),
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("power-supply.probes", dmi_power_supply_lint_probes, {
            .name              = "Probes of the power supply are of the kinds it measures",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    })
};
