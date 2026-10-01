//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include "processor-internal.h"

bool dmi_hpe_processor_derive(dmi_entity_t *entity)
{
    dmi_hpe_processor_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_processor));
    if (info == nullptr)
        return false;

    info->qdf = dmi_text_from_bytes(info->qdf_raw, sizeof(info->qdf_raw), info->qdf_buffer, true);

    return true;
}
