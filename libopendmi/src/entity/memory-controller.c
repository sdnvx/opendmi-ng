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

#include <opendmi/entity/memory-controller-internal.h>

const dmi_entity_spec_t dmi_memory_controller_spec =
{
    .code            = "memory-controller",
    .name            = "Memory controller information",
    .type            = DMI_TYPE(MEMORY_CONTROLLER),
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x0F,
        .decoded_length  = sizeof(dmi_memory_controller_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_memory_controller_t, error_detection,      BYTE),
        DMI_FIELD(dmi_memory_controller_t, error_correction,     BYTE),
        DMI_FIELD(dmi_memory_controller_t, supported_interleave, BYTE),
        DMI_FIELD(dmi_memory_controller_t, current_interleave,   BYTE),

        // Module size is carried as the power of two it is a number of
        // megabytes of
        DMI_FIELD(dmi_memory_controller_t, maximum_module_size, BYTE,
                  .decode = dmi_memory_controller_decode_size,
                  .encode = dmi_memory_controller_encode_size),

        DMI_FIELD(dmi_memory_controller_t, supported_speeds,  WORD),
        DMI_FIELD(dmi_memory_controller_t, supported_types,   WORD),
        DMI_FIELD(dmi_memory_controller_t, required_voltages, BYTE),

        DMI_FIELD_ARRAY(dmi_memory_controller_t, module_handles, slot_count,
            .count_type = DMI_FIELD_TYPE_BYTE,
            .fields     = DMI_FIELDS({
                DMI_FIELD_ELEMENT(dmi_memory_controller_t, module_handles, WORD),
                {}
            })),

        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 1, 0)),
        DMI_FIELD(dmi_memory_controller_t, enabled_error_correction, BYTE),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_memory_controller_t, error_detection, ENUM, {
            .code   = "error-detection",
            .name   = "Error detecting method",
            .values = &dmi_error_detect_method_names
        }),
        DMI_ATTRIBUTE(dmi_memory_controller_t, error_correction, SET, {
            .code   = "error-correction",
            .name   = "Error correcting capabilities",
            .values = &dmi_error_correct_caps_names
        }),
        DMI_ATTRIBUTE(dmi_memory_controller_t, supported_interleave, ENUM, {
            .code   = "supported-interleave",
            .name   = "Supported interleave",
            .values = &dmi_memory_interleave_names
        }),
        DMI_ATTRIBUTE(dmi_memory_controller_t, current_interleave, ENUM, {
            .code   = "current-interleave",
            .name   = "Current interleave",
            .values = &dmi_memory_interleave_names
        }),
        DMI_ATTRIBUTE(dmi_memory_controller_t, maximum_module_size, SIZE, {
            .code   = "maximum-module-size",
            .name   = "Maximum module size"
        }),
        DMI_ATTRIBUTE(dmi_memory_controller_t, maximum_memory_size, SIZE, {
            .code   = "maximum-memory-size",
            .name   = "Maximum memory size"
        }),
        DMI_ATTRIBUTE(dmi_memory_controller_t, supported_speeds, SET, {
            .code   = "supported-speeds",
            .name   = "Supported speeds",
            .values = &dmi_memory_module_speed_names
        }),
        DMI_ATTRIBUTE(dmi_memory_controller_t, supported_types, SET, {
            .code   = "supported-types",
            .name   = "Supported types",
            .values = &dmi_memory_module_type_names
        }),
        DMI_ATTRIBUTE(dmi_memory_controller_t, required_voltages, SET, {
            .code   = "required-voltages",
            .name   = "Required voltages",
            .values = &dmi_memory_module_voltage_names
        }),
        DMI_ATTRIBUTE(dmi_memory_controller_t, slot_count, INTEGER, {
            .code   = "slot-count",
            .name   = "Slot count"
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_memory_controller_t, module_handles, slot_count, HANDLE, {
            .code   = "module-handles",
            .name   = "Module handles",
            .targets = dmi_types(DMI_TYPE_MEMORY_MODULE),
            .link   = dmi_member(dmi_memory_controller_t, modules)
        }),
        DMI_ATTRIBUTE(dmi_memory_controller_t, enabled_error_correction, SET, {
            .code   = "enabled-error-correction",
            .name   = "Enabled error correcting capabilities",
            .values = &dmi_error_correct_caps_names,
            .level  = DMI_VERSION(2, 1, 0)
        }),
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("memory-controller.module-size", dmi_memory_controller_lint_module_size, {
            .name              = "Modules are no larger than the controller supports",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    }),

    .handlers = {
        .derive  = dmi_memory_controller_derive,
        .link    = dmi_memory_controller_link,
        .cleanup = dmi_memory_controller_cleanup
    }
};
