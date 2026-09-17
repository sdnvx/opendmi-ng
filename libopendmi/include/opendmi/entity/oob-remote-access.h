//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_OOB_REMOTE_ACCESS_H
#define OPENDMI_ENTITY_OOB_REMOTE_ACCESS_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_oob_remote_access dmi_oob_remote_access_t;
typedef union  dmi_oob_connections   dmi_oob_connections_t;

/**
 * @brief Out-of-band remote access connections.
 */
dmi_packed_union(dmi_oob_connections)
{
    /**
     * @brief Raw value.
     */
    dmi_byte_t __value;

    dmi_packed_struct()
    {
        /**
         * @brief Inbound connection enabled. Identifies whether (`true`) or
         * not (`false`) the facility is allowed to initiate outbound
         * connections to receive incoming connections for the purpose of
         * remote operations or problem management.
         */
        dmi_byte_t is_inbound_enabled : 1;

        /**
         * @brief Outbound connection enabled. Identifies whether (`true`) or
         * not (`false`) the facility is allowed to initiate outbound
         * connections to contact an alert management facility when critical
         * conditions occur.
         */
        dmi_byte_t is_outbound_enabled : 1;

        /**
         * @brief Reserved for future definition by this specification. Set to
         * all zeros.
         */
        dmi_byte_t __reserved : 6;
    };
};

dmi_static_assert_value_union(dmi_oob_connections);

/**
 * @brief Out-of-band remote access structure (type 30).
 *
 * This structure describes the attributes and policy settings of a hardware
 * facility that may be used to gain remote access to a hardware system when
 * the operating system is not available due to power-down status, hardware
 * failures, or boot failures.
 *
 * @since SMBIOS 2.2
 */
struct dmi_oob_remote_access
{
    /**
     * @brief The manufacturer of the out-of-band access facility.
     */
    const char *vendor;

    /**
     * @brief Current remote-access connections.
     */
    dmi_oob_connections_t connections;
};

/**
 * @brief Out-of-band remote access entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_oob_remote_access_spec;

#endif // !OPENDMI_ENTITY_OOB_REMOTE_ACCESS_H
