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

#include <opendmi/entity/hpe/cru-internal.h>

const dmi_entity_spec_t dmi_hpe_cru_spec =
{
    .type        = DMI_TYPE(HPE_CRU),
    .code        = "hpe-cru",
    .name        = "HP/HPE 64-bit CRU information",
    .description = (const char *[]){
        "Tells where the 64-bit entry point of the Compaq ROM Utility (CRU) "
        "services of the firmware is, which the watchdog timer driver calls.",
        //
        nullptr
    },
    .params = {
        .generations    = { .maximum = DMI_HPE_GEN8 },
        .minimum_length = 0x18,
        .decoded_length = sizeof(dmi_hpe_cru_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_BINARY(dmi_hpe_cru_t, signature_raw, 4),
        DMI_FIELD(dmi_hpe_cru_t, address, dmi_qword_t),
        DMI_FIELD(dmi_hpe_cru_t, length,  dmi_dword_t),
        DMI_FIELD(dmi_hpe_cru_t, offset,  dmi_dword_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_cru_t, signature, STRING, {
            .code = "signature",
            .name = "Signature"
        }),
        DMI_ATTRIBUTE(dmi_hpe_cru_t, address, ADDRESS, {
            .code = "address",
            .name = "Physical address"
        }),
        DMI_ATTRIBUTE(dmi_hpe_cru_t, length, INTEGER, {
            .code  = "length",
            .name  = "Length",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_cru_t, offset, INTEGER, {
            .code  = "offset",
            .name  = "Entry point offset",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_cru_t, entry_point, ADDRESS, {
            .code = "entry-point",
            .name = "Entry point address"
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_hpe_cru_derive
    }
};

bool dmi_hpe_cru_derive(dmi_entity_t *entity)
{
    dmi_hpe_cru_t *info = dmi_entity_info(entity, DMI_TYPE(HPE_CRU));
    if (info == nullptr)
        return false;

    info->entry_point = info->address + info->offset;
    info->signature   = dmi_text_from_bytes(info->signature_raw.data, info->signature_raw.length,
                                     info->signature_buffer, false);

    return true;
}
