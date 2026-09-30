//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/fvi-internal.h>

void dmi_intel_fvi_cleanup(dmi_entity_t *entity)
{
    dmi_intel_fvi_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_FVI));
    if (info == nullptr)
        return;

    dmi_free(info->items);
}
