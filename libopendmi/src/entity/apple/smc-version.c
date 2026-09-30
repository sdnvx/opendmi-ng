//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>

#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/apple.h>

#include <opendmi/entity/apple/smc-version-internal.h>

const dmi_entity_spec_t dmi_apple_smc_version_spec =
{
    .type        = DMI_TYPE(apple_smc_version),
    .code        = "apple-smc-version",
    .name        = "Apple SMC version information",
    .description = (const char *[]){
        "Tells the version of the firmware of the System Management "
        "Controller (SMC).",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x14,
        .decoded_length = sizeof(dmi_apple_smc_version_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_BINARY(dmi_apple_smc_version_t, version_raw, DMI_APPLE_SMC_VERSION_SIZE),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_apple_smc_version_t, version, STRING, {
            .code = "version",
            .name = "SMC version"
        }),
        DMI_ATTRIBUTE(dmi_apple_smc_version_t, version_raw, BINARY, {
            .code = "version-raw",
            .name = "SMC version data"
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_apple_smc_version_derive
    }
};

bool dmi_apple_smc_version_derive(dmi_entity_t *entity)
{
    dmi_apple_smc_version_t *info = dmi_entity_info(entity, DMI_TYPE(apple_smc_version));
    if (info == nullptr)
        return false;

    // Text is followed by bytes of zero up to the end of the field
    const uint8_t *end    = memchr(info->version_raw.data, 0, info->version_raw.length);
    size_t         length = (end != nullptr) ? (size_t)(end - info->version_raw.data) : info->version_raw.length;

    info->version = dmi_text_from_bytes(info->version_raw.data, length, info->version_buffer, false);

    return true;
}
