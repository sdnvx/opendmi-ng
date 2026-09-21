//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>

#include <opendmi/entity/common.h>
#include <opendmi/entity/slot-internal.h>

void dmi_slot_cleanup(dmi_entity_t *entity)
{
    dmi_slot_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_SLOTS));
    if (info == nullptr)
        return;

    dmi_free(info->peer_groups);
}
