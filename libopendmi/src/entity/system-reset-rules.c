//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/reader.h>
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <limits.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/system-reset-internal.h>

void dmi_system_reset_lint_limit(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_system_reset_t *info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_RESET));
    if (info == nullptr)
        return;

    // Both counters hold 0xFFFF when they are unknown
    if ((info->reset_count == USHRT_MAX) or (info->reset_limit == USHRT_MAX))
        return;

    if (info->reset_count <= info->reset_limit)
        return;

    dmi_lint_issue(lint, entity, "reset-count", dmi_lint_entity_offset(lint, entity),
                   "system has been reset %u times, while the limit is %u",
                   info->reset_count, info->reset_limit);
}
