//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include "reserved-memory-internal.h"

bool dmi_hpe_reserved_memory_derive(dmi_entity_t *entity)
{
    dmi_hpe_reserved_memory_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_reserved_memory));
    if (info == nullptr)
        return false;

    for (size_t i = 0; i < info->entry_count; i++) {
        dmi_hpe_reserved_memory_entry_t *entry = &info->entries[i];

        entry->size      = (uint64_t)entry->raw_size * (entry->is_kilobytes ? 1024 : 1);
        entry->signature = dmi_text_from_bytes(entry->signature_raw.data, entry->signature_raw.length,
                                        entry->signature_buffer, false);
    }

    return true;
}
