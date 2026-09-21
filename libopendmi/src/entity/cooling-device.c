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
#include <opendmi/registry.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/cooling-device-internal.h>

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
    .params = {
        .minimum_version = DMI_VERSION(2, 2, 0),
        .minimum_length  = 0x0C,
        .decoded_length  = sizeof(dmi_cooling_device_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_cooling_device_t, probe_handle, WORD),

        DMI_FIELD_BITS(dmi_cooling_device_t, type,   5),
        DMI_FIELD_BITS(dmi_cooling_device_t, status, 3),
        DMI_FIELD_PAD(BYTE),

        DMI_FIELD(dmi_cooling_device_t, group,       BYTE),
        DMI_FIELD(dmi_cooling_device_t, oem_defined, DWORD),

        // Devices which read nothing of their own carry no speed
        DMI_FIELD_PRESET(dmi_cooling_device_t, nominal_speed, (short)SHRT_MIN),
        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_cooling_device_t, nominal_speed, WORD,
                  .decode = dmi_cooling_device_decode_speed,
                  .encode = dmi_cooling_device_encode_speed),

        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 7, 0)),
        DMI_FIELD(dmi_cooling_device_t, description, STRING),
        {}
    }),

    .attributes      = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_cooling_device_t, probe_handle, HANDLE, {
            .code    = "probe-handle",
            .name    = "Temperature probe handle",
            .targets = dmi_types(DMI_TYPE_TEMPERATURE_PROBE),
            .link    = dmi_member(dmi_cooling_device_t, probe)
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
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("cooling-device.probe", dmi_cooling_device_lint_probe, {
            .name              = "Probe of the cooling device is a temperature probe",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    })
};
