//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_ONBOARD_DEVICE_EX_H
#define OPENDMI_ENTITY_ONBOARD_DEVICE_EX_H

#pragma once

#include <opendmi/entity/common.h>
#include <opendmi/entity/onboard-device.h>

typedef struct dmi_onboard_device_ex dmi_onboard_device_ex_t;

/**
 * @brief Onboard devices extended information structure (type 41).
 */
struct dmi_onboard_device_ex
{
    /**
     * @brief Device reference designator.
     */
    const char *designator;

    /**
     * @brief Device type.
     */
    dmi_onboard_device_type_t type;

    /**
     * @brief Set to `true` if device is enabled.
     */
    bool is_enabled;

    /**
     * @brief Device type instance. An unique value (within a given onboard
     * device type) used to indicate the order the device is designated by the
     * system.
     *
     * For example, a system with two identical ethernet NICs may designate one
     * NIC (with higher Bus/Device/Function=15/0/0) as the first onboard NIC
     * (instance 1) and the other NIC (with lower Bus/Device/Function = 3/0/0)
     * as the second onboard NIC (instance 2).
     */
    unsigned short instance;

    /**
     * @brief Device address.
     */
    dmi_pci_addr_t address;
};

/**
 * @brief Onboard devices additional information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_onboard_device_ex_spec;

#endif // !OPENDMI_ENTITY_ONBOARD_DEVICE_EX_H
