//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/lenovo.h>

#include <opendmi/entity/lenovo/tvt-internal.h>

const dmi_entity_spec_t dmi_lenovo_tvt_spec =
{
    .type        = DMI_TYPE(lenovo_tvt),
    .code        = "lenovo-tvt",
    .name        = "Lenovo ThinkVantage Technologies enablement",
    .description = (const char *[]){
        "Tells which ThinkVantage Technologies the platform enables, of which "
        "only the diagnostics are known.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x16,
        .decoded_length = sizeof(dmi_lenovo_tvt_t),
        .signature      = DMI_SIGNATURE({
            .length = 0x16,
            .string = 1,
            .text   = "TVT-Enablement"
        })
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_lenovo_tvt_t, version, dmi_byte_t),
        DMI_FIELD_BINARY(dmi_lenovo_tvt_t, features, 16),

        // Signature is kept, so that the structure is written back with it
        DMI_FIELD_STRING(dmi_lenovo_tvt_t, signature),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_lenovo_tvt_t, version, INTEGER, {
            .code = "version",
            .name = "Version"
        }),
        DMI_ATTRIBUTE(dmi_lenovo_tvt_t, features, BINARY, {
            .code = "features",
            .name = "Features"
        }),
        DMI_ATTRIBUTE(dmi_lenovo_tvt_t, is_diagnostics, BOOL, {
            .code = "is-diagnostics",
            .name = "Diagnostics available"
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_lenovo_tvt_derive
    }
};

bool dmi_lenovo_tvt_derive(dmi_entity_t *entity)
{
    dmi_lenovo_tvt_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(lenovo_tvt));
    if (info == nullptr)
        return false;

    // Bit 127 is the last one of the last byte
    info->is_diagnostics = (info->features.length == 16) and ((info->features.data[15] & 0x80) != 0);

    return true;
}
