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

#include <opendmi/entity/memory-error-32.h>
#include <opendmi/entity/memory-error-64.h>

#include "memory-device-internal.h"

/**
 * @internal
 * @brief Offset of the size, the value telling that the actual one is in the
 * extended field, and the length of a structure carrying that field.
 */
#define DMI_MEMORY_DEVICE_SIZE_OFFSET 0x0C
#define DMI_MEMORY_DEVICE_SIZE_OFFSET_EXTENDED 0x7FFF
#define DMI_MEMORY_DEVICE_SIZE_OFFSET_LENGTH 0x20

/**
 * @internal
 * @brief Offset of the attributes, the bits of them which the specification
 * reserves up to SMBIOS 3.10 and from it, and the flag of a device disabled
 * because of an error, which SMBIOS 3.10 defines.
 */
#define DMI_MEMORY_DEVICE_ATTRIBUTES_OFFSET 0x1B
#define DMI_MEMORY_DEVICE_ATTRIBUTES_RESERVED 0xF0u
#define DMI_MEMORY_DEVICE_ATTRIBUTES_RESERVED_3_10 0x80u
#define DMI_MEMORY_DEVICE_ATTRIBUTES_DISABLED 0x20u

//
// Attributes are read the way the version the data is checked against
// defines them, so they are taken from the data rather than from the members
// decoded by the version of the entry point.
//
static bool dmi_memory_device_lint_get_attributes(const dmi_entity_t *entity, dmi_byte_t *value)
{
    dmi_reader_t reader;

    if (not dmi_reader_initialize(&reader, dmi_entity_buffer(entity),
                                  dmi_entity_offset(entity), entity->body_length))
        return false;

    return dmi_reader_get_bytes_at(&reader, value, DMI_MEMORY_DEVICE_ATTRIBUTES_OFFSET, sizeof(*value));
}

void dmi_memory_device_lint_width(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_memory_device_t *info = dmi_entity_info(entity, DMI_TYPE(memory_device));

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
    const dmi_memory_device_t *info = dmi_entity_info(entity, DMI_TYPE(memory_device));

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
    const dmi_memory_device_t *info = dmi_entity_info(entity, DMI_TYPE(memory_device));

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
    const dmi_memory_device_t *info = dmi_entity_info(entity, DMI_TYPE(memory_device));

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

    if (not dmi_reader_initialize(&reader, dmi_entity_buffer(entity),
                                  dmi_entity_offset(entity), entity->body_length))
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

void dmi_memory_device_lint_attributes(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_byte_t value;

    if (not dmi_memory_device_lint_get_attributes(entity, &value))
        return;

    dmi_byte_t reserved = (dmi_lint_version(lint) >= DMI_VERSION(3, 10, 0))
            ? DMI_MEMORY_DEVICE_ATTRIBUTES_RESERVED_3_10
            : DMI_MEMORY_DEVICE_ATTRIBUTES_RESERVED;

    if ((value & reserved) == 0)
        return;

    dmi_lint_issue(lint, entity, nullptr,
                   dmi_lint_entity_offset(lint, entity) + DMI_MEMORY_DEVICE_ATTRIBUTES_OFFSET,
                   "reserved bits 0x%02X of the attributes are set", value & reserved);
}

void dmi_memory_device_lint_disabled(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_memory_device_t *info = dmi_entity_info(entity, DMI_TYPE(memory_device));
    dmi_byte_t value;

    if ((info == nullptr) or (dmi_lint_version(lint) < DMI_VERSION(3, 10, 0)))
        return;

    if (not dmi_memory_device_lint_get_attributes(entity, &value) or
        not (value & DMI_MEMORY_DEVICE_ATTRIBUTES_DISABLED))
        return;

    // Error information is optional, while the handle telling that no error
    // has been detected, or an error of no kind, contradicts the flag
    if (info->error_info_handle == DMI_HANDLE_INVALID) {
        dmi_lint_issue(lint, entity, "error-info-handle", dmi_lint_entity_offset(lint, entity),
                       "device is disabled because of an error, while no error "
                       "has been detected in it");
        return;
    }

    const dmi_entity_t *error = info->error_info;
    dmi_memory_error_type_t type;

    if (error == nullptr)
        return;

    if (dmi_entity_type(error) == DMI_TYPE(memory_error_32)) {
        const dmi_memory_error_32_t *error_info = dmi_entity_info(error, DMI_TYPE(memory_error_32));

        if (error_info == nullptr)
            return;

        type = error_info->type;
    } else if (dmi_entity_type(error) == DMI_TYPE(memory_error_64)) {
        const dmi_memory_error_64_t *error_info = dmi_entity_info(error, DMI_TYPE(memory_error_64));

        if (error_info == nullptr)
            return;

        type = error_info->type;
    } else {
        return;
    }

    // Error of the OK kind describes a device which is healthy, though it may
    // have been unmapped, so it is no error a device is disabled because of
    if (type != DMI_MEMORY_ERROR_TYPE_OK)
        return;

    dmi_lint_issue(lint, entity, "error-info-handle", dmi_lint_entity_offset(lint, entity),
                   "device is disabled because of an error, while its error "
                   "information at handle 0x%04X reports none", info->error_info_handle);
}
