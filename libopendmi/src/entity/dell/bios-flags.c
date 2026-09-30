//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/bios-flags-internal.h>

const dmi_entity_spec_t dmi_dell_bios_flags_spec =
{
    .type        = DMI_TYPE(dell_bios_flags),
    .code        = "dell-bios-flags",
    .name        = "Dell BIOS flags",
    .description = (const char *[]){
        "Tells the features of the firmware the Dell drivers rely on, of "
        "which only the support of ACPI WMI is known.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x0C,
        .decoded_length = sizeof(dmi_dell_bios_flags_t)
    },

    // Flags take 8 bytes, which the Dell SMBIOS WMI driver of Linux reads
    // the first word of, and the rest of which is not known to carry anything
    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_dell_bios_flags_t, flags, dmi_word_t),
        DMI_FIELD_SKIP(6),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_dell_bios_flags_t, flags, INTEGER, {
            .code  = "flags",
            .name  = "Flags",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_bios_flags_t, is_acpi_wmi, BOOL, {
            .code = "is-acpi-wmi",
            .name = "ACPI WMI supported"
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_dell_bios_flags_derive
    }
};

bool dmi_dell_bios_flags_derive(dmi_entity_t *entity)
{
    dmi_dell_bios_flags_t *info = dmi_entity_info(entity, DMI_TYPE(dell_bios_flags));
    if (info == nullptr)
        return false;

    info->is_acpi_wmi = (info->flags & 0x0002) != 0;

    return true;
}
