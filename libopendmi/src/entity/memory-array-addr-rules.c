//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <inttypes.h>
#include <opendmi/registry.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/memory-array-addr-internal.h>

void dmi_memory_array_addr_lint_range(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_memory_array_addr_t *info = dmi_entity_info(entity, DMI_TYPE(MEMORY_ARRAY_ADDR));

    if ((info == nullptr) or (info->start_addr <= info->end_addr))
        return;

    dmi_lint_issue(lint, entity, "start-addr", dmi_lint_entity_offset(lint, entity),
                   "range starts at 0x%" PRIX64 " and ends at 0x%" PRIX64,
                   info->start_addr, info->end_addr);
}

//
// Ranges of the arrays describe the physical address space of the system, so
// no two of them may claim the same addresses.
//
void dmi_memory_array_addr_lint_overlap(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_memory_array_addr_t *info = dmi_entity_info(entity, DMI_TYPE(MEMORY_ARRAY_ADDR));

    if ((info == nullptr) or (info->start_addr > info->end_addr))
        return;

    dmi_registry_t *registry = dmi_get_registry(dmi_lint_context(lint));
    dmi_registry_iter_t iter;
    dmi_entity_t *other;

    if (not dmi_registry_iter_init(&iter, registry, nullptr))
        return;

    while ((other = dmi_registry_iter_next(&iter)) != nullptr) {
        if ((other == entity) or (dmi_entity_type(other) != DMI_TYPE_MEMORY_ARRAY_ADDR))
            continue;

        // Every pair is reported once, by the structure which comes later
        if (dmi_entity_handle(other) >= dmi_entity_handle(entity))
            continue;

        const dmi_memory_array_addr_t *peer =
                dmi_entity_info(other, DMI_TYPE(MEMORY_ARRAY_ADDR));

        if ((peer == nullptr) or (peer->start_addr > peer->end_addr))
            continue;

        if ((info->start_addr > peer->end_addr) or (info->end_addr < peer->start_addr))
            continue;

        dmi_lint_issue(lint, entity, "start-addr", dmi_lint_entity_offset(lint, entity),
                       "range 0x%" PRIX64 "-0x%" PRIX64 " overlaps the one of handle 0x%04X",
                       info->start_addr, info->end_addr, (unsigned)dmi_entity_handle(other));
    }
}
