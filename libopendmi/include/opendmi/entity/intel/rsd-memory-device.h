//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DMI_ENTITY_INTEL_RSD_MEMORY_DEVICE_H
#define DMI_ENTITY_INTEL_RSD_MEMORY_DEVICE_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_intel_rsd_memory_device dmi_intel_rsd_memory_device_t;

/**
 * @brief Intel RSD memory types.
 */
typedef enum dmi_intel_rsd_memory_type
{
    DMI_INTEL_RSD_MEMORY_TYPE_DIMM     = 0x00, ///< DIMM
    DMI_INTEL_RSD_MEMORY_TYPE_NVDIMM_N = 0x01, ///< NVDIMM-N (Byte accessible persistent memory)
    DMI_INTEL_RSD_MEMORY_TYPE_NVDIMM_F = 0x02, ///< NVDIMM-F (Block accessible persistent memory)
    DMI_INTEL_RSD_MEMORY_TYPE_NVDIMM_P = 0x03, ///< NVDIMM-P
    __DMI_INTEL_RSD_MEMORY_TYPE_COUNT
} dmi_intel_rsd_memory_type_t;

/**
 * @brief Intel RSD memory media types.
 */
typedef enum dmi_intel_rsd_memory_media
{
    DMI_INTEL_RSD_MEMORY_MEDIA_DRAM        = 0x00, ///< DRAM
    DMI_INTEL_RSD_MEMORY_MEDIA_NAND        = 0x01, ///< NAND
    DMI_INTEL_RSD_MEMORY_MEDIA_PROPRIETARY = 0x02, ///< Proprietary
    __DMI_INTEL_RSD_MEMORY_MEDIA_COUNT
} dmi_intel_rsd_memory_media_t;

/**
 * @brief f Intel RSD memory device extended information (type 197).
 *
 */
struct dmi_intel_rsd_memory_device
{
    /**
     * @brief Handle, or instance number, of the memory device SMBIOS OEM
     * record (type 17) that this extended information represents.
     */
    dmi_handle_t device_handle;

    /**
     * @brief Type of memory.
     */
    dmi_intel_rsd_memory_type_t memory_type;

    /**
     * @brief Type of memory media.
     */
    dmi_intel_rsd_memory_media_t memory_media;

    /**
     * @brief The string that identifies the revision information of the
     * firmware in the memory device.
     */
    const char *firmware_revision;

    /**
     * @brief The string that identifies the API version information that the
     * firmware exposes.
     */
    const char *firmware_api_version;

    /**
     * @brief Maximum thermal design power (TDP) in mWs that this memory
     * device will consume.
     */
    uint32_t maximum_tdp;

    /**
     * @brief The SMBus address that is associated with this specific memory
     * device. The specific SMBus address that must be reported must allow
     * access to the SPD and/or 3DXPoint Firmware Interface. PSME will use
     * this information to pass to the Intel Intelligent Power Node Manager
     * as necessary to gather information from the memory subsystem.
     */
    uint8_t smbus_address;
};

/**
 * @brief Intel RSD memory device extended information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_intel_rsd_memory_device_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_intel_rsd_memory_type_name(dmi_intel_rsd_memory_type_t value);
__dmi_api const char *dmi_intel_rsd_memory_media_name(dmi_intel_rsd_memory_media_t value);

__END_DECLS

#endif // !DMI_ENTITY_INTEL_RSD_MEMORY_DEVICE_H
