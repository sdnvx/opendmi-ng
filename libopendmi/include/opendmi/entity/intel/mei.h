//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INTEL_MEI_H
#define OPENDMI_ENTITY_INTEL_MEI_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_intel_mei        dmi_intel_mei_t;
typedef struct dmi_intel_mei_device dmi_intel_mei_device_t;

/**
 * @brief Working states of the Management Engine firmware, as bits 0 to 3 of
 * the first firmware status register tell them.
 */
typedef enum dmi_intel_me_state
{
    DMI_INTEL_ME_STATE_RESET        = 0x00, ///< Reset
    DMI_INTEL_ME_STATE_INIT         = 0x01, ///< Initializing
    DMI_INTEL_ME_STATE_RECOVERY     = 0x02, ///< Recovery
    DMI_INTEL_ME_STATE_TEST         = 0x03, ///< Test
    DMI_INTEL_ME_STATE_M3_NO_UMA    = 0x04, ///< M3 without UMA
    DMI_INTEL_ME_STATE_NORMAL       = 0x05, ///< Normal
    DMI_INTEL_ME_STATE_WAIT         = 0x06, ///< Waiting
    DMI_INTEL_ME_STATE_TRANSITION   = 0x07, ///< Transition
    DMI_INTEL_ME_STATE_INVALID_CPU  = 0x08, ///< Invalid CPU plugged in
    DMI_INTEL_ME_STATE_UNSPEC       = 0xFF  ///< Unspecified
} dmi_intel_me_state_t;

/**
 * @brief Error codes of the Management Engine firmware, as bits 12 to 15 of
 * the first firmware status register tell them.
 */
typedef enum dmi_intel_me_error
{
    DMI_INTEL_ME_ERROR_NONE          = 0x00, ///< No error
    DMI_INTEL_ME_ERROR_UNCATEGORIZED = 0x01, ///< Uncategorized failure
    DMI_INTEL_ME_ERROR_IMAGE         = 0x03, ///< Image failure
    DMI_INTEL_ME_ERROR_DEBUG         = 0x04, ///< Debug failure
    DMI_INTEL_ME_ERROR_UNSPEC        = 0xFF  ///< Unspecified
} dmi_intel_me_error_t;

/**
 * @brief Operation modes of the Management Engine firmware, as bits 16 to 19
 * of the first firmware status register tell them.
 */
typedef enum dmi_intel_me_mode
{
    DMI_INTEL_ME_MODE_NORMAL           = 0x00, ///< Normal
    DMI_INTEL_ME_MODE_DEBUG            = 0x02, ///< Debug
    DMI_INTEL_ME_MODE_DISABLED         = 0x03, ///< Temporarily disabled
    DMI_INTEL_ME_MODE_OVERRIDE_JUMPER  = 0x04, ///< Security override by jumper
    DMI_INTEL_ME_MODE_OVERRIDE_MESSAGE = 0x05, ///< Security override by message
    DMI_INTEL_ME_MODE_UNSPEC           = 0xFF  ///< Unspecified
} dmi_intel_me_mode_t;

/**
 * @brief Management Engine firmware SKUs, as bits 4 to 6 of the third firmware
 * status register tell them.
 */
typedef enum dmi_intel_me_sku
{
    DMI_INTEL_ME_SKU_CONSUMER  = 0x02, ///< Consumer
    DMI_INTEL_ME_SKU_CORPORATE = 0x03, ///< Corporate
    DMI_INTEL_ME_SKU_LITE      = 0x05, ///< Lite
    DMI_INTEL_ME_SKU_UNSPEC    = 0xFF  ///< Unspecified
} dmi_intel_me_sku_t;

/**
 * @brief Host Embedded Controller Interface (HECI) of the Management Engine.
 */
struct dmi_intel_mei_device
{
    /**
     * @brief Name of the interface, `MEI1` to `MEI4` for the PCI functions
     * 0:16.0, 0:16.1, 0:16.4 and 0:16.5.
     */
    const char *name;

    /**
     * @brief Firmware status registers HFSTS1 to HFSTS6, as the PCI
     * configuration space of the function holds them at offsets 0x40, 0x48,
     * 0x60, 0x64, 0x68 and 0x6C. All bits are set if the function is absent,
     * and all are zero if the interface is not reported.
     *
     * The meaning of the registers depends on the interface, and on the
     * generation of the Management Engine for all but the first and the third
     * registers of the first interface: the first one tells the working state,
     * the operation mode and the error code, and the third one the firmware
     * SKU.
     */
    uint32_t hfsts[6];

    /**
     * @brief Whether the function is present, which the registers of all
     * bits set tell it is not. Not shown if the interface is not reported.
     */
    bool is_present;

    /**
     * @brief Whether the interface is reported, which the registers of all
     * zeroes tell it is not. The registers are not shown then.
     */
    bool is_reported;
};

/**
 * @brief Intel Management Engine interface information (type 219).
 *
 * The Intel reference code keeps the firmware status registers of the
 * interfaces of the Management Engine, as the firmware has read them at boot.
 * The state of the Management Engine firmware is told from the registers of
 * the first interface, which is the one the host talks to the firmware
 * through.
 */
struct dmi_intel_mei
{
    /**
     * @brief Version of the structure.
     */
    uint8_t version;

    /**
     * @brief Number of interfaces.
     */
    size_t device_count;

    /**
     * @brief Interfaces.
     */
    dmi_intel_mei_device_t *devices;

    /**
     * @brief Working state of the firmware. `DMI_INTEL_ME_STATE_UNSPEC` if the
     * first interface is absent or not reported, as are the other fields the
     * first register tells.
     */
    dmi_intel_me_state_t state;

    /**
     * @brief Operation mode of the firmware. `DMI_INTEL_ME_MODE_UNSPEC` if the
     * first interface is absent.
     */
    dmi_intel_me_mode_t mode;

    /**
     * @brief Error code of the firmware. `DMI_INTEL_ME_ERROR_UNSPEC` if the
     * first interface is absent.
     */
    dmi_intel_me_error_t error_code;

    /**
     * @brief Whether the firmware has completed its initialization.
     */
    bool is_init_complete;

    /**
     * @brief Whether the firmware is in the manufacturing mode, in which the
     * platform is not locked down yet.
     */
    bool is_manufacturing;

    /**
     * @brief SKU of the firmware. `DMI_INTEL_ME_SKU_UNSPEC` if the first
     * interface is absent, or if the register holds zero for the SKU, as the
     * firmware of Management Engine 11.0 may.
     */
    dmi_intel_me_sku_t sku;
};

/**
 * @brief Intel Management Engine interface information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_intel_mei_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_intel_me_state_name(dmi_intel_me_state_t value);
__dmi_api const char *dmi_intel_me_error_name(dmi_intel_me_error_t value);
__dmi_api const char *dmi_intel_me_mode_name(dmi_intel_me_mode_t value);
__dmi_api const char *dmi_intel_me_sku_name(dmi_intel_me_sku_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_INTEL_MEI_H
