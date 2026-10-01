//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INTEL_AMT_H
#define OPENDMI_ENTITY_INTEL_AMT_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_INTEL_AMT_T
#   define DMI_INTEL_AMT_T
    typedef struct dmi_intel_amt dmi_intel_amt_t;
#endif // !DMI_INTEL_AMT_T

#ifndef DMI_INTEL_AMT_OEM_CAPS_1_T
#   define DMI_INTEL_AMT_OEM_CAPS_1_T
    typedef union dmi_intel_amt_oem_caps_1 dmi_intel_amt_oem_caps_1_t;
#endif // !DMI_INTEL_AMT_OEM_CAPS_1_T

#ifndef DMI_INTEL_AMT_OEM_CAPS_3_T
#   define DMI_INTEL_AMT_OEM_CAPS_3_T
    typedef union dmi_intel_amt_oem_caps_3 dmi_intel_amt_oem_caps_3_t;
#endif // !DMI_INTEL_AMT_OEM_CAPS_3_T

#ifndef DMI_INTEL_AMT_OEM_CAPS_4_T
#   define DMI_INTEL_AMT_OEM_CAPS_4_T
    typedef union dmi_intel_amt_oem_caps_4 dmi_intel_amt_oem_caps_4_t;
#endif // !DMI_INTEL_AMT_OEM_CAPS_4_T

/**
 * @brief Terminal emulations of Serial over LAN, as the low nibble of the
 * second byte of the OEM capabilities tells them.
 */
typedef enum dmi_intel_amt_terminal
{
    DMI_INTEL_AMT_TERMINAL_UNSPEC     = 0x00, ///< Unspecified
    DMI_INTEL_AMT_TERMINAL_VT52       = 0x01, ///< VT52
    DMI_INTEL_AMT_TERMINAL_VT100_PLUS = 0x02, ///< VT100+
    DMI_INTEL_AMT_TERMINAL_VT_UTF8    = 0x03, ///< VT-UTF8
    DMI_INTEL_AMT_TERMINAL_PC_ANSI    = 0x04  ///< PC-ANSI
} dmi_intel_amt_terminal_t;

/**
 * @brief First byte of the OEM capabilities: the features of AMT the
 * platform supports.
 */
dmi_packed_union(dmi_intel_amt_oem_caps_1)
{
    /**
     * @brief Raw value.
     */
    dmi_byte_t __value;

    dmi_packed_struct()
    {
        /**
         * @brief Whether storage redirection, IDE-R or USB-R, is supported.
         */
        dmi_byte_t is_storage_redirection_supported : 1;

        /**
         * @brief Whether Serial over LAN is supported.
         */
        dmi_byte_t is_sol_supported : 1;

        /**
         * @brief Whether the BIOS can be reflashed remotely.
         */
        dmi_byte_t is_bios_reflash_supported : 1;

        /**
         * @brief Whether the setup of the BIOS can be entered remotely.
         */
        dmi_byte_t is_bios_setup_supported : 1;

        /**
         * @brief Whether the BIOS can be paused remotely.
         */
        dmi_byte_t is_bios_pause_supported : 1;

        /**
         * @brief Whether booting from the floppy disk can be prevented.
         */
        dmi_byte_t is_floppy_boot_blockable : 1;

        /**
         * @brief Whether booting from the CD can be prevented.
         */
        dmi_byte_t is_cd_boot_blockable : 1;

        /**
         * @brief Reserved, while most firmware sets it.
         */
        dmi_byte_t __reserved : 1;
    };
};

dmi_static_assert_value_union(dmi_intel_amt_oem_caps_1);

/**
 * @brief Third byte of the OEM capabilities.
 */
dmi_packed_union(dmi_intel_amt_oem_caps_3)
{
    /**
     * @brief Raw value.
     */
    dmi_byte_t __value;

    dmi_packed_struct()
    {
        /**
         * @brief Reserved, set to 0.
         */
        dmi_byte_t __reserved : 6;

        /**
         * @brief Whether Remote Secure Erase is supported.
         */
        dmi_byte_t is_secure_erase_supported : 1;

        /**
         * @brief Whether the BIOS supports Secure Boot.
         */
        dmi_byte_t is_secure_boot_supported : 1;
    };
};

