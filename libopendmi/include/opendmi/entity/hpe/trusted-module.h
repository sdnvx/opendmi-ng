//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_TRUSTED_MODULE_H
#define OPENDMI_ENTITY_HPE_TRUSTED_MODULE_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_hpe_trusted_module dmi_hpe_trusted_module_t;

/**
 * @brief Presence of a trusted module.
 */
typedef enum dmi_hpe_tm_presence
{
    DMI_HPE_TM_PRESENCE_ABSENT   = 0x00, ///< Not present
    DMI_HPE_TM_PRESENCE_ENABLED  = 0x01, ///< Present and enabled
    DMI_HPE_TM_PRESENCE_DISABLED = 0x02  ///< Present and disabled
} dmi_hpe_tm_presence_t;

/**
 * @brief Reason a trusted module is disabled for.
 */
typedef enum dmi_hpe_tm_disable_reason
{
    DMI_HPE_TM_DISABLE_REASON_UNSPEC = 0x00, ///< Not specified
    DMI_HPE_TM_DISABLE_REASON_USER   = 0x01, ///< Disabled by the user
    DMI_HPE_TM_DISABLE_REASON_ERROR  = 0x02  ///< Error condition
} dmi_hpe_tm_disable_reason_t;

/**
 * @brief Type of a trusted module.
 */
typedef enum dmi_hpe_tm_type
{
    DMI_HPE_TM_TYPE_UNSPEC  = 0x00, ///< Not specified
    DMI_HPE_TM_TYPE_TPM_1_2 = 0x01, ///< TPM 1.2
    DMI_HPE_TM_TYPE_TPM_2_0 = 0x02, ///< TPM 2.0
    DMI_HPE_TM_TYPE_PTT     = 0x03  ///< Intel PTT firmware TPM
} dmi_hpe_tm_type_t;

/**
 * @brief Physical attribute of a trusted module.
 */
typedef enum dmi_hpe_tm_mounting
{
    DMI_HPE_TM_MOUNTING_UNSPEC    = 0x00, ///< Not specified
    DMI_HPE_TM_MOUNTING_OPTIONAL  = 0x01, ///< Pluggable and optional
    DMI_HPE_TM_MOUNTING_STANDARD  = 0x02, ///< Pluggable but standard
    DMI_HPE_TM_MOUNTING_SOLDERED  = 0x03  ///< Soldered down on the system board
} dmi_hpe_tm_mounting_t;

/**
 * @brief FIPS certification of a trusted module.
 */
typedef enum dmi_hpe_tm_fips
{
    DMI_HPE_TM_FIPS_UNSPEC        = 0x00, ///< Not specified
    DMI_HPE_TM_FIPS_NOT_CERTIFIED = 0x01, ///< Not FIPS certified
    DMI_HPE_TM_FIPS_CERTIFIED     = 0x02  ///< FIPS certified
} dmi_hpe_tm_fips_t;

/**
 * @brief Chip of a trusted module.
 */
typedef enum dmi_hpe_tm_chip
{
    DMI_HPE_TM_CHIP_NONE              = 0x00, ///< None
    DMI_HPE_TM_CHIP_STM_GEN10         = 0x01, ///< STMicro TPM of Gen10
    DMI_HPE_TM_CHIP_PTT               = 0x02, ///< Intel firmware TPM (PTT)
    DMI_HPE_TM_CHIP_NATIONZ           = 0x03, ///< Nationz TPM
    DMI_HPE_TM_CHIP_STM_GEN10_PLUS    = 0x04, ///< STMicro TPM of Gen10 Plus
    DMI_HPE_TM_CHIP_STM_GEN11         = 0x05, ///< STMicro TPM of Gen11
    DMI_HPE_TM_CHIP_STM_GEN12         = 0x06  ///< STMicro TPM of Gen12
} dmi_hpe_tm_chip_t;

/**
 * @brief HP/HPE trusted module (TPM or TCM) status (type 224).
 */
struct dmi_hpe_trusted_module
{
    /**
     * @brief Presence of the module.
     */
    dmi_hpe_tm_presence_t presence;

    /**
     * @brief Whether option ROMs are measured.
     */
    bool is_orom_measuring;

    /**
     * @brief Whether the module is hidden.
     */
    bool is_hidden;

    /**
     * @brief Reason the module is disabled for.
     */
    dmi_hpe_tm_disable_reason_t disable_reason;

    /**
     * @brief Error condition of the module, `1` for a self-test failure.
     */
    uint8_t error_condition;

    /**
     * @brief Type of the module.
     */
    dmi_hpe_tm_type_t type;

    /**
     * @brief Whether the standard algorithms are supported.
     */
    bool is_standard_algorithm;

    /**
     * @brief Whether the Chinese algorithms are supported.
     */
    bool is_chinese_algorithm;

    /**
     * @brief How the module is mounted.
     */
    dmi_hpe_tm_mounting_t mounting;

    /**
     * @brief FIPS certification of the module.
     */
    dmi_hpe_tm_fips_t fips;

    /**
     * @brief Handle of the version indicator (type 216) of the firmware of
     * the module. Set to `DMI_HANDLE_INVALID` when the structure holds none.
     */
    dmi_handle_t version_handle;

    /**
     * @brief Chip of the module.
     */
    dmi_hpe_tm_chip_t chip;
};

/**
 * @brief HP/HPE trusted module status entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_trusted_module_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_hpe_tm_presence_name(dmi_hpe_tm_presence_t value);
__dmi_api const char *dmi_hpe_tm_disable_reason_name(dmi_hpe_tm_disable_reason_t value);
__dmi_api const char *dmi_hpe_tm_type_name(dmi_hpe_tm_type_t value);
__dmi_api const char *dmi_hpe_tm_mounting_name(dmi_hpe_tm_mounting_t value);
__dmi_api const char *dmi_hpe_tm_fips_name(dmi_hpe_tm_fips_t value);
__dmi_api const char *dmi_hpe_tm_chip_name(dmi_hpe_tm_chip_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_HPE_TRUSTED_MODULE_H
