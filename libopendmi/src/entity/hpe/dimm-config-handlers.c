//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include "dimm-config-internal.h"

bool dmi_hpe_dimm_config_derive(dmi_entity_t *entity)
{
    dmi_hpe_dimm_config_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_dimm_config));
    if (info == nullptr)
        return false;

    info->size                  = info->raw_size * 1024 * 1024;
    info->is_passphrase_enabled = (info->passphrase_state != 0);

    return true;
}
