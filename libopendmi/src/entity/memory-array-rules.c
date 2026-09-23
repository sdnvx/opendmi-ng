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
#include <opendmi/reader.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>
#include <opendmi/entity/memory-device.h>

#include <opendmi/entity/memory-array-internal.h>

void dmi_memory_array_lint_device_count(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_memory_array_t *info = dmi_entity_info(entity, DMI_TYPE(MEMORY_ARRAY));
    if (info == nullptr)
        return;

    size_t count = dmi_memory_array_devices(lint, entity, nullptr);

    // Devices are counted whether they are populated or not, so the number
    // is the number of the sockets of the array
    if (count == info->device_count)
        return;

    dmi_lint_issue(lint, entity, "device-count", dmi_lint_entity_offset(lint, entity),
                   "array declares %u devices, while %zu structures refer to it",
                   info->device_count, count);
}

void dmi_memory_array_lint_capacity(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_memory_array_t *info = dmi_entity_info(entity, DMI_TYPE(MEMORY_ARRAY));

    if ((info == nullptr) or (info->maximum_capacity == DMI_SIZE_MAX) or
        (info->maximum_capacity == 0))
        return;

    dmi_size_t capacity = 0;

    dmi_memory_array_devices(lint, entity, &capacity);

    if (capacity <= info->maximum_capacity)
        return;

    dmi_lint_issue(lint, entity, "maximum-capacity", dmi_lint_entity_offset(lint, entity),
                   "devices of the array add up to %" PRIu64 " bytes, while its maximum "
                   "capacity is %" PRIu64 " bytes", capacity, info->maximum_capacity);
}

void dmi_memory_array_lint_extended_capacity(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_reader_t reader;
    dmi_dword_t value;

    if (not dmi_reader_initialize(&reader, entity))
        return;

    if (not dmi_reader_get_bytes_at(&reader, &value, DMI_MEMORY_ARRAY_CAPACITY_OFFSET, sizeof(value)))
        return;

    if (dmi_decode(value) != DMI_MEMORY_ARRAY_CAPACITY_OFFSET_EXTENDED)
        return;

    if (entity->body_length >= DMI_MEMORY_ARRAY_CAPACITY_OFFSET_LENGTH)
        return;

    dmi_lint_issue(lint, entity, "maximum-capacity", dmi_lint_entity_offset(lint, entity) + DMI_MEMORY_ARRAY_CAPACITY_OFFSET,
                   "Maximum capacity refers to the extended one, which the structure does not carry");
}