dmi_static_assert_value_union(dmi_intel_amt_oem_caps_3);

/**
 * @brief Fourth byte of the OEM capabilities: the targets AMT can boot the
 * platform to remotely.
 */
dmi_packed_union(dmi_intel_amt_oem_caps_4)
{
    /**
     * @brief Raw value.
     */
    dmi_byte_t __value;

    dmi_packed_struct()
    {
        /**
         * @brief Whether a Thunderbolt dock is supported.
         */
        dmi_byte_t is_thunderbolt_dock_supported : 1;

        /**
         * @brief Whether booting over HTTPS is supported.
         */
        dmi_byte_t is_https_boot_supported : 1;

        /**
         * @brief Whether booting to the local pre-boot authentication (PBA)
         * is supported.
         */
        dmi_byte_t is_pba_boot_supported : 1;

        /**
         * @brief Whether booting to the Windows recovery environment (WinRE)
         * is supported.
         */
        dmi_byte_t is_winre_boot_supported : 1;

        /**
         * @brief Whether booting without Secure Boot is supported.
         */
        dmi_byte_t is_nonsecure_boot_supported : 1;

        /**
         * @brief Whether the coexistence with Wi-Fi is supported.
         */
        dmi_byte_t is_wifi_coexistence_supported : 1;

        /**
         * @brief Reserved, set to 0.
         */
        dmi_byte_t __reserved : 2;
    };
};

dmi_static_assert_value_union(dmi_intel_amt_oem_caps_4);

/**
 * @brief Intel Active Management Technology information (type 130).
 *
 * Tells whether the platform supports Intel Active Management Technology
 * (AMT), and which of its features are enabled. The structure is told by the
 * `$AMT` signature.
 *
 * The flags of the features are not consistent with each other on every
 * platform, e.g. some give the network as enabled while AMT is not supported.
 */
struct dmi_intel_amt
{
    /**
     * @brief Signature, `$AMT`, which the structure is told by.
     */
    dmi_binary_t signature;

    /**
     * @brief Whether the platform supports AMT.
     */
    bool is_supported;

    /**
     * @brief Whether AMT is enabled.
     */
    bool is_enabled;

    /**
     * @brief Whether storage redirection is enabled: IDE redirection (IDE-R),
     * or USB redirection (USB-R) from AMT 11.0 onwards.
     */
    bool is_ider_enabled;

    /**
     * @brief Whether Serial over LAN (SOL) is enabled.
     */
    bool is_sol_enabled;

    /**
     * @brief Whether the network interface of AMT is enabled.
     */
    bool is_network_enabled;

    /**
     * @brief Extended data, which Intel leaves to the platform vendor, `0xA5`
     * in all known data.
     */
    uint8_t extended_data;

    /**
     * @brief First byte of the OEM capabilities: the features of AMT the
     * platform supports.
     */
    dmi_intel_amt_oem_caps_1_t oem_capabilities_1;

    /**
     * @brief Terminal emulation of Serial over LAN, from the second byte of
     * the OEM capabilities.
     */
    dmi_intel_amt_terminal_t terminal;

    /**
     * @brief Third byte of the OEM capabilities.
     */
    dmi_intel_amt_oem_caps_3_t oem_capabilities_3;

    /**
     * @brief Fourth byte of the OEM capabilities: the targets AMT can boot the
     * platform to remotely.
     */
    dmi_intel_amt_oem_caps_4_t oem_capabilities_4;

    /**
     * @brief Whether keyboard, video and mouse redirection (KVM) is enabled.
     */
    bool is_kvm_enabled;
};

/**
 * @brief Intel Active Management Technology information entity
 * specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_intel_amt_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_intel_amt_terminal_name(dmi_intel_amt_terminal_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_INTEL_AMT_H
