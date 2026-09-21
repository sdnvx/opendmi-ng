//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/module/sun.h>

#include <opendmi/entity/sun/processor-ex-internal.h>

void dmi_sun_processor_ex_cleanup(dmi_entity_t *entity)
{
    dmi_sun_processor_ex_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SUN_PROCESSOR_EX));
    if (info == nullptr)
        return;

    dmi_free(info->apic_ids);
}
