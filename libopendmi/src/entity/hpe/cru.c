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

#include "cru-internal.h"

const dmi_entity_spec_t dmi_hpe_cru_spec =
{
    .type        = DMI_TYPE(hpe_cru),
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
            .code  = "signature",
            .name  = "Signature",
            .flags = DMI_ATTRIBUTE_FLAG_OWNED
        }),
        // Records of other signatures than "$CRU" describe something else,
        // whose layout is not known
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_cru_t, is_cru, {
            .code     = "address",
            .name     = "Physical address",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_cru_t, address, ADDRESS, {}),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_cru_t, is_cru, {
            .code     = "length",
            .name     = "Length",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_cru_t, length, INTEGER, {
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_cru_t, is_cru, {
            .code     = "offset",
            .name     = "Entry point offset",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_cru_t, offset, INTEGER, {
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_cru_t, is_cru, {
            .code     = "entry-point",
            .name     = "Entry point address",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_cru_t, entry_point, ADDRESS, {}),
                {}
            })
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_hpe_cru_derive
    }
};
