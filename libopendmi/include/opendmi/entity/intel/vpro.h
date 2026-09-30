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
 * @brief Intel vPro information (type 131).
 *
 * The Intel reference code describes the parts of the platform Intel vPro
 * technology relies on: the Management Engine firmware and its BIOS
 * extension, and the PCI functions of the chipset and of the network
 * controller the Management Engine uses. The structure is told by the `vPro`
 * signature, since some vendors give type 131 to structures of their own in
 * the same table.
 *
 * PCI functions are given as their device and function numbers on bus 0,
 * `(device << 3) | function`, e.g. `0xF8` for 0:1F.0.
 */
struct dmi_intel_vpro
{
    /**
     * @brief Capability flags, whose meaning is not established.
     */
    uint32_t flags_1;

    /**
     * @brief Version of the Management Engine BIOS extension (MEBx), all
     * zeroes if not reported.
     */
    dmi_intel_vpro_version_t mebx_version;

    /**
     * @brief PCI device and function of the LPC bridge of the chipset.
     */
    uint16_t lpc_devfn;

    /**
     * @brief PCI device ID of the LPC bridge, which tells the chipset.
     */
    uint16_t lpc_device_id;

    /**
     * @brief Capability flags of the Management Engine, whose meaning is not
     * established.
     */
    uint32_t flags_2;

    /**
     * @brief Version of the Management Engine firmware, all zeroes if not
     * reported.
     */
    dmi_intel_vpro_version_t me_version;

    /**
     * @brief PCI device and function of the Gigabit Ethernet controller the
     * Management Engine uses.
     */
    uint16_t gbe_devfn;

    /**
     * @brief PCI device ID of the Gigabit Ethernet controller, `0xFFFF` if it
     * is absent.
     */
    uint16_t gbe_device_id;

    /**
     * @brief Feature flags, whose meaning is not established.
     */
    uint32_t flags_3;

    /**
     * @brief Signature, `vPro`, which the structure is told by.
     */
    dmi_binary_t signature;
};

/**
 * @brief Intel vPro information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_intel_vpro_spec;

#endif // !OPENDMI_ENTITY_INTEL_VPRO_H
