//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>
#include <opendmi/entity/memory-device.h>

#include <opendmi/entity/memory-channel-internal.h>

//
// Devices of a channel share its capacity, so the load they put on it
// together fits the maximum it supports.
//
void dmi_memory_channel_lint_load(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_memory_channel_t *info = dmi_entity_info(entity, DMI_TYPE(MEMORY_CHANNEL));

    if ((info == nullptr) or (info->devices == nullptr) or (info->device_count == 0))
        return;

    if (info->maximum_load == 0)
        return;

    unsigned total = 0;

    for (size_t i = 0; i < info->device_count; i++)
        total += info->devices[i].load;

    if (total <= info->maximum_load)
        return;

    dmi_lint_issue(lint, entity, "load", dmi_lint_entity_offset(lint, entity),
                   "devices of the channel put a load of %u on it, while it supports %u",
                   total, info->maximum_load);
}
