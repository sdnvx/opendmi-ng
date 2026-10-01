//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MGMT_DEVICE_COMPONENT_H
#define OPENDMI_ENTITY_MGMT_DEVICE_COMPONENT_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_MGMT_DEVICE_COMPONENT_T
#   define DMI_MGMT_DEVICE_COMPONENT_T
    typedef struct dmi_mgmt_device_component dmi_mgmt_device_component_t;
#endif // !DMI_MGMT_DEVICE_COMPONENT_T

/**
 * @brief Management device component structure (type 35).
 *
 * The structure associates a probe or a cooling device with the management
 * device controlling it and, optionally, with its thresholds.
 */
struct dmi_mgmt_device_component
{
    /**
     * @brief String that contains additional descriptive information about
     * the component.
     */
    const char *description;

    /**
     * @brief Handle, or instance number, of the management device that
     * contains this component.
     */
    dmi_handle_t device_handle;

    /**
     * @brief Reference to the management device that contains this component.
     */
    dmi_entity_t *device;

    /**
     * @brief Handle, or instance number, of the probe or cooling device that
     * defines this component.
     */
    dmi_handle_t component_handle;

    /**
     * @brief Reference to the probe or cooling device that defines this
     * component.
     */
    dmi_entity_t *component;

    /**
     * @brief Handle, or instance number, associated with the device
     * thresholds.
     */
    dmi_handle_t threshold_handle;

    /**
     * @brief Reference to the device thresholds structure.
     */
    dmi_entity_t *threshold;
};

/**
 * @brief Management device component entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_mgmt_device_component_spec;

#endif // !OPENDMI_ENTITY_MGMT_DEVICE_COMPONENT_H
