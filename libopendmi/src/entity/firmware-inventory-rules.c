//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/firmware-inventory-internal.h>

//
// Versions are comparable when they are written the same way, and the one
// installed is not older than the oldest one the component supports.
//
void dmi_firmware_inventory_lint_version(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_firmware_inventory_t *info =
            dmi_entity_info(entity, DMI_TYPE(FIRMWARE_INVENTORY));
    if (info == nullptr)
        return;

    const dmi_firmware_version_t *version = &info->parsed_version;
    const dmi_firmware_version_t *lowest  = &info->parsed_lowest_version;

    if ((version->format != lowest->format) or (version->format == DMI_VERSION_FORMAT_FREE))
        return;

    if (version->value >= lowest->value)
        return;

    dmi_lint_issue(lint, entity, "lowest-version", dmi_lint_entity_offset(lint, entity),
                   "version \"%s\" is older than the lowest supported \"%s\"",
                   (info->version != nullptr) ? info->version : "",
                   (info->lowest_version != nullptr) ? info->lowest_version : "");
}
