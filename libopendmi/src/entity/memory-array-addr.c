//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/memory-array-addr.h>

static bool dmi_memory_array_addr_validate(dmi_entity_t *entity);
static bool dmi_memory_array_addr_decode(dmi_entity_t *entity);
static bool dmi_memory_array_addr_link(dmi_entity_t *entity);

const dmi_entity_spec_t dmi_memory_array_addr_spec =
{
    .code            = "memory-array-address",
    .name            = "Memory array mapped address",
    .type            = DMI_TYPE(MEMORY_ARRAY_ADDR),
    .minimum_version = DMI_VERSION(2, 1, 0),
    .minimum_length  = 0x0F,
    .decoded_length  = sizeof(dmi_memory_array_addr_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_memory_array_addr_t, start_addr, ADDRESS, {
            .code   = "start-addr",
            .name   = "Starting address",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_memory_array_addr_t, end_addr, ADDRESS, {
            .code  = "end-addr",
            .name  = "Ending address",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_memory_array_addr_t, range_size, SIZE, {
            .code   = "range-size",
            .name   = "Range size"
        }),
        DMI_ATTRIBUTE(dmi_memory_array_addr_t, array_handle, HANDLE, {
            .code   = "array-handle",
            .name   = "Memory array handle"
        }),
        DMI_ATTRIBUTE(dmi_memory_array_addr_t, partition_width, INTEGER, {
            .code   = "partition-width",
            .name   = "Partition width"
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .validate = dmi_memory_array_addr_validate,
        .decode   = dmi_memory_array_addr_decode,
        .link     = dmi_memory_array_addr_link
    }
};

static bool dmi_memory_array_addr_validate(dmi_entity_t *entity)
{
    if ((entity == nullptr) or (entity->type != DMI_TYPE(MEMORY_ARRAY_ADDR)))
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
        dmi_stream_decode_at(stream, 0x0Fu, dmi_qword_t, &start_addr_ex) and
        dmi_stream_decode_at(stream, 0x17u, dmi_qword_t, &end_addr_ex);

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

static bool dmi_memory_array_addr_decode(dmi_entity_t *entity)
{
    dmi_memory_array_addr_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_ARRAY_ADDR));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = dmi_entity_stream(entity);

    bool success = false;

    do {
        bool status;

        uint32_t start_addr = 0;
        uint32_t end_addr   = 0;

        status =
            dmi_stream_decode(stream, dmi_dword_t, &start_addr) and
            dmi_stream_decode(stream, dmi_dword_t, &end_addr) and
            dmi_stream_decode(stream, dmi_handle_t, &info->array_handle) and
            dmi_stream_decode(stream, dmi_byte_t, &info->partition_width);

        if (not status)
            return false;

        info->start_addr = (uint64_t)start_addr << 10u;
        info->end_addr   = (uint64_t)end_addr << 10u;

        // SMBIOS 2.7 fields
        if (dmi_stream_is_done(stream)) {
            success = dmi_entity_stop(entity);
            break;
        }

        entity->level = dmi_version(2, 7, 0);

        // Actual addresses are stored in extended fields, missing extended
        // addresses keep the original values
        uint64_t start_addr_ex = info->start_addr;
        uint64_t end_addr_ex   = info->end_addr;

        status =
            dmi_stream_decode(stream, dmi_qword_t, &start_addr_ex) and
            dmi_stream_decode(stream, dmi_qword_t, &end_addr_ex);

        if (start_addr == 0xFFFFFFFFu) {
            info->start_addr = start_addr_ex;
            info->end_addr   = end_addr_ex;
        }

        success = status ? true : dmi_entity_incomplete(entity);
    } while (false);

    if (info->end_addr > info->start_addr)
        info->range_size = info->end_addr - info->start_addr;
    else
        info->range_size = info->start_addr - info->end_addr;

    return success;
}

static bool dmi_memory_array_addr_link(dmi_entity_t *entity)
{
    dmi_memory_array_addr_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_ARRAY_ADDR));
    if (info == nullptr)
        return false;

    dmi_context_t  *context  = dmi_entity_context(entity);
    dmi_registry_t *registry = dmi_get_registry(context);

    return dmi_registry_resolve(registry, info->array_handle, DMI_TYPE(MEMORY_ARRAY), &info->array);
}
