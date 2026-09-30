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

#include <opendmi/entity/hpe/microcode-internal.h>

const dmi_entity_spec_t dmi_hpe_microcode_spec =
{
    .type        = DMI_TYPE(HPE_MICROCODE),
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
        .derive  = dmi_hpe_microcode_derive,
        .cleanup = dmi_hpe_microcode_cleanup
    }
};

//
// Date is 0xMMDDYYYY, each part in binary-coded decimal, the way the header
// of the microcode update writes it.
//
static unsigned dmi_hpe_microcode_bcd(uint32_t value, unsigned digits)
{
    unsigned result = 0;

    for (unsigned i = digits; i > 0; i--)
        result = result * 10 + ((value >> ((i - 1) * 4)) & 0x0F);

    return result;
}

bool dmi_hpe_microcode_derive(dmi_entity_t *entity)
{
    dmi_hpe_microcode_t *info = dmi_entity_info(entity, DMI_TYPE(HPE_MICROCODE));
    if (info == nullptr)
        return false;

    for (size_t i = 0; i < info->patch_count; i++) {
        dmi_hpe_microcode_patch_t *patch = &info->patches[i];
        uint32_t raw = patch->raw_date;

        patch->date = dmi_date(dmi_hpe_microcode_bcd(raw & 0xFFFF, 4),
                               dmi_hpe_microcode_bcd(raw >> 24, 2),
                               dmi_hpe_microcode_bcd((raw >> 16) & 0xFF, 2));
    }

    return true;
}

void dmi_hpe_microcode_cleanup(dmi_entity_t *entity)
{
    dmi_hpe_microcode_t *info = dmi_entity_info(entity, DMI_TYPE(HPE_MICROCODE));
    if (info == nullptr)
        return;

    dmi_free(info->patches);
}
