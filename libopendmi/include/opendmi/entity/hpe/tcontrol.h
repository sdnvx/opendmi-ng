//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_TCONTROL_H
#define OPENDMI_ENTITY_HPE_TCONTROL_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_hpe_tcontrol dmi_hpe_tcontrol_t;

/**
 * @brief HP/HPE processor TControl information (type 211).
 *
 * Gives the Tcontrol value of a processor, the temperature the fans begin to
 * spin up at, which Intel programs into each processor.
 */
struct dmi_hpe_tcontrol
{
    /**
     * @brief Handle of the processor information structure (type 4).
     */
    dmi_handle_t processor_handle;

    /**
     * @brief Tcontrol value. Set to zero when not available.
     */
    uint8_t tcontrol;
};

/**
 * @brief HP/HPE processor TControl information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_tcontrol_spec;

#endif // !OPENDMI_ENTITY_HPE_TCONTROL_H
