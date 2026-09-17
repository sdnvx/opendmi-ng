//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MGMT_DEVICE_THRESHOLD_H
#define OPENDMI_ENTITY_MGMT_DEVICE_THRESHOLD_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_mgmt_device_threshold dmi_mgmt_device_threshold_t;

struct dmi_mgmt_device_threshold
{
    /**
     * @brief Lower non-critical threshold for this component.
     */
    short lower_non_critical;

    /**
     * @brief Upper non-critical threshold for this component.
     */
    short upper_non_critical;

    /**
     * @brief Lower critical threshold for this component.
     */
    short lower_critical;

    /**
     * @brief Upper critical threshold for this component.
     */
    short upper_critical;

    /**
     * @brief Lower non-recoverable threshold for this component.
     */
    short lower_non_recoverable;

    /**
     * @brief Upper non-recoverable threshold for this component.
     */
    short upper_non_recoverable;
};

/**
 * @brief Management device threshold data entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_mgmt_device_threshold_spec;

#endif // !OPENDMI_ENTITY_MGMT_DEVICE_THRESHOLD_H
