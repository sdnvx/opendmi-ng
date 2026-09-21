//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/module/sun.h>

#include <opendmi/entity/sun/memory-device-ex-internal.h>

void dmi_sun_memory_device_ex_cleanup(dmi_entity_t *entity)
{
    dmi_sun_memory_device_ex_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SUN_MEMORY_DEVICE_EX));
    if (info == nullptr)
        return;

    dmi_free(info->chip_selects);
}
