//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/apple.h>

#include "platform-feature-internal.h"

const dmi_entity_spec_t dmi_apple_platform_feature_spec =
{
    .type        = DMI_TYPE(apple_platform_feature),
    .code        = "apple-platform-feature",
    .name        = "Apple platform feature information",
    .description = (const char *[]){
        "Tells the features of the platform, which macOS reads.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x0C,
        .decoded_length = sizeof(dmi_apple_platform_feature_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_apple_platform_feature_t, features, dmi_qword_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_apple_platform_feature_t, features, SET, {
            .code   = "features",
            .name   = "Platform features",
            .values = &dmi_apple_platform_feature_bit_names
        }),
        {}
    })
};

// Bits of no known meaning are named after their numbers, the way OpenCore
// names them, since they are not reserved
const dmi_name_set_t dmi_apple_platform_feature_bit_names =
{
    .code  = "apple-platform-feature-bit",
    .names = DMI_NAMES({
        {
            .id   = 0,
            .code = "unknown-bit-0",
            .name = "Unknown bit 0"
        },
        {
            .id   = DMI_APPLE_PLATFORM_FEATURE_SOLDERED_MEMORY,
            .code = "soldered-memory",
            .name = "Soldered system memory"
        },
        {
            .id   = DMI_APPLE_PLATFORM_FEATURE_HEADLESS_GPU,
            .code = "headless-gpu",
            .name = "Headless GPU"
        },
        {
            .id   = DMI_APPLE_PLATFORM_FEATURE_HOST_PM,
            .code = "host-pm",
            .name = "Host power management"
        },
        {
            .id   = DMI_APPLE_PLATFORM_FEATURE_POWER_CHIME,
            .code = "power-chime",
            .name = "Boot chime"
        },
        {}
    })
};

const char *dmi_apple_platform_feature_bit_name(dmi_apple_platform_feature_bit_t value)
{
    return dmi_name_lookup(&dmi_apple_platform_feature_bit_names, (int)value);
}
