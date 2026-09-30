//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/lenovo.h>

#include <opendmi/entity/lenovo/records.h>

static bool dmi_lenovo_date_derive(dmi_entity_t *entity);
static bool dmi_lenovo_tpm_info_derive(dmi_entity_t *entity);

const dmi_entity_spec_t dmi_lenovo_date_spec =
{
    .type        = DMI_TYPE(lenovo_date),
    .code        = "lenovo-date",
    .name        = "Lenovo date record",
    .description = (const char *[]){
        "Holds a date of a ThinkPad, which is probably its manufacture date.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x0D,
        .decoded_length = sizeof(dmi_lenovo_date_t),
        // Structures of the same type holding TPM information are longer
        .signature      = DMI_SIGNATURE({
            .length = 0x0D
        })
    },

    // Date is followed by bytes of zeros
    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_lenovo_date_t, raw_date, dmi_dword_t),
        DMI_FIELD_SKIP(5),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_lenovo_date_t, date, DATE, {
            .code = "date",
            .name = "Date"
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_lenovo_date_derive
    }
};

const dmi_entity_spec_t dmi_lenovo_tpm_info_spec =
{
    .type        = DMI_TYPE(lenovo_tpm_info),
    .code        = "lenovo-tpm-info",
    .name        = "Lenovo TPM information",
    .description = (const char *[]){
        "Tells the vendor of the TPM of a ThinkPad.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x10,
        .decoded_length = sizeof(dmi_lenovo_tpm_info_t),
        .signature      = DMI_SIGNATURE({
            .string = 1,
            .text   = "TPM INFO"
        })
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_lenovo_tpm_info_t, unknown_1, dmi_byte_t),
        DMI_FIELD_BINARY(dmi_lenovo_tpm_info_t, vendor_raw, 4),
        DMI_FIELD_VECTOR(dmi_lenovo_tpm_info_t, unknown_2,
            .fields = DMI_FIELDS({
                DMI_FIELD_ELEMENT(dmi_lenovo_tpm_info_t, unknown_2, dmi_byte_t),
                {}
            })),
        DMI_FIELD(dmi_lenovo_tpm_info_t, unknown_3, dmi_word_t),
        DMI_FIELD(dmi_lenovo_tpm_info_t, unknown_4, dmi_byte_t),

        // Signature is kept, so that the structure is written back with it
        DMI_FIELD_STRING(dmi_lenovo_tpm_info_t, signature),
        DMI_FIELD_STRING(dmi_lenovo_tpm_info_t, description),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_lenovo_tpm_info_t, vendor, STRING, {
            .code = "vendor",
            .name = "TPM vendor ID"
        }),
        DMI_ATTRIBUTE(dmi_lenovo_tpm_info_t, unknown_1, INTEGER, {
            .code  = "unknown-1",
            .name  = "Unknown 1",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE_VECTOR(dmi_lenovo_tpm_info_t, unknown_2, INTEGER, {
            .code  = "unknown-2",
            .name  = "Unknown 2",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_lenovo_tpm_info_t, unknown_3, INTEGER, {
            .code  = "unknown-3",
            .name  = "Unknown 3",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_lenovo_tpm_info_t, unknown_4, INTEGER, {
            .code  = "unknown-4",
            .name  = "Unknown 4",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_lenovo_tpm_info_t, description, STRING, {
            .code = "description",
            .name = "Description"
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_lenovo_tpm_info_derive
    }
};

const dmi_entity_spec_t dmi_lenovo_mtm_spec =
{
    .type        = DMI_TYPE(lenovo_mtm),
    .code        = "lenovo-mtm",
    .name        = "Lenovo machine type model",
    .description = (const char *[]){
        "Tells the brand and the full machine type model of an IdeaPad or a "
        "ThinkBook.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x06,
        .decoded_length = sizeof(dmi_lenovo_mtm_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_STRING(dmi_lenovo_mtm_t, brand),
        DMI_FIELD_STRING(dmi_lenovo_mtm_t, mtm),
        DMI_FIELD_BINARY(dmi_lenovo_mtm_t, data, DMI_FIELD_LENGTH_REST),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_lenovo_mtm_t, brand, STRING, {
            .code = "brand",
            .name = "Brand"
        }),
        DMI_ATTRIBUTE(dmi_lenovo_mtm_t, mtm, STRING, {
            .code = "mtm",
            .name = "Machine type model"
        }),
        DMI_ATTRIBUTE(dmi_lenovo_mtm_t, data, BINARY, {
            .code = "data",
            .name = "Data"
        }),
        {}
    })
};

static unsigned dmi_lenovo_bcd(unsigned value)
{
    return (value >> 4) * 10 + (value & 0x0F);
}

static bool dmi_lenovo_date_derive(dmi_entity_t *entity)
{
    dmi_lenovo_date_t *info = dmi_entity_info(entity, DMI_TYPE(lenovo_date));
    if (info == nullptr)
        return false;

    // Day, month, then the year low byte first, e.g. 27 11 21 20 for
    // November 27, 2021
    uint32_t raw = info->raw_date;
    unsigned day   = dmi_lenovo_bcd(raw & 0xFF);
    unsigned month = dmi_lenovo_bcd((raw >> 8) & 0xFF);
    unsigned year  = dmi_lenovo_bcd((raw >> 24) & 0xFF) * 100 + dmi_lenovo_bcd((raw >> 16) & 0xFF);

    info->date = dmi_date(year, month, day);

    return true;
}

static bool dmi_lenovo_tpm_info_derive(dmi_entity_t *entity)
{
    dmi_lenovo_tpm_info_t *info = dmi_entity_info(entity, DMI_TYPE(lenovo_tpm_info));
    if (info == nullptr)
        return false;

    info->vendor = dmi_text_from_bytes(info->vendor_raw.data, info->vendor_raw.length, info->vendor_buffer, false);

    return true;
}
