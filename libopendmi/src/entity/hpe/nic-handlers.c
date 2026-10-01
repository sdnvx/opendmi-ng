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

bool dmi_hpe_nic_derive(dmi_entity_t *entity)
{
    dmi_hpe_nic_info_t *info = dmi_entity_info(entity, DMI_TYPE_ANY);
    if (info == nullptr)
        return false;

    for (size_t i = 0; i < info->port_count; i++) {
        dmi_hpe_nic_port_t *port = &info->ports[i];

        if ((port->bus == 0x00) and (port->devfn == 0x00))
            port->state = DMI_HPE_NIC_STATE_DISABLED;
        else if ((port->bus == 0xFF) and (port->devfn == 0xFF))
            port->state = DMI_HPE_NIC_STATE_NOT_INSTALLED;
        else
            port->state = DMI_HPE_NIC_STATE_INSTALLED;
    }

    return true;
}
