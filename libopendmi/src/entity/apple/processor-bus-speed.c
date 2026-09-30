//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/apple.h>

#include <opendmi/entity/apple/processor-bus-speed.h>

const dmi_entity_spec_t dmi_apple_processor_bus_speed_spec =
{
    .type        = DMI_TYPE(apple_processor_bus_speed),
    .code        = "apple-processor-bus-speed",
    .name        = "Apple processor bus speed information",
    .description = (const char *[]){
        "Tells the speed of the bus of the processor.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x06,
        .decoded_length = sizeof(dmi_apple_processor_bus_speed_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_apple_processor_bus_speed_t, speed, dmi_word_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_apple_processor_bus_speed_t, speed, INTEGER, {
            .code = "speed",
            .name = "Bus speed",
            .unit = DMI_UNIT_MEGAXA_SECOND
        }),
        {}
    })
};
