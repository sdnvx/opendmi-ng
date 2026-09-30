//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INTEL_VPRO_H
#define OPENDMI_ENTITY_INTEL_VPRO_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_intel_vpro         dmi_intel_vpro_t;
typedef struct dmi_intel_vpro_version dmi_intel_vpro_version_t;

typedef union dmi_intel_vpro_cpu_caps  dmi_intel_vpro_cpu_caps_t;
typedef union dmi_intel_vpro_mch_caps  dmi_intel_vpro_mch_caps_t;
typedef union dmi_intel_vpro_me_caps   dmi_intel_vpro_me_caps_t;
typedef union dmi_intel_vpro_tpm_caps  dmi_intel_vpro_tpm_caps_t;
typedef union dmi_intel_vpro_bios_caps dmi_intel_vpro_bios_caps_t;

/**
 * @brief Version of a firmware component, as Intel vPro information gives it.
 */
struct dmi_intel_vpro_version
{
    /**
     * @brief Major version.
     */
    uint16_t major;

    /**
     * @brief Minor version.
     */
    uint16_t minor;

    /**
     * @brief Hotfix number.
     */
    uint16_t hotfix;

    /**
     * @brief Build number.
     */
    uint16_t build;
};

/**
 * @brief Capabilities of the processor.
 */
dmi_packed_union(dmi_intel_vpro_cpu_caps)
{
    /**
     * @brief Raw value.
     */
    dmi_dword_t __value;

    dmi_packed_struct()
    {
        /**
         * @brief Whether the virtual machine extensions (VMX) are enabled.
         */
        dmi_dword_t is_vmx_enabled : 1;

        /**
         * @brief Whether the safer mode extensions (SMX) are enabled.
         */
        dmi_dword_t is_smx_enabled : 1;

        /**
         * @brief Whether the processor supports Trusted Execution Technology
         * (TXT).
         */
        dmi_dword_t is_txt_capable : 1;

        /**
         * @brief Whether Trusted Execution Technology is enabled.
         */
        dmi_dword_t is_txt_enabled : 1;

        /**
         * @brief Whether the processor supports Virtualization Technology
         * (VT-x).
         */
        dmi_dword_t is_vtx_capable : 1;

        /**
         * @brief Whether Virtualization Technology is enabled.
         */
        dmi_dword_t is_vtx_enabled : 1;

        /**
         * @brief Reserved, set to 0.
         */
        dmi_dword_t __reserved : 26;
    };
};

dmi_static_assert_value_union(dmi_intel_vpro_cpu_caps);

/**
 * @brief Capabilities of the memory controller hub, which the older layout
 * of the structure holds.
 */
dmi_packed_union(dmi_intel_vpro_mch_caps)
{
    /**
     * @brief Raw value.
     */
    dmi_dword_t __value;

    dmi_packed_struct()
    {
        /**
         * @brief Whether the chipset supports Virtualization Technology for
         * Directed I/O (VT-d).
         */
        dmi_dword_t is_vtd_capable : 1;

        /**
         * @brief Whether VT-d is enabled.
         */
        dmi_dword_t is_vtd_enabled : 1;

        /**
         * @brief Whether the chipset supports Trusted Execution Technology.
         */
        dmi_dword_t is_txt_capable : 1;

        /**
         * @brief Whether Trusted Execution Technology is enabled.
         */
        dmi_dword_t is_txt_enabled : 1;

        /**
         * @brief Reserved, set to 0.
         */
        dmi_dword_t __reserved : 28;
    };
};

dmi_static_assert_value_union(dmi_intel_vpro_mch_caps);

/**
 * @brief Capabilities of the Management Engine firmware.
 */
