//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/value.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/battery-internal.h>

//
// The specification puts the SBDS values in place of the ones the structure
// carries itself, so a battery provides either of them, and not both.
//
void dmi_battery_lint_sbds(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_battery_t *info = dmi_entity_info(entity, DMI_TYPE(PORTABLE_BATTERY));
    if (info == nullptr)
        return;

    size_t offset = dmi_lint_entity_offset(lint, entity);

    bool has_serial = (info->serial_number != nullptr) and (*info->serial_number != 0);

    if (has_serial and (info->sbds_serial_number != 0)) {
        dmi_lint_issue(lint, entity, "sbds-serial-number", offset,
                       "battery carries both a serial number and an SBDS one");
    }

    bool has_chemistry = (info->sbds_chemistry != nullptr) and (*info->sbds_chemistry != 0);

    if (has_chemistry and (info->chemistry != DMI_BATTERY_CHEMISTRY_UNKNOWN)) {
        dmi_lint_issue(lint, entity, "sbds-chemistry", offset,
                       "battery carries both a chemistry and an SBDS one");
    }
}
