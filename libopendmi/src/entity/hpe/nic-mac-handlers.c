//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include "nic-internal.h"
#include "nic-mac-internal.h"

bool dmi_hpe_nic_mac_derive(dmi_entity_t *entity)
{
    dmi_hpe_nic_mac_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_nic));
    if (info == nullptr)
        return false;

    if ((info->bus == 0x00) and (info->devfn == 0x00))
        info->state = DMI_HPE_NIC_STATE_DISABLED;
    else if ((info->bus == 0xFF) and (info->devfn == 0xFF))
        info->state = DMI_HPE_NIC_STATE_NOT_INSTALLED;
    else
        info->state = DMI_HPE_NIC_STATE_INSTALLED;

    return true;
}
