//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_DIMM_CONFIG_H
#define OPENDMI_ENTITY_HPE_DIMM_CONFIG_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_HPE_DIMM_CONFIG_T
#   define DMI_HPE_DIMM_CONFIG_T
    typedef struct dmi_hpe_dimm_config dmi_hpe_dimm_config_t;
#endif // !DMI_HPE_DIMM_CONFIG_T

/**
 * @brief Health of an interleave set.
 */
typedef enum dmi_hpe_interleave_health
{
    DMI_HPE_INTERLEAVE_HEALTH_HEALTHY        = 0x00, ///< Healthy
    DMI_HPE_INTERLEAVE_HEALTH_DIMM_MISSING   = 0x01, ///< DIMM missing
    DMI_HPE_INTERLEAVE_HEALTH_INACTIVE       = 0x02, ///< Configuration inactive
    DMI_HPE_INTERLEAVE_HEALTH_SPA_MISSING    = 0x03, ///< SPA missing
    DMI_HPE_INTERLEAVE_HEALTH_NEW_GOAL       = 0x04, ///< New goal
    DMI_HPE_INTERLEAVE_HEALTH_LOCKED         = 0x05, ///< Locked
    // Reserved: 0x06 .. 0xFF

    /**
     * @brief Health of a structure too short to hold it, which is no value
     * the field may hold.
     */
    DMI_HPE_INTERLEAVE_HEALTH_ABSENT         = 0x100
} dmi_hpe_interleave_health_t;

/**
 * @brief HP/HPE DIMM current configuration record (type 244).
 *
 * Describes a memory region configured on a module of a memory device
 * (type 17), e.g. of a persistent memory one, and the interleave set it
 * belongs to. There is a record for each region.
 */
struct dmi_hpe_dimm_config
{
    /**
     * @brief Handle of the memory device structure (type 17).
     */
    dmi_handle_t device_handle;

    /**
     * @brief ID of the region, unique on the module.
     */
    uint8_t region_id;

    /**
     * @brief Whether the region is volatile, its data lost on a power cycle.
     */
    bool is_volatile;

    /**
     * @brief Whether the region is byte accessible persistent memory, its
     * data kept through a reset.
     */
    bool is_byte_accessible;

    /**
     * @brief Whether the region is block I/O persistent memory.
     */
    bool is_block_io;

    /**
     * @brief Size of the region in mebibytes, as the structure holds it.
     */
    uint64_t raw_size;

    /**
     * @brief Size of the region in bytes.
     */
    uint64_t size;

    /**
     * @brief State of the passphrase, as the structure holds it.
     */
    uint8_t passphrase_state;

    /**
     * @brief Whether a passphrase is required, byte accessible persistent
     * regions only: any state other than zero.
     */
    bool is_passphrase_enabled;

    /**
     * @brief 1-based index of the interleave set of the region, unique per
     * set. Regions of the same index are interleaved.
     */
    uint16_t interleave_set;

    /**
     * @brief Number of the DIMMs of the interleave set.
     */
    uint8_t interleave_dimm_count;

    /**
     * @brief Health of the interleave set, or
     * `DMI_HPE_INTERLEAVE_HEALTH_ABSENT` if the structure is too short to
     * hold it.
     */
    dmi_hpe_interleave_health_t interleave_health;
};

/**
 * @brief HP/HPE DIMM current configuration record entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_dimm_config_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_hpe_interleave_health_name(dmi_hpe_interleave_health_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_HPE_DIMM_CONFIG_H
