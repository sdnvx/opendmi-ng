//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/apple.h>

#include "processor-type-internal.h"

const dmi_entity_spec_t dmi_apple_processor_type_spec =
{
    .type        = DMI_TYPE(apple_processor_type),
    .code        = "apple-processor-type",
    .name        = "Apple processor type information",
    .description = (const char *[]){
        "Tells the type of the processor, by which macOS names it.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x06,
        .decoded_length = sizeof(dmi_apple_processor_type_t)
    },

    // Type is a word whose low byte is the generation and whose high byte is
    // the class, and newer firmware follows it with two reserved bytes
    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_apple_processor_type_t, generation, dmi_byte_t),
        DMI_FIELD(dmi_apple_processor_type_t, clazz,      dmi_byte_t),
        DMI_FIELD_GROUP(),
        DMI_FIELD_SKIP(2 * sizeof(dmi_byte_t)),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_apple_processor_type_t, clazz, ENUM, {
            .code   = "class",
            .name   = "Processor class",
            .values = &dmi_apple_processor_class_names
        }),
        DMI_ATTRIBUTE(dmi_apple_processor_type_t, generation, INTEGER, {
            .code = "generation",
            .name = "Generation"
        }),
        {}
    })
};

const dmi_name_set_t dmi_apple_processor_class_names =
{
    .code  = "apple-processor-class",
    .names = DMI_NAMES({
        {
            .id   = DMI_APPLE_PROCESSOR_CLASS_UNKNOWN,
            .code = "unknown",
            .name = "Unknown"
        },
        {
            .id   = DMI_APPLE_PROCESSOR_CLASS_CORE,
            .code = "core",
            .name = "Intel Core"
        },
        {
            .id   = DMI_APPLE_PROCESSOR_CLASS_CORE_2,
            .code = "core-2",
            .name = "Intel Core 2"
        },
        {
            .id   = DMI_APPLE_PROCESSOR_CLASS_XEON_PENRYN,
            .code = "xeon-penryn",
            .name = "Intel Xeon (Penryn)"
        },
        {
            .id   = DMI_APPLE_PROCESSOR_CLASS_XEON_NEHALEM,
            .code = "xeon-nehalem",
            .name = "Intel Xeon (Nehalem)"
        },
        {
            .id   = DMI_APPLE_PROCESSOR_CLASS_CORE_I5,
            .code = "core-i5",
            .name = "Intel Core i5"
        },
        {
            .id   = DMI_APPLE_PROCESSOR_CLASS_CORE_I7,
            .code = "core-i7",
            .name = "Intel Core i7"
        },
        {
            .id   = DMI_APPLE_PROCESSOR_CLASS_CORE_I3,
            .code = "core-i3",
            .name = "Intel Core i3"
        },
        {
            .id   = DMI_APPLE_PROCESSOR_CLASS_XEON_E5,
            .code = "xeon-e5",
            .name = "Intel Xeon E5"
        },
        {
            .id   = DMI_APPLE_PROCESSOR_CLASS_CORE_M,
            .code = "core-m",
            .name = "Intel Core M"
        },
        {
            .id   = DMI_APPLE_PROCESSOR_CLASS_CORE_M3,
            .code = "core-m3",
            .name = "Intel Core m3"
        },
        {
            .id   = DMI_APPLE_PROCESSOR_CLASS_CORE_M5,
            .code = "core-m5",
            .name = "Intel Core m5"
        },
        {
            .id   = DMI_APPLE_PROCESSOR_CLASS_CORE_M7,
            .code = "core-m7",
            .name = "Intel Core m7"
        },
        {
            .id   = DMI_APPLE_PROCESSOR_CLASS_XEON_W,
            .code = "xeon-w",
            .name = "Intel Xeon W"
        },
        {
            .id   = DMI_APPLE_PROCESSOR_CLASS_CORE_I9,
            .code = "core-i9",
            .name = "Intel Core i9"
        },
        {}
    })
};

DMI_NAME_FUNCTION(dmi_apple_processor_class)