dmi_packed_union(dmi_intel_vpro_me_caps)
{
    /**
     * @brief Raw value.
     */
    dmi_dword_t __value;

    dmi_packed_struct()
    {
        /**
         * @brief Whether the Management Engine is enabled.
         */
        dmi_dword_t is_me_enabled : 1;

        /**
         * @brief Whether the firmware supports Quiet System Technology (QST).
         */
        dmi_dword_t is_qst_supported : 1;

        /**
         * @brief Whether the firmware supports Alert Standard Format (ASF).
         */
        dmi_dword_t is_asf_supported : 1;

        /**
         * @brief Whether the firmware supports Active Management Technology
         * (AMT).
         */
        dmi_dword_t is_amt_supported : 1;

        /**
         * @brief Reserved, set to 0.
         */
        dmi_dword_t __reserved_1 : 1;

        /**
         * @brief Whether the firmware supports Small Business Technology
         * (SBT).
         */
        dmi_dword_t is_sbt_supported : 1;

        /**
         * @brief Whether the firmware supports the level III manageability.
         */
        dmi_dword_t is_l3_supported : 1;

        /**
         * @brief Bits which Intel reserves, while the newer firmware sets
         * some of them, e.g. bits 13 to 15, whose meaning is not established.
         */
        dmi_dword_t __reserved_2 : 25;
    };
};

dmi_static_assert_value_union(dmi_intel_vpro_me_caps);

/**
 * @brief Capabilities of the Trusted Platform Module.
 */
dmi_packed_union(dmi_intel_vpro_tpm_caps)
{
    /**
     * @brief Raw value.
     */
    dmi_dword_t __value;

    dmi_packed_struct()
    {
        /**
         * @brief Whether a TPM is on board.
         */
        dmi_dword_t is_tpm_present : 1;

        /**
         * @brief Whether the TPM is enabled.
         */
        dmi_dword_t is_tpm_enabled : 1;

        /**
         * @brief Reserved, set to 0.
         */
        dmi_dword_t __reserved : 14;

        /**
         * @brief Major version of the TCG specification the TPM is designed
         * to.
         */
        dmi_dword_t tcg_major : 8;

        /**
         * @brief Minor version of the TCG specification the TPM is designed
         * to.
         */
        dmi_dword_t tcg_minor : 8;
    };
};

dmi_static_assert_value_union(dmi_intel_vpro_tpm_caps);

/**
 * @brief Capabilities of the BIOS.
 */
dmi_packed_union(dmi_intel_vpro_bios_caps)
{
    /**
     * @brief Raw value.
     */
    dmi_dword_t __value;

    dmi_packed_struct()
    {
        /**
         * @brief Whether VT-x can be set in the setup of the BIOS.
         */
        dmi_dword_t is_vtx_configurable : 1;

        /**
         * @brief Whether VT-d can be set in the setup of the BIOS.
         */
        dmi_dword_t is_vtd_configurable : 1;

        /**
         * @brief Whether Trusted Execution Technology can be set in the setup
         * of the BIOS.
         */
        dmi_dword_t is_txt_configurable : 1;

        /**
         * @brief Whether the TPM can be set in the setup of the BIOS.
         */
        dmi_dword_t is_tpm_configurable : 1;

        /**
         * @brief Whether the Management Engine can be set in the setup of the
         * BIOS.
         */
        dmi_dword_t is_me_configurable : 1;

        /**
         * @brief Whether the BIOS supports the extensions of the Virtual
         * Appliance, the ACPI operation region.
         */
        dmi_dword_t is_va_supported : 1;

        /**
         * @brief Whether the platform data area of the SPI flash is reserved.
         */
        dmi_dword_t is_spi_data_reserved : 1;

        /**
         * @brief Highest version of the Virtual Appliance supported: 0 for
         * 2.6, 1 for 3.0.
         */
        dmi_dword_t va_version : 3;

        /**
         * @brief Reserved, set to 0.
         */
        dmi_dword_t __reserved : 22;
    };
};

dmi_static_assert_value_union(dmi_intel_vpro_bios_caps);

