//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_RACK_LOCATOR_H
#define OPENDMI_ENTITY_HPE_RACK_LOCATOR_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_hpe_rack_locator dmi_hpe_rack_locator_t;

/**
 * @brief HP/HPE system/rack locator (type 204).
 *
 * Tells where a blade server is: the rack, the enclosure and the bay.
 */
struct dmi_hpe_rack_locator
{
    /**
     * @brief Name of the rack.
     */
    const char *rack_name;

    /**
     * @brief Name of the enclosure.
     */
    const char *enclosure_name;

    /**
     * @brief Model of the enclosure.
     */
    const char *enclosure_model;

    /**
     * @brief Bay of the server.
     */
    const char *server_bay;

    /**
     * @brief Number of the bays of the enclosure.
     */
    uint8_t bay_count;

    /**
     * @brief Number of the filled bays of the enclosure.
     */
    uint8_t filled_bay_count;

    /**
     * @brief Serial number of the enclosure.
     */
    const char *enclosure_serial;
};

/**
 * @brief HP/HPE system/rack locator entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_rack_locator_spec;

#endif // !OPENDMI_ENTITY_HPE_RACK_LOCATOR_H
