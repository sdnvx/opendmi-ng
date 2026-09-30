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

#include <opendmi/entity/hpe/rom-info.h>

const dmi_entity_spec_t dmi_hpe_rom_info_spec =
{
    .type        = DMI_TYPE(HPE_ROM_INFO),
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
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_rom_info_t, is_redundant_rom, BOOL, {
            .code = "is-redundant-rom",
            .name = "Redundant ROM installed"
        }),
        DMI_ATTRIBUTE(dmi_hpe_rom_info_t, redundant_rom_version, STRING, {
            .code = "redundant-rom-version",
            .name = "Redundant ROM version"
        }),
        DMI_ATTRIBUTE(dmi_hpe_rom_info_t, bootblock_version, STRING, {
            .code = "bootblock-version",
            .name = "Boot block version"
        }),
        DMI_ATTRIBUTE(dmi_hpe_rom_info_t, oem_rom_filename, STRING, {
            .code = "oem-rom-filename",
            .name = "OEM ROM binary file name"
        }),
        DMI_ATTRIBUTE(dmi_hpe_rom_info_t, oem_rom_date, STRING, {
            .code = "oem-rom-date",
            .name = "OEM ROM binary build date"
        }),
        {}
    })
};
