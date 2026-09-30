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

typedef struct dmi_intel_amt dmi_intel_amt_t;

/**
 * @brief Intel Active Management Technology information (type 130).
 *
 * Tells whether the platform supports Intel Active Management Technology
 * (AMT), and which of its features are enabled. The structure is told by the
 * `$AMT` signature.
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
     * @brief Whether IDE redirection (IDE-R) is enabled.
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
     * @brief Marker of the extended data, `0xA5`.
     */
    uint8_t extended_data;

    /**
     * @brief Capabilities the platform vendor gives to AMT, four bytes,
     * whose meaning is not established.
     */
    uint8_t oem_capabilities[4];

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

#endif // !OPENDMI_ENTITY_INTEL_AMT_H
