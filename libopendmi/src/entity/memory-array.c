//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <inttypes.h>
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/registry.h>
#include <opendmi/lint.h>
#include <opendmi/stream.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>
#include <opendmi/entity/memory-device.h>

#include <opendmi/entity/memory-array-internal.h>

const dmi_entity_spec_t dmi_memory_array_spec =
{
    .code            = "memory-array",
    .name            = "Physical memory array",
    .description     = (const char *[]){
        "This structure describes a collection of memory devices that operate "
        "together to form a memory address space.",
        //
        nullptr
    },
    .type            = DMI_TYPE(MEMORY_ARRAY),
    .params = {
        .minimum_version = DMI_VERSION(2, 1, 0),
        .required_from   = DMI_VERSION(2, 3, 0),
        .minimum_length  = 0x0F,
        .decoded_length  = sizeof(dmi_memory_array_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_memory_array_t, location,         BYTE),
        DMI_FIELD(dmi_memory_array_t, usage,            BYTE),
        DMI_FIELD(dmi_memory_array_t, error_correction, BYTE),

        // Capacity is carried in kilobytes, and arrays of two tebibytes or
        // more carry it in the extended field instead
        DMI_FIELD(dmi_memory_array_t, maximum_capacity, DWORD,
                  .unknown_raw = 0x80000000u,
                  .convert     = dmi_field_kilobytes),

        DMI_FIELD(dmi_memory_array_t, error_info_handle, WORD),
        DMI_FIELD(dmi_memory_array_t, device_count,      WORD),

        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 7, 0)),
        DMI_FIELD_EXTENDED(dmi_memory_array_t, maximum_capacity, QWORD,
                           .when_raw = 0x80000000u),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_memory_array_t, location, ENUM, {
            .code    = "location",
            .name    = "Location",
            .unspec  = dmi_value_ptr(DMI_MEMORY_ARRAY_LOCATION_UNSPEC),
            .unknown = dmi_value_ptr(DMI_MEMORY_ARRAY_LOCATION_UNKNOWN),
            .values  = &dmi_memory_array_location_names
        }),
        DMI_ATTRIBUTE(dmi_memory_array_t, usage, ENUM, {
            .code    = "use",
            .name    = "Use",
            .unspec  = dmi_value_ptr(DMI_MEMORY_ARRAY_USAGE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_MEMORY_ARRAY_USAGE_UNKNOWN),
            .values  = &dmi_memory_array_usage_names
        }),
        DMI_ATTRIBUTE(dmi_memory_array_t, error_correction, ENUM, {
            .code    = "error-correction",
            .name    = "Memory error correction",
            .unspec  = dmi_value_ptr(DMI_ERROR_CORRECT_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_ERROR_CORRECT_TYPE_UNSPEC),
            .values  = &dmi_error_correct_type_names
        }),
        DMI_ATTRIBUTE(dmi_memory_array_t, maximum_capacity, SIZE, {
            .code    = "maximum-capacity",
            .name    = "Maximum capacity",
            .unknown = dmi_value_ptr(DMI_SIZE_MAX)
        }),
        DMI_ATTRIBUTE(dmi_memory_array_t, error_info_handle, HANDLE, {
            .code    = "error-handle",
            .name    = "Memory error information handle",
            .targets = dmi_types(DMI_TYPE_MEMORY_ERROR_32, DMI_TYPE_MEMORY_ERROR_64),
        }),
        DMI_ATTRIBUTE(dmi_memory_array_t, device_count, INTEGER, {
            .code    = "device-count",
            .name    = "Number of memory devices"
        }),
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("memory-array.extended-capacity", dmi_memory_array_lint_extended_capacity, {
            .name              = "Extended maximum capacity is present when the plain one needs it",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        DMI_LINT_RULE("memory-array.device-count", dmi_memory_array_lint_device_count, {
            .name              = "Number of the devices matches the ones referring to the array",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        DMI_LINT_RULE("memory-array.capacity", dmi_memory_array_lint_capacity, {
            .name              = "Devices of the array fit its maximum capacity",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    }),

    .handlers = {
        .link   = dmi_memory_array_link
    }
};
