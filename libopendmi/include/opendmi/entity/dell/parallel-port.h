//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_DELL_PARALLEL_PORT_H
#define OPENDMI_ENTITY_DELL_PARALLEL_PORT_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/entity/dell/common.h>

typedef struct dmi_dell_parallel_port dmi_dell_parallel_port_t;
typedef union dmi_dell_parallel_port_caps dmi_dell_parallel_port_caps_t;

/**
 * @brief Dell parallel port connector types.
 */
typedef enum dmi_dell_parallel_port_connector_type
{
    DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_UNSPEC      = 0x00, ///< Unspecified
    DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_OTHER       = 0x01, ///< Other
    DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_UNKNOWN     = 0x02, ///< Unknown
    DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_DB_25_F     = 0x03, ///< DB-25 female IEEE 1284-A receptacle
    DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_DB_25_M     = 0x04, ///< DB-25 male IEEE 1284-A plug
    DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_CN_36       = 0x05, ///< Centronics IEEE 1284-B receptacle
    DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_HPCN_36     = 0x06, ///< Mini-Centronics IEEE 1284-C receptacle
    DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_PROPRIETARY = 0x07, ///< Proprietary
    DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_CN_14       = 0xA0, ///< Centronics-14
    DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_DB_36_F     = 0xA1, ///< DB-36 female
    DMI_DELL_PARALLEL_PORT_CONNECTOR_TYPE_HPCN_20     = 0xA2, ///< Mini-Centronics-20
} dmi_dell_parallel_port_connector_type_t;

/**
 * @brief Dell parallel port connector pinouts.
 */
typedef enum dmi_dell_parallel_port_connector_pinout
{
    DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_UNSPEC       = 0x00, ///< Unspecied
    DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_OTHER        = 0x01, ///< Other
    DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_UNKNOWN      = 0x02, ///< Unknown
    DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_XT_AT        = 0x03, ///< XT/AT
    DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_PS2          = 0x04, ///< PS/2
    DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_IEEE_1284    = 0x05, ///< IEEE 1284
    DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_PROPRIETARY  = 0x06, ///< Proprietary
    DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_PC_98        = 0xA0, ///< PC-98
    DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_PC_98_HIRESO = 0xA1, ///< PC-98 Hireso
    DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_PC_H98       = 0xA2, ///< PC-H98
    DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_PC_98_NOTE   = 0xA3, ///< PC-98 Note
    DMI_DELL_PARALLEL_PORT_CONNECTOR_PINOUT_PC_98_FULL   = 0xA4, ///< PC-98 Full
} dmi_dell_parallel_port_connector_pinout_t;

/**
 * @brief Dell parallell port capabilities.
 */
dmi_packed_union(dmi_dell_parallel_port_caps)
{
    dmi_word_t __value;

    dmi_packed_struct()
    {
        dmi_word_t is_xt_at        : 1; ///< XT/AT
        dmi_word_t is_ps2          : 1; ///< PS/2
        dmi_word_t is_ecp          : 1; ///< ECP
        dmi_word_t is_epp          : 1; ///< EPP
        dmi_word_t is_pc_98        : 1; ///< PC-98
        dmi_word_t is_pc_98_hireso : 1; ///< PC-98 Hireso
        dmi_word_t is_pc_h98       : 1; ///< PC-H98
        dmi_word_t reserved        : 8; ///< Reserved
        dmi_word_t has_dma_support : 1; ///< Device supports DMA
    };
};

dmi_static_assert_value_union(dmi_dell_parallel_port_caps);

struct dmi_dell_parallel_port
{
    /**
     * @brief Base address.
     */
    uint16_t base_addr;

    /**
     * @brief IRQ number.
     */
    uint8_t irq_number;

    /**
     * @brief Connector type.
     */
    dmi_dell_parallel_port_connector_type_t connector;

    /**
     * @brief Connector pinout.
     */
    dmi_dell_parallel_port_connector_pinout_t pinout;

    /**
     * @brief Capabilities.
     */
    dmi_dell_parallel_port_caps_t capabilities;

    /**
     * @brief Security settings.
     */
    dmi_dell_port_security_t security;
};

/**
 * @brief Dell parallel port entity specification.
 */
extern const dmi_entity_spec_t dmi_dell_parallel_port_spec;

__BEGIN_DECLS

const char *dmi_dell_parallel_port_connector_type_name(
        dmi_dell_parallel_port_connector_type_t value);

const char *dmi_dell_parallel_port_connector_pinout_name(
        dmi_dell_parallel_port_connector_pinout_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_DELL_PARALLEL_PORT_H
