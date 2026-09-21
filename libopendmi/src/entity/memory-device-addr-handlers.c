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

#include <opendmi/entity/memory-device-addr-internal.h>

bool dmi_memory_device_addr_validate(dmi_entity_t *entity)
{
    if ((entity == nullptr) or (entity->type != DMI_TYPE(MEMORY_DEVICE_ADDR)))
        return false;

    const dmi_stream_t *stream = dmi_entity_stream(entity);

    uint32_t start_addr = 0, end_addr = 0;
    uint64_t start_addr_ex = 0, end_addr_ex = 0;

    bool has_addr =
        dmi_stream_decode_at(stream, 0x04u, dmi_dword_t, &start_addr) and
        dmi_stream_decode_at(stream, 0x08u, dmi_dword_t, &end_addr);

    if (!has_addr)
        return false;

    bool has_addr_ex =
        dmi_stream_decode_at(stream, 0x13u, dmi_qword_t, &start_addr_ex) and
        dmi_stream_decode_at(stream, 0x1Bu, dmi_qword_t, &end_addr_ex);

    if ((start_addr == 0xFFFFFFFFu) or (end_addr == 0xFFFFFFFFu)) {
        if (start_addr != end_addr)
            return false;

        if (has_addr_ex) {
            if (end_addr_ex <= start_addr_ex)
                return false;
        } else {
            return false;
        }
    } else {
        if (end_addr <= start_addr)
            return false;

        if (has_addr_ex) {
            if ((start_addr_ex != 0) or (end_addr_ex != 0))
                return false;
        }
    }

    return true;
}

//
// Size of the range is what its bounds say, in whichever order the data
// happens to carry them.
//
bool dmi_memory_device_addr_derive(dmi_entity_t *entity)
{
    dmi_memory_device_addr_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_DEVICE_ADDR));
    if (info == nullptr)
        return false;

    if (info->end_addr > info->start_addr)
        info->range_size = info->end_addr - info->start_addr;
    else
        info->range_size = info->start_addr - info->end_addr;

    return true;
}

