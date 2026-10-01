//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/reader.h>
#include <opendmi/module/intel-rsd.h>

#include "cabled-pcie-internal.h"

/**
 * @internal
 * @brief Offset of the number of the cable indices, which the cable indices
 * follow, two bytes each.
 */
#define DMI_INTEL_RSD_CABLED_PCIE_COUNT_OFFSET 0x07

/**
 * @internal
 * @brief Number of the cable indices the specification lays out.
 */
#define DMI_INTEL_RSD_CABLED_PCIE_COUNT_MAX 4

//
// A cable carries a group of four lanes, and the groups start at the lanes of
// a x16 port which are multiples of four.
//
void dmi_intel_rsd_cabled_pcie_lint_start_lane(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_intel_rsd_cabled_pcie_t *info = dmi_entity_info(entity, DMI_TYPE(intel_rsd_cabled_pcie));

    if (info == nullptr)
        return;

    for (size_t i = 0; i < info->port_count; i++) {
        unsigned lane = info->ports[i].start_lane;

        if ((lane <= 12) and (lane % 4 == 0))
            continue;

        dmi_lint_issue(lint, entity, "start-lane",
                       dmi_lint_entity_offset(lint, entity) + DMI_INTEL_RSD_CABLED_PCIE_COUNT_OFFSET + 2 + 2 * i,
                       "start lane %u of cable index %zu is not one of 0, 4, 8 and 12", lane, i);
    }
}

//
// The number is read as stored, since the decoder keeps only the cable
// indices the structure holds.
//
void dmi_intel_rsd_cabled_pcie_lint_count(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_reader_t reader;
    dmi_byte_t   count;

    if (not dmi_reader_initialize(&reader, dmi_entity_buffer(entity),
                                  dmi_entity_offset(entity), entity->body_length))
        return;

    if (not dmi_reader_get_bytes_at(&reader, &count, DMI_INTEL_RSD_CABLED_PCIE_COUNT_OFFSET, sizeof(count)))
        return;

    if (count <= DMI_INTEL_RSD_CABLED_PCIE_COUNT_MAX)
        return;

    dmi_lint_issue(lint, entity, "ports",
                   dmi_lint_entity_offset(lint, entity) + DMI_INTEL_RSD_CABLED_PCIE_COUNT_OFFSET,
                   "%u cable indices are more than the %d the specification lays out",
                   (unsigned)count, DMI_INTEL_RSD_CABLED_PCIE_COUNT_MAX);
}
