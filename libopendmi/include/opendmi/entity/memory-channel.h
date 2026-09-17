//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_CHANNEL_H
#define OPENDMI_ENTITY_MEMORY_CHANNEL_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_memory_channel        dmi_memory_channel_t;
typedef struct dmi_memory_channel_device dmi_memory_channel_device_t;

/**
 * @brief Memory channel types.
 */
typedef enum dmi_memory_channel_type
{
    DMI_MEMORY_CHANNEL_TYPE_UNSPEC   = 0x00, ///< Unspecified
    DMI_MEMORY_CHANNEL_TYPE_OTHER    = 0x01, ///< Other
    DMI_MEMORY_CHANNEL_TYPE_UNKNOWN  = 0x02, ///< Unknown
    DMI_MEMORY_CHANNEL_TYPE_RAMBUS   = 0x03, ///< RamBus
    DMI_MEMORY_CHANNEL_TYPE_SYNCLINK = 0x04, ///< SyncLink
    __DMI_MEMORY_CHANNEL_TYPE_COUNT
} dmi_memory_channel_type_t;

/**
 * @brief Memory channel device.
 */
struct dmi_memory_channel_device
{
    /**
     * @brief Channel load provided by the memory device associated with
     * this channel.
     */
    dmi_byte_t load;

    /**
     * @brief Structure handle that identifies the memory device associated
     * with this channel.
     */
    dmi_handle_t handle;

    /**
     * @brief Reference to the structure that identifies the memory device
     * associated with this channel.
     */
    dmi_entity_t *device;
};

/**
 * @brief Memory channel structure (type 37).
 *
 * The information in this structure provides the correlation between a memory
 * channel and its associated memory devices. Each device presents one or more
 * loads to the channel. The sum of all device loads cannot exceed the
 * channel's defined maximum.
 *
 * @since SMBIOS 2.3
 */
struct dmi_memory_channel
{
    /**
     * @brief Type of memory associated with the channel.
     */
    dmi_memory_channel_type_t type;

    /**
     * @brief Maximum load supported by the channel. The sum of all device
     * loads cannot exceed this value.
     */
    unsigned short maximum_load;

    /**
     * @brief Number of memory devices (type 11h) that are associated with this
     * channel. This value also defines the number of load/handle pairs that
     * follow.
     */
    size_t device_count;

    /**
     * @brief Channel loads provided by memory devices associated with this
     * channel.
     */
    dmi_memory_channel_device_t *devices;
};

/**
 * @brief Memory channel entity specification.
 */
extern const dmi_entity_spec_t dmi_memory_channel_spec;

__BEGIN_DECLS

/**
 * @brief Get memory channel type name.
 *
 * Returns the human-readable name of the memory channel type.
 *
 * @param[in] value Memory channel type value.
 *
 * @return The memory channel type name string, or @c nullptr if @p value is out of
 * range.
 */
const char *dmi_memory_channel_type_name(dmi_memory_channel_type_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_MEMORY_CHANNEL_H
