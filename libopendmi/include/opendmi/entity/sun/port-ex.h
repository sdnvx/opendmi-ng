//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SUN_PORT_EX_H
#define OPENDMI_ENTITY_SUN_PORT_EX_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_sun_port_ex dmi_sun_port_ex_t;

/**
 * @brief Sun port extended information (type 136).
 */
struct dmi_sun_port_ex
{
    /**
     * @brief Handle of the chassis containing the port.
     */
    dmi_handle_t chassis_handle;

    /**
     * @brief Handle of the extended port connector information structure.
     */
    dmi_handle_t port_handle;

    /**
     * @brief Type of the device connected to the port.
     */
    uint8_t device_type;

    /**
     * @brief Handle of the device connected to the port.
     */
    dmi_handle_t device_handle;

    /**
     * @brief PHY number.
     */
    uint8_t phy;
};

/**
 * @brief Sun port extended information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_sun_port_ex_spec;

#endif // !OPENDMI_ENTITY_SUN_PORT_EX_H
