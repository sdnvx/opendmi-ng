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

#include "proliant-info-internal.h"

const dmi_entity_spec_t dmi_hpe_proliant_info_spec =
{
    .type        = DMI_TYPE(hpe_proliant_info),
    .code        = "hpe-proliant-info",
    .name        = "HP/HPE ProLiant information",
    .description = (const char *[]){
        "Gives the feature flags of the server, which the watchdog timer "
        "driver reads.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x08,
        .decoded_length = sizeof(dmi_hpe_proliant_info_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_hpe_proliant_info_t, power_features, dmi_dword_t),

        DMI_FIELD_GROUP(.present = dmi_member(dmi_hpe_proliant_info_t, has_omega_features)),
        DMI_FIELD(dmi_hpe_proliant_info_t, omega_features, dmi_dword_t),

        DMI_FIELD_GROUP(.present = dmi_member(dmi_hpe_proliant_info_t, has_misc_features)),
        DMI_FIELD_SKIP(4),
        DMI_FIELD(dmi_hpe_proliant_info_t, misc_features, dmi_dword_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_proliant_info_t, power_features, INTEGER, {
            .code  = "power-features",
            .name  = "Power features",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        // Features the structure is too short to hold are not shown
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_proliant_info_t, has_omega_features, {
            .code     = "omega-features",
            .name     = "Omega features",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_proliant_info_t, omega_features, INTEGER, {
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_proliant_info_t, has_misc_features, {
            .code     = "misc-features",
            .name     = "Miscellaneous features",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_proliant_info_t, misc_features, INTEGER, {
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_proliant_info_t, has_misc_features, {
            .code     = "is-icru",
            .name     = "iCRU",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_proliant_info_t, is_icru, BOOL, {}),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_proliant_info_t, has_misc_features, {
            .code     = "is-uefi",
            .name     = "UEFI",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_proliant_info_t, is_uefi, BOOL, {}),
                {}
            })
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_hpe_proliant_info_derive
    }
};

bool dmi_hpe_proliant_info_derive(dmi_entity_t *entity)
{
    dmi_hpe_proliant_info_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_proliant_info));
    if (info == nullptr)
        return false;

    info->is_icru = (info->misc_features & 0x0001) != 0;
    info->is_uefi = (info->misc_features & 0x1400) != 0;

    return true;
}