/**
 * @brief Intel vPro information (type 131).
 *
 * Intel calls the structure the vPro verification table: it describes the
 * parts of the platform Intel vPro technology relies on, i.e. the
 * capabilities of the processor, of the chipset, of the Management Engine
 * firmware, of the TPM and of the BIOS, and the PCI functions of the chipset
 * and of the network controller the Management Engine uses. The structure is
 * told by the `vPro` signature, since some vendors give type 131 to
 * structures of their own in the same table.
 *
 * PCI functions are given as their bus, and their device and function
 * numbers, `(device << 3) | function`, e.g. `0xF8` for 0:1F.0.
 */
struct dmi_intel_vpro
{
    /**
     * @brief Capabilities of the processor.
     */
    dmi_intel_vpro_cpu_caps_t cpu_capabilities;

    /**
     * @brief Version of the Management Engine BIOS extension (MEBx), all
     * zeroes if not reported. The older layout holds the capabilities of the
     * memory controller hub in its place.
     */
    dmi_intel_vpro_version_t mebx_version;

    /**
     * @brief PCI device and function of the LPC bridge of the chipset.
     */
    uint8_t lpc_devfn;

    /**
     * @brief PCI bus of the LPC bridge.
     */
    uint8_t lpc_bus;

    /**
     * @brief PCI device ID of the LPC bridge, which tells the chipset.
     */
    uint16_t lpc_device_id;

    /**
     * @brief Capabilities of the Management Engine firmware.
     */
    dmi_intel_vpro_me_caps_t me_capabilities;

    /**
     * @brief Version of the Management Engine firmware, all zeroes if not
     * reported.
     */
    dmi_intel_vpro_version_t me_version;

    /**
     * @brief Capabilities of the Trusted Platform Module.
     */
    dmi_intel_vpro_tpm_caps_t tpm_capabilities;

    /**
     * @brief PCI device and function of the wired network controller the
     * Management Engine uses.
     */
    uint8_t gbe_devfn;

    /**
     * @brief PCI bus of the wired network controller.
     */
    uint8_t gbe_bus;

    /**
     * @brief PCI device ID of the wired network controller, `0xFFFF` if it
     * is absent.
     */
    uint16_t gbe_device_id;

    /**
     * @brief Capabilities of the BIOS.
     */
    dmi_intel_vpro_bios_caps_t bios_capabilities;

    /**
     * @brief Signature, `vPro`, which the structure is told by.
     */
    dmi_binary_t signature;

    /**
     * @brief Major version of the TCG specification the TPM is designed to,
     * taken from `tpm_capabilities`.
     */
    uint8_t tcg_major;

    /**
     * @brief Minor version of the TCG specification the TPM is designed to,
     * taken from `tpm_capabilities`.
     */
    uint8_t tcg_minor;

    /**
     * @brief Highest version of the Virtual Appliance the BIOS supports,
     * taken from `bios_capabilities`: 0 for 2.6, 1 for 3.0.
     */
    uint8_t va_version;

    /**
     * @brief Whether the structure is of the older layout, which holds the
     * capabilities of the memory controller hub in place of the version of
     * the BIOS extension. The version is not shown then, and the fields of
     * the hub are.
     */
    bool has_mch_capabilities;

    /**
     * @brief PCI device and function of the memory controller hub, older
     * layout only.
     */
    uint8_t mch_devfn;

    /**
     * @brief PCI bus of the memory controller hub, older layout only.
     */
    uint8_t mch_bus;

    /**
     * @brief PCI device ID of the memory controller hub, older layout only.
     */
    uint16_t mch_device_id;

    /**
     * @brief Capabilities of the memory controller hub, older layout only.
     */
    dmi_intel_vpro_mch_caps_t mch_capabilities;
};

/**
 * @brief Intel vPro information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_intel_vpro_spec;

#endif // !OPENDMI_ENTITY_INTEL_VPRO_H
