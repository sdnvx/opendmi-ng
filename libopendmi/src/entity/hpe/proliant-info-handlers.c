//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include "proliant-info-internal.h"

bool dmi_hpe_proliant_info_derive(dmi_entity_t *entity)
{
    dmi_hpe_proliant_info_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_proliant_info));
    if (info == nullptr)
        return false;

    info->is_icru = (info->misc_features & 0x0001) != 0;
    info->is_uefi = (info->misc_features & 0x1400) != 0;

    return true;
}
