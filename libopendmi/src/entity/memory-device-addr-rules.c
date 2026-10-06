//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <inttypes.h>
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/registry.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>
#include <opendmi/entity/memory-array-addr.h>

#include "memory-device-addr-internal.h"

void dmi_memory_device_addr_lint_range(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_memory_device_addr_t *info = dmi_entity_info(entity, DMI_TYPE(memory_device_addr));

    if ((info == nullptr) or (info->start_address <= info->end_address))
        return;

    dmi_lint_issue(lint, entity, "start-address", dmi_lint_entity_offset(lint, entity),
                   "range starts at 0x%" PRIX64 " and ends at 0x%" PRIX64,
                   info->start_address, info->end_address);
}

void dmi_memory_device_addr_lint_bounds(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_memory_device_addr_t *info = dmi_entity_info(entity, DMI_TYPE(memory_device_addr));

    if ((info == nullptr) or (info->start_address > info->end_address))
        return;

    dmi_registry_t *registry = dmi_get_registry(dmi_lint_context(lint));

    const dmi_entity_t *array = dmi_registry_lookup(registry, info->array_address_handle,
                                                    DMI_TYPE(memory_array_addr), true);
    if (array == nullptr)
        return;

    const dmi_memory_array_addr_t *range = dmi_entity_info(array, DMI_TYPE(memory_array_addr));

    if ((range == nullptr) or (range->start_address > range->end_address))
        return;

    if ((info->start_address >= range->start_address) and (info->end_address <= range->end_address))
        return;

    dmi_lint_issue(lint, entity, "start-address", dmi_lint_entity_offset(lint, entity),
                   "range 0x%" PRIX64 "-0x%" PRIX64 " is outside the range "
                   "0x%" PRIX64 "-0x%" PRIX64 " of handle 0x%04X",
                   info->start_address, info->end_address, range->start_address, range->end_address,
                   (unsigned)info->array_address_handle);
}
