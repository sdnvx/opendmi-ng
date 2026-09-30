//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_PHYSICAL_ATTRS_H
#define OPENDMI_ENTITY_HPE_PHYSICAL_ATTRS_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/utils/uuid.h>

typedef struct dmi_hpe_physical_attrs dmi_hpe_physical_attrs_t;

/**
 * @brief HP/HPE physical attribute information (type 226).
 *
 * Keeps the physical serial number and UUID of a server, whose standard
 * fields hold virtual ones, e.g. on Synergy, where a workload moves from one
 * server to another along with its identity. Up to G7, the structure holds 16
 * characters instead of the UUID: the product number and the serial number.
 */
struct dmi_hpe_physical_attrs
{
    /**
     * @brief Physical UUID, from Gen8 onwards.
     */
    dmi_uuid_t uuid;

    /**
     * @brief Product number and serial number, 16 characters, up to G7, as
     * the structure holds them.
     */
    dmi_binary_t identifier_raw;

    /**
     * @brief Product number and serial number, which `identifier` points to.
     */
    char identifier_buffer[17];

    /**
     * @brief Product number and serial number, e.g. `484184GB894484YN`, or
     * @c nullptr if they are not printable.
     */
    const char *identifier;

    /**
     * @brief Physical serial number.
     */
    const char *serial_number;
};

/**
 * @brief HP/HPE physical attribute information entity specification, up to
 * G7.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_physical_attrs_legacy_spec;

/**
 * @brief HP/HPE physical attribute information entity specification, from
 * Gen8 onwards.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_physical_attrs_spec;

#endif // !OPENDMI_ENTITY_HPE_PHYSICAL_ATTRS_H
