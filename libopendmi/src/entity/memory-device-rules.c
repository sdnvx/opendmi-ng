//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <inttypes.h>

#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/registry.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/memory-device-internal.h>


/**
 * @internal
 * @brief Offset of the size, the value telling that the actual one is in the
 * extended field, and the length of a structure carrying that field.
 */
#define DMI_MEMORY_DEVICE_SIZE_OFFSET 0x0C
#define DMI_MEMORY_DEVICE_SIZE_OFFSET_EXTENDED 0x7FFF
#define DMI_MEMORY_DEVICE_SIZE_OFFSET_LENGTH 0x20

void dmi_memory_device_lint_width(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_memory_device_t *info = dmi_entity_info(entity, DMI_TYPE(MEMORY_DEVICE));

    if ((info == nullptr) or (info->total_width == USHRT_MAX) or (info->data_width == USHRT_MAX))
        return;

    // Total width covers the data bits and the ones used for error correction
    if (info->total_width >= info->data_width)
        return;

    dmi_lint_issue(lint, entity, "total-width", dmi_lint_entity_offset(lint, entity),
                   "total width of %u bits is less than the data width of %u bits",
                   info->total_width, info->data_width);
}

void dmi_memory_device_lint_speed(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_memory_device_t *info = dmi_entity_info(entity, DMI_TYPE(MEMORY_DEVICE));

    if ((info == nullptr) or (info->maximum_speed == 0) or (info->configured_speed == 0))
        return;

    if (info->configured_speed <= info->maximum_speed)
        return;

    dmi_lint_issue(lint, entity, "configured-speed", dmi_lint_entity_offset(lint, entity),
                   "device is configured at %lu MT/s, while it is capable of %lu MT/s",
                   info->configured_speed, info->maximum_speed);
}

void dmi_memory_device_lint_voltage(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_memory_device_t *info = dmi_entity_info(entity, DMI_TYPE(MEMORY_DEVICE));

    if ((info == nullptr) or (info->configured_voltage == 0))
        return;

    if ((info->minimum_voltage != 0) and (info->configured_voltage < info->minimum_voltage)) {
        dmi_lint_issue(lint, entity, "configured-voltage", dmi_lint_entity_offset(lint, entity),
                       "configured voltage of %u mV is below the minimum of %u mV",
                       info->configured_voltage, info->minimum_voltage);
    }

    if ((info->maximum_voltage != 0) and (info->configured_voltage > info->maximum_voltage)) {
        dmi_lint_issue(lint, entity, "configured-voltage", dmi_lint_entity_offset(lint, entity),
                       "configured voltage of %u mV is above the maximum of %u mV",
                       info->configured_voltage, info->maximum_voltage);
    }
}

void dmi_memory_device_lint_sizes(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_memory_device_t *info = dmi_entity_info(entity, DMI_TYPE(MEMORY_DEVICE));

    if ((info == nullptr) or (info->size == 0) or (info->size == DMI_SIZE_MAX))
        return;

    // Volatile and non-volatile parts are carved out of the device itself
    dmi_size_t total = 0;

    if ((info->volatile_size != 0) and (info->volatile_size != DMI_SIZE_MAX))
        total += info->volatile_size;

    if ((info->non_volatile_size != 0) and (info->non_volatile_size != DMI_SIZE_MAX))
        total += info->non_volatile_size;

    if ((total == 0) or (total <= info->size))
        return;

    dmi_lint_issue(lint, entity, "size", dmi_lint_entity_offset(lint, entity),
                   "volatile and non-volatile sizes add up to %" PRIu64 " bytes, while the "
                   "device is %" PRIu64 " bytes", total, info->size);
}

void dmi_memory_device_lint_extended_size(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_reader_t reader;
    dmi_word_t value;

    if (not dmi_reader_initialize(&reader, entity))
        return;

    if (not dmi_reader_get_bytes_at(&reader, &value, DMI_MEMORY_DEVICE_SIZE_OFFSET, sizeof(value)))
        return;

    if (dmi_decode(value) != DMI_MEMORY_DEVICE_SIZE_OFFSET_EXTENDED)
        return;

    if (entity->body_length >= DMI_MEMORY_DEVICE_SIZE_OFFSET_LENGTH)
        return;

    dmi_lint_issue(lint, entity, "size", dmi_lint_entity_offset(lint, entity) + DMI_MEMORY_DEVICE_SIZE_OFFSET,
                   "Size refers to the extended one, which the structure does not carry");
}
