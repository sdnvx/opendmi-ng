//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/platform.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include "dimm-location-internal.h"

bool dmi_hpe_dimm_location_derive(dmi_entity_t *entity)
{
    dmi_hpe_dimm_location_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_dimm_location));
    if (info == nullptr)
        return false;

    const dmi_platform_t *platform = dmi_get_platform(dmi_entity_context(entity));
    unsigned generation = (platform != nullptr) ? platform->generation : 0;

    info->is_system_board   = (info->board == UINT8_MAX);
    info->has_ie            = (generation < DMI_HPE_GEN12);

    return true;
}
