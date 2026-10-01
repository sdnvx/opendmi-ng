//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include "microcode-internal.h"

const dmi_entity_spec_t dmi_hpe_microcode_spec =
{
    .type        = DMI_TYPE(hpe_microcode),
    .code        = "hpe-microcode",
    .name        = "HP/HPE CPU microcode patch support information",
    .description = (const char *[]){
        "Lists the CPU microcode patches the firmware carries.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x04,
        .decoded_length = sizeof(dmi_hpe_microcode_t)
    },

    // Patches run to the end of the structure, which carries no number of
    // them of its own
    .fields = DMI_FIELDS({
        DMI_FIELD_ARRAY(dmi_hpe_microcode_t, patches, patch_count,
            .stride = 3 * sizeof(dmi_dword_t),
            .fields = DMI_FIELDS({
                DMI_FIELD(dmi_hpe_microcode_patch_t, patch_id, dmi_dword_t),
                DMI_FIELD(dmi_hpe_microcode_patch_t, raw_date, dmi_dword_t),
                DMI_FIELD(dmi_hpe_microcode_patch_t, cpuid,    dmi_dword_t),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE_ARRAY(dmi_hpe_microcode_t, patches, patch_count, STRUCT, {
            .code  = "patches",
            .name  = "Patches",
            .attrs = DMI_ATTRIBUTES({
                DMI_ATTRIBUTE(dmi_hpe_microcode_patch_t, cpuid, INTEGER, {
                    .code  = "cpuid",
                    .name  = "CPU ID",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                DMI_ATTRIBUTE(dmi_hpe_microcode_patch_t, signature, INTEGER, {
                    .code  = "signature",
                    .name  = "Processor signature",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                DMI_ATTRIBUTE(dmi_hpe_microcode_patch_t, date, DATE, {
                    .code = "date",
                    .name = "Date"
                }),
                DMI_ATTRIBUTE(dmi_hpe_microcode_patch_t, patch_id, INTEGER, {
                    .code  = "patch-id",
                    .name  = "Patch",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        {}
    }),

    .handlers = {
        .derive  = dmi_hpe_microcode_derive
    }
};
