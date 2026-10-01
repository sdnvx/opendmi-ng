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

#include "slot-internal.h"

/**
 * @internal
 * @brief Tell whether two slot widths can be compared with each other.
 *
 * @details Widths are comparable within a series only: the ones counting bits
 * and the ones counting lanes are ordered by their values, but say nothing
 * about each other. Anything below the first series stands for "other" or
 * "unknown".
 *
 * @param[in] first  First width.
 * @param[in] second Second width.
 *
 * @return `true` if both widths are known and of the same series, `false`
 *         otherwise.
 */
static bool dmi_slot_width_comparable(dmi_slot_width_t first, dmi_slot_width_t second);

void dmi_slot_lint_width(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_slot_t *info = dmi_entity_info(entity, DMI_TYPE(system_slots));
    if (info == nullptr)
        return;

    if (not dmi_slot_width_comparable(info->physical_width, info->bus_width))
        return;

    if (info->physical_width >= info->bus_width)
        return;

    dmi_lint_issue(lint, entity, "physical-width", dmi_lint_entity_offset(lint, entity),
                   "slot is %s wide, while its bus is %s wide",
                   dmi_slot_width_name(info->physical_width),
                   dmi_slot_width_name(info->bus_width));
}

static bool dmi_slot_width_comparable(dmi_slot_width_t first, dmi_slot_width_t second)
{
    if ((first < DMI_SLOT_WIDTH_8_BIT) or (second < DMI_SLOT_WIDTH_8_BIT))
        return false;

    bool first_lanes  = (first >= DMI_SLOT_WIDTH_1X);
    bool second_lanes = (second >= DMI_SLOT_WIDTH_1X);

    return first_lanes == second_lanes;
}
