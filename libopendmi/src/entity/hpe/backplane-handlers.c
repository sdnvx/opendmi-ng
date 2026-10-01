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

#include "backplane-internal.h"

bool dmi_hpe_backplane_derive(dmi_entity_t *entity)
{
    dmi_hpe_backplane_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_backplane));
    if (info == nullptr)
        return false;

    const dmi_platform_t *platform = dmi_get_platform(dmi_entity_context(entity));
    unsigned generation = (platform != nullptr) ? platform->generation : 0;

    info->has_legacy_details = (generation < DMI_HPE_GEN10_PLUS);

    return true;
}
