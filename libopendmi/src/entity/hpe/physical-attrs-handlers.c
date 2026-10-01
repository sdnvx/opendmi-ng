//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include "physical-attrs-internal.h"

bool dmi_hpe_physical_attrs_derive(dmi_entity_t *entity)
{
    dmi_hpe_physical_attrs_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_physical_attrs));
    if (info == nullptr)
        return false;

    info->identifier = dmi_text_from_bytes(info->identifier_raw.data, info->identifier_raw.length,
                                    info->identifier_buffer, false);

    return true;
}
