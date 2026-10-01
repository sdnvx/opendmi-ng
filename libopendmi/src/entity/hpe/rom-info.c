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

#include "rom-info-internal.h"

const dmi_entity_spec_t dmi_hpe_rom_info_spec =
{
    .type        = DMI_TYPE(hpe_rom_info),
    .code        = "hpe-rom-info",
    .name        = "HP/HPE other ROM information",
    .description = (const char *[]){
        "Describes the redundant system ROM and the OEM ROM image of the "
        "server.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x07,
        .decoded_length = sizeof(dmi_hpe_rom_info_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_BITS(dmi_hpe_rom_info_t, is_redundant_rom, 1),
        DMI_FIELD_PAD(dmi_byte_t),
        DMI_FIELD_STRING(dmi_hpe_rom_info_t, redundant_rom_version),
        DMI_FIELD_STRING(dmi_hpe_rom_info_t, bootblock_version),

        DMI_FIELD_GROUP(),
        DMI_FIELD_STRING(dmi_hpe_rom_info_t, oem_rom_filename),
        DMI_FIELD_STRING(dmi_hpe_rom_info_t, oem_rom_date),
        DMI_FIELD_GROUP(.present = dmi_member(dmi_hpe_rom_info_t, has_unknown_string)),
        DMI_FIELD_STRING(dmi_hpe_rom_info_t, unknown_string),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_rom_info_t, is_redundant_rom, BOOL, {
            .code = "is-redundant-rom",
            .name = "Redundant ROM installed"
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_rom_info_t, has_redundant_rom_version, {
            .code     = "redundant-rom-version",
            .name     = "Redundant ROM version",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_rom_info_t, redundant_rom_version, STRING, {}),
                {}
            })
        }),
        DMI_ATTRIBUTE(dmi_hpe_rom_info_t, bootblock_version, STRING, {
            .code = "bootblock-version",
            .name = "Boot block version"
        }),
        // Image of the OEM ROM is named by the firmware which has one only
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_rom_info_t, has_oem_rom, {
            .code     = "oem-rom-filename",
            .name     = "OEM ROM binary file name",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_rom_info_t, oem_rom_filename, STRING, {}),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_rom_info_t, has_oem_rom, {
            .code     = "oem-rom-date",
            .name     = "OEM ROM binary build date",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_rom_info_t, oem_rom_date, STRING, {}),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_rom_info_t, has_unknown_string, {
            .code     = "unknown-string",
            .name     = "Unknown string",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_rom_info_t, unknown_string, STRING, {}),
                {}
            })
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_hpe_rom_info_derive
    }
};
