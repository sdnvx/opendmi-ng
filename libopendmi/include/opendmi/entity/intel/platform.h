//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INTEL_PLATFORM_H
#define OPENDMI_ENTITY_INTEL_PLATFORM_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_intel_platform dmi_intel_platform_t;

/**
 * @brief Intel platform information (type 148).
 *
 * The reference code of Intel Bay Trail platforms gives the versions of the
 * firmware components and some settings of the platform, all of them as
 * strings, as the firmware writes them. The structure is told by its length,
 * since Intel server boards give type 148 to structures of other layouts.
 */
struct dmi_intel_platform
{
    /**
     * @brief Version of the graphics output protocol (GOP) driver, e.g. `7.2.1013`.
     */
    const char *gop_version;

    /**
     * @brief Revision of the microcode in hexadecimal, e.g. `832`.
     */
    const char *microcode_version;

    /**
     * @brief Version of the memory reference code (MRC), e.g. `1.02`.
     */
    const char *mrc_version;

    /**
     * @brief Version of the firmware of the security engine, the Trusted Execution Engine (TXE), e.g. `1.2.0.1149`.
     */
    const char *sec_version;

    /**
     * @brief Version of the firmware of the ultra low power microcontroller (ULPMC), or `Non ULPMC!!` if there is none.
     */
    const char *ulpmc_version;

    /**
     * @brief Version of the firmware of the power management controller (PMC), e.g. `0x4_45`.
     */
    const char *pmc_version;

    /**
     * @brief Version of the firmware of the P-unit, e.g. `0x27`.
     */
    const char *punit_version;

    /**
     * @brief Stepping of the system on a chip, e.g. `0F (C0 Stepping)`.
     */
    const char *soc_version;

    /**
     * @brief Board, e.g. `BAY LAKE CR (6)`.
     */
    const char *board_version;

    /**
     * @brief Fab of the board, e.g. `0`.
     */
    const char *fab_version;

    /**
     * @brief Flavor of the processor, e.g. `VLV-QC Notebook (3)`.
     */
    const char *cpu_flavor;

    /**
     * @brief Version of the firmware, e.g. `E2CN13WW`.
     */
    const char *bios_version;

    /**
     * @brief Vendor and revision of the power management IC (PMIC), e.g. `41.01`.
     */
    const char *pmic_version;

    /**
     * @brief Version of the firmware of the touch screen, `NA` if there is none.
     */
    const char *touch_version;

    /**
     * @brief Secure Boot setting, `0` or `1`.
     */
    const char *secure_boot;

    /**
     * @brief Boot mode of the firmware, as a number.
     */
    const char *boot_mode;

    /**
     * @brief Enhanced Intel SpeedStep setting.
     */
    const char *speedstep_mode;

    /**
     * @brief Turbo Boost setting.
     */
    const char *turbo_mode;

    /**
     * @brief Deepest C-state of the processor, e.g. `3`.
     */
    const char *max_cstate;

    /**
     * @brief Turbo setting of the graphics.
     */
    const char *gfx_turbo;

    /**
     * @brief S0ix setting.
     */
    const char *idle_reserve;

    /**
     * @brief RC6 power saving setting of the graphics.
     */
    const char *rc6;
};

/**
 * @brief Intel platform information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_intel_platform_spec;

#endif // !OPENDMI_ENTITY_INTEL_PLATFORM_H
