//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <inttypes.h>
#include <opendmi/context.h>
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/cache-internal.h>

void dmi_cache_lint_size(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_cache_t *info = dmi_entity_info(entity, DMI_TYPE(CACHE));

    if ((info == nullptr) or (info->maximum_size == 0) or (info->maximum_size == DMI_SIZE_MAX))
        return;

    if ((info->installed_size == DMI_SIZE_MAX) or (info->installed_size <= info->maximum_size))
        return;

    dmi_lint_issue(lint, entity, "installed-size", dmi_lint_entity_offset(lint, entity),
                   "installed size of %" PRIu64 " bytes is above the maximum of %" PRIu64
                   " bytes", info->installed_size, info->maximum_size);
}

void dmi_cache_lint_sram(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_cache_t *info = dmi_entity_info(entity, DMI_TYPE(CACHE));

    if ((info == nullptr) or (info->current_sram.__value == 0) or
        (info->supported_sram.__value == 0))
        return;

    // The cache operates in one of the modes it supports, so the current type
    // is one of the supported ones
    if ((info->current_sram.__value & ~info->supported_sram.__value) == 0)
        return;

    dmi_lint_issue(lint, entity, "current-sram", dmi_lint_entity_offset(lint, entity),
                   "current SRAM type 0x%04X is not among the supported ones 0x%04X",
                   (unsigned)info->current_sram.__value, (unsigned)info->supported_sram.__value);
}
