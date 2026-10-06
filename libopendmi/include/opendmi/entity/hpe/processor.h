//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_PROCESSOR_H
#define OPENDMI_ENTITY_HPE_PROCESSOR_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_HPE_PROCESSOR_T
#   define DMI_HPE_PROCESSOR_T
    typedef struct dmi_hpe_processor dmi_hpe_processor_t;
#endif // !DMI_HPE_PROCESSOR_T

/**
 * @brief HP/HPE processor specific information (type 197).
 *
 * Supplements the processor information (type 4) of each processor socket
 * or slot with the numbers the server tells its processors by.
 */
struct dmi_hpe_processor
{
    /**
     * @brief Handle of the processor information structure (type 4).
     */
    dmi_handle_t processor_handle;

    /**
     * @brief Local APIC ID of the processor.
     */
    uint8_t apic_id;

    /**
     * @brief Whether the processor is the bootstrap processor.
     */
    bool is_bsp;

    /**
     * @brief Whether the processor runs in the x2APIC mode.
     */
    bool is_x2apic;

    /**
     * @brief Whether advanced thermal margining is enabled.
     */
    bool is_thermal_margining;

    /**
     * @brief Physical slot, as the silk screen names it. Set to `UINT8_MAX`
     * when the processor is not in a slot.
     */
    uint8_t slot;

    /**
     * @brief Physical socket, as the silk screen names it. Set to `UINT8_MAX`
     * when the processor is not in a socket.
     */
    uint8_t socket;

    /**
     * @brief Rated maximum power of the processor in watts, zero if unknown.
     */
    uint16_t maximum_power;

    /**
     * @brief x2APIC ID of the processor, valid in the x2APIC mode, and not
     * shown otherwise. Set to `UINT32_MAX` when the structure holds none.
     */
    uint32_t x2apic_id;

    /**
     * @brief Unique identifier of the processor, zero if unknown.
     */
    uint64_t uuid;

    /**
     * @brief Speed of the interconnect in MT/s, zero if unknown.
     */
    uint16_t interconnect_speed;

    /**
     * @brief QDF or S-Spec number of Intel processors, six characters padded
     * with spaces, as the structure holds them.
     */
    uint8_t qdf_raw[6];

    /**
     * @brief QDF or S-Spec number of Intel processors, @c nullptr if the
     * structure holds none.
     *
     * The string belongs to the structure, and is freed along with it.
     */
    char *qdf;
};

/**
 * @brief HP/HPE processor specific information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_processor_spec;

#endif // !OPENDMI_ENTITY_HPE_PROCESSOR_H
