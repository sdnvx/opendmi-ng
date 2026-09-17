//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_DELL_INFRARED_PORT_H
#define OPENDMI_ENTITY_DELL_INFRARED_PORT_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/entity/dell/common.h>

typedef struct dmi_dell_infrared_port dmi_dell_infrared_port_t;

/**
 * @brief Dell infrared port protocols.
 */
typedef enum dmi_dell_infrared_proto {
    DMI_DELL_INFRARED_PROTO_UNSPEC  = 0x00,
    DMI_DELL_INFRARED_PROTO_OTHER   = 0x01,
    DMI_DELL_INFRARED_PROTO_UNKNOWN = 0x02,
    DMI_DELL_INFRARED_PROTO_SIR     = 0x03,
    DMI_DELL_INFRARED_PROTO_FIR     = 0x04,
    DMI_DELL_INFRARED_PROTO_MIR     = 0x05,
} dmi_dell_infrared_proto_t;

/**
 * @brief Dell infrared port structure (type 211).
 */
struct dmi_dell_infrared_port
{
    /**
     * @brief Location.
     */
    const char *location;

    /**
     * @brief Enable state.
     */
    dmi_dell_enable_state_t state;

    /**
     * @brief Speed limit state.
     */
    dmi_dell_enable_state_t speed_limit_state;

    /**
     * @brief Speed limit.
     */
    uint32_t speed_limit;

    /**
     * @brief Physical port name.
     */
    const char *physical_port;

    /**
     * @brief Virtual COM port name.
     */
    const char *virtual_com_port;

    /**
     * @brief Virtual LPT port name.
     */
    const char *virtual_lpt_port;

    /**
     * @brief Protocol.
     */
    dmi_dell_infrared_proto_t protocol;
};

/**
 * @brief Dell infrared port entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_dell_infrared_port_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_dell_infrared_proto_name(dmi_dell_infrared_proto_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_DELL_INFRARED_PORT_H
