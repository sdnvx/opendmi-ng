//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_DELL_SERIAL_PORT_H
#define OPENDMI_ENTITY_DELL_SERIAL_PORT_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/entity/dell/common.h>

typedef struct dmi_dell_serial_port dmi_dell_serial_port_t;

/**
 * @brief Dell serial port connector types.
 */
typedef enum dmi_dell_serial_port_connector_type
{
    DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_UNSPEC           = 0x00, ///< Unspecified
    DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_OTHER            = 0x01, ///< Other
    DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_UNKNOWN          = 0x02, ///< Unknown
    DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_DB_9_M           = 0x03, ///< DB-9 male
    DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_DB_9_F           = 0x04, ///< DB-9 female
    DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_DB_25_M          = 0x05, ///< DB-25 male
    DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_DB_25_F          = 0x06, ///< DB-25 female
    DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_RJ_11            = 0x07, ///< RJ-11
    DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_RJ_45            = 0x08, ///< RJ-45
    DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_PROPRIETARY      = 0x09, ///< Proprietary
    DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_CIRCULAR_DIN_8_M = 0x0A, ///< Circular DIN-8 male
    DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_CIRCULAR_DIN_8_F = 0x0B, ///< Circular DIN-8 female
    DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_IEEE_1284_C_14   = 0xA0, ///< Mini-Centronics Type-14
    DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_IEEE_1284_C_26   = 0xA1, ///< Mini-Centronics Type-26
    __DMI_DELL_SERIAL_PORT_CONNECTOR_TYPE_COUNT
} dmi_dell_serial_port_connector_type_t;

/**
 * @brief Dell serial port capabilities.
 */
typedef enum dmi_dell_serial_port_caps
{
    DMI_DELL_SERIAL_PORT_CAPS_UNSPEC    = 0x00, ///< Unspecified
    DMI_DELL_SERIAL_PORT_CAPS_OTHER     = 0x01, ///< Other
    DMI_DELL_SERIAL_PORT_CAPS_UNKNOWN   = 0x02, ///< Unknown
    DMI_DELL_SERIAL_PORT_CAPS_XT_AT     = 0x03, ///< XT/AT compatible
    DMI_DELL_SERIAL_PORT_CAPS_16450     = 0x04, ///< 16450 compatible
    DMI_DELL_SERIAL_PORT_CAPS_16550     = 0x05, ///< 16550 compatible
    DMI_DELL_SERIAL_PORT_CAPS_16550A    = 0x06, ///< 16550A compatible
    DMI_DELL_SERIAL_PORT_CAPS_8251      = 0xA0, ///< 8251 compatible
    DMI_DELL_SERIAL_PORT_CAPS_8251_FIFO = 0xA1, ///< 8251 FIFO compatible
    __DMI_DELL_SERIAL_PORT_CAPS_COUNT
} dmi_dell_serial_port_caps_t;

/**
 * @brief Dell serial port structure (type 210).
 */
struct dmi_dell_serial_port
{
    /**
     * @brief Base address.
     */
    uint16_t base_addr;

    /**
     * @brief IRQ number;
     */
    uint8_t irq_number;

    /**
     * @brief Connector type.
     */
    dmi_dell_serial_port_connector_type_t connector;

    /**
     * @brief Capabilities.
     */
    dmi_dell_serial_port_caps_t capabilities;

    /**
     * @brief Maximum BIOS speed.
     */
    uint32_t maximum_speed;

    /**
     * @brief Security settings.
     */
    dmi_dell_port_security_t security;
};

/**
 * @brief Dell serial port entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_dell_serial_port_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_dell_serial_port_connector_type_name(dmi_dell_serial_port_connector_type_t value);
__dmi_api const char *dmi_dell_serial_port_caps_name(dmi_dell_serial_port_caps_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_DELL_SERIAL_PORT_H
