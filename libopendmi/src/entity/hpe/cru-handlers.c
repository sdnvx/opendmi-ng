//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>

#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include "cru-internal.h"

bool dmi_hpe_cru_derive(dmi_entity_t *entity)
{
    dmi_hpe_cru_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_cru));
    if (info == nullptr)
        return false;

    info->entry_point = info->address + info->offset;
    info->is_cru      = (info->signature_raw.length == 4) and
                        (memcmp(info->signature_raw.data, "$CRU", 4) == 0);
    info->signature   = dmi_text_from_bytes(info->signature_raw.data, info->signature_raw.length,
                                     info->signature_buffer, false);

    return true;
}
