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

#include <opendmi/entity/hpe/proliant-info-internal.h>

const dmi_entity_spec_t dmi_hpe_proliant_info_spec =
{
    .type        = DMI_TYPE(HPE_PROLIANT_INFO),
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

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_hpe_proliant_info_t, omega_features, dmi_dword_t),

        DMI_FIELD_GROUP(),
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
        DMI_ATTRIBUTE(dmi_hpe_proliant_info_t, omega_features, INTEGER, {
            .code  = "omega-features",
            .name  = "Omega features",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_proliant_info_t, misc_features, INTEGER, {
            .code  = "misc-features",
            .name  = "Miscellaneous features",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_proliant_info_t, is_icru, BOOL, {
            .code = "is-icru",
            .name = "iCRU"
        }),
        DMI_ATTRIBUTE(dmi_hpe_proliant_info_t, is_uefi, BOOL, {
            .code = "is-uefi",
            .name = "UEFI"
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_hpe_proliant_info_derive
    }
};

bool dmi_hpe_proliant_info_derive(dmi_entity_t *entity)
{
    dmi_hpe_proliant_info_t *info = dmi_entity_info(entity, DMI_TYPE(HPE_PROLIANT_INFO));
    if (info == nullptr)
        return false;

    info->is_icru = (info->misc_features & 0x0001) != 0;
    info->is_uefi = (info->misc_features & 0x1400) != 0;

    return true;
}
