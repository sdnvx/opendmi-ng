//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/mei-internal.h>

//
// Registers of an absent PCI function read with all bits set
//
#define DMI_INTEL_MEI_ABSENT UINT32_MAX

bool dmi_intel_mei_derive(dmi_entity_t *entity)
{
    dmi_intel_mei_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_MEI));
    if (info == nullptr)
        return false;

    for (size_t i = 0; i < info->device_count; i++)
        info->devices[i].is_present = (info->devices[i].hfsts[0] != DMI_INTEL_MEI_ABSENT);

    info->state      = DMI_INTEL_ME_STATE_UNSPEC;
    info->mode       = DMI_INTEL_ME_MODE_UNSPEC;
    info->sku        = DMI_INTEL_ME_SKU_UNSPEC;
    info->error_code = UINT8_MAX;

    // State of the firmware is told by the first interface, which is the
    // one of the host, and the layouts of its first and third registers are
    // the same in all generations of the Management Engine
    if ((info->device_count == 0) or not info->devices[0].is_present)
        return true;

    uint32_t hfsts1 = info->devices[0].hfsts[0];
    uint32_t hfsts3 = info->devices[0].hfsts[2];

    info->state            = (dmi_intel_me_state_t)(hfsts1 & 0x0F);
    info->is_manufacturing = (hfsts1 & (1u << 4)) != 0;
    info->is_init_complete = (hfsts1 & (1u << 9)) != 0;
    info->error_code       = (uint8_t)((hfsts1 >> 12) & 0x0F);
    info->mode             = (dmi_intel_me_mode_t)((hfsts1 >> 16) & 0x0F);
    info->sku              = (dmi_intel_me_sku_t)((hfsts3 >> 4) & 0x07);

    return true;
}

void dmi_intel_mei_cleanup(dmi_entity_t *entity)
{
    dmi_intel_mei_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_MEI));
    if (info == nullptr)
        return;

    dmi_free(info->devices);
}
