//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_APPLE_PROCESSOR_BUS_SPEED_H
#define OPENDMI_ENTITY_APPLE_PROCESSOR_BUS_SPEED_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_APPLE_PROCESSOR_BUS_SPEED_T
#   define DMI_APPLE_PROCESSOR_BUS_SPEED_T
    typedef struct dmi_apple_processor_bus_speed dmi_apple_processor_bus_speed_t;
#endif // !DMI_APPLE_PROCESSOR_BUS_SPEED_T

/**
 * @brief Apple processor bus speed information structure (type 132).
 *
 * Tells the speed of the bus of the processor, e.g. of its QuickPath
 * Interconnect. Laid out as the `AppleSmBios.h` header of OpenCore describes
 * it.
 */
struct dmi_apple_processor_bus_speed
{
    /**
     * @brief Speed of the bus, in megatransfers per second, e.g. `4800` for
     * a QuickPath Interconnect of 4.8 GT/s.
     */
    uint16_t speed;
};

/**
 * @brief Apple processor bus speed information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_apple_processor_bus_speed_spec;

#endif // !OPENDMI_ENTITY_APPLE_PROCESSOR_BUS_SPEED_H
