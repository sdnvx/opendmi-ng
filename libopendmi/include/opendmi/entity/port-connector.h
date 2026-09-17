//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_PORT_CONNECTOR_H
#define OPENDMI_ENTITY_PORT_CONNECTOR_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_port_connector dmi_port_connector_t;

typedef enum dmi_connector_type
{
    DMI_CONNECTOR_TYPE_NONE             = 0x00, ///< None
    DMI_CONNECTOR_TYPE_CN_36            = 0x01, ///< Centronics
    DMI_CONNECTOR_TYPE_HPCN_36          = 0x02, ///< Mini-Centronics
    DMI_CONNECTOR_TYPE_PROPRIETARY      = 0x03, ///< Proprietary
    DMI_CONNECTOR_TYPE_DB_25_M          = 0x04, ///< DB-25 male
    DMI_CONNECTOR_TYPE_DB_25_F          = 0x05, ///< DB-25 female
    DMI_CONNECTOR_TYPE_DB_15_M          = 0x06, ///< DB-15 male
    DMI_CONNECTOR_TYPE_DB_15_F          = 0x07, ///< DB-15 female
    DMI_CONNECTOR_TYPE_DB_9_M           = 0x08, ///< DB-9 male
    DMI_CONNECTOR_TYPE_DB_9_F           = 0x09, ///< DB-9 female
    DMI_CONNECTOR_TYPE_RJ_11            = 0x0A, ///< RJ-11
    DMI_CONNECTOR_TYPE_RJ_45            = 0x0B, ///< RJ-45
    DMI_CONNECTOR_TYPE_MINI_SCSI_50PIN  = 0x0C, ///< 50-pin MiniSCSI
    DMI_CONNECTOR_TYPE_MINI_DIN         = 0x0D, ///< Mini-DIN
    DMI_CONNECTOR_TYPE_MICRO_DIN        = 0x0E, ///< Micro-DIN
    DMI_CONNECTOR_TYPE_PS2              = 0x0F, ///< PS/2
    DMI_CONNECTOR_TYPE_INFRARED         = 0x10, ///< Infrared
    DMI_CONNECTOR_TYPE_HP_HIL           = 0x11, ///< HP-HIL
    DMI_CONNECTOR_TYPE_ACCESS_BUS       = 0x12, ///< Access Bus (USB)
    DMI_CONNECTOR_TYPE_SSA_SCSI         = 0x13, ///< SSA SCSI
    DMI_CONNECTOR_TYPE_CIRCULAR_DIN_8_M = 0x14, ///< Circular DIN-8 male
    DMI_CONNECTOR_TYPE_CIRCULAR_DIN_8_F = 0x15, ///< Circular DIN-8 female
    DMI_CONNECTOR_TYPE_ONBOARD_IDE      = 0x16, ///< On Board IDE
    DMI_CONNECTOR_TYPE_ONBOARD_FLOPPY   = 0x17, ///< On Board Floppy
    DMI_CONNECTOR_TYPE_DIL_9PIN         = 0x18, ///< 9-pin Dual Inline (pin 10 cut)
    DMI_CONNECTOR_TYPE_DIL_25PIN        = 0x19, ///< 25-pin Dual Inline (pin 26 cut)
    DMI_CONNECTOR_TYPE_DIL_50PIN        = 0x1A, ///< 50-pin Dual Inline
    DMI_CONNECTOR_TYPE_DIL_68PIN        = 0x1B, ///< 68-pin Dual Inline
    DMI_CONNECTOR_TYPE_ONBOARD_SOUND    = 0x1C, ///< On Board Sound Input from CD-ROM
    DMI_CONNECTOR_TYPE_HPCN_14          = 0x1D, ///< Mini-Centronics Type-14
    DMI_CONNECTOR_TYPE_HPCN_26          = 0x1E, ///< Mini-Centronics Type-26
    DMI_CONNECTOR_TYPE_MINI_JACK        = 0x1F, ///< Mini-jack (headphones)
    DMI_CONNECTOR_TYPE_BNC              = 0x20, ///< BNC
    DMI_CONNECTOR_TYPE_IEEE_1394        = 0x21, ///< IEEE 1394
    DMI_CONNECTOR_TYPE_SAS_SATA         = 0x22, ///< SAS/SATA Plug Receptacle
    DMI_CONNECTOR_TYPE_USB_C            = 0x23, ///< USB Type-C Receptacle
    DMI_CONNECTOR_TYPE_PC_98            = 0xA0, ///< PC-98
    DMI_CONNECTOR_TYPE_PC_98_HIRESO     = 0xA1, ///< PC-98 Hireso
    DMI_CONNECTOR_TYPE_PC_H98           = 0xA2, ///< PC-H98
    DMI_CONNECTOR_TYPE_PC_98_NOTE       = 0xA3, ///< PC-98 Note
    DMI_CONNECTOR_TYPE_PC_98_FULL       = 0xA4, ///< PC-98 Full
    DMI_CONNECTOR_TYPE_OTHER            = 0xFF, ///< Other
} dmi_connector_type_t;

/**
 * @brief Port types
 */
typedef enum dmi_port_type
{
    DMI_PORT_TYPE_NONE             = 0x00, ///< None
    DMI_PORT_TYPE_PARALLEL_XT_AT   = 0x01, ///< Parallel XT/AT compatible
    DMI_PORT_TYPE_PARALLEL_PS2     = 0x02, ///< Parallel PS/2
    DMI_PORT_TYPE_PARALLEL_ECP     = 0x03, ///< Parallel ECP
    DMI_PORT_TYPE_PARALLEL_EPP     = 0x04, ///< Parallel EPP
    DMI_PORT_TYPE_PARALLEL_ECP_EPP = 0x05, ///< Parallel ECP/EPP
    DMI_PORT_TYPE_SERIAL_XT_AT     = 0x06, ///< Serial XT/AT compatible
    DMI_PORT_TYPE_SERIAL_16450     = 0x07, ///< Serial 16450 compatible
    DMI_PORT_TYPE_SERIAL_16550     = 0x08, ///< Serial 16550 compatible
    DMI_PORT_TYPE_SERIAL_16550A    = 0x09, ///< Serial 16550A compatible
    DMI_PORT_TYPE_SCSI             = 0x0A, ///< SCSI
    DMI_PORT_TYPE_MIDI             = 0x0B, ///< MIDI
    DMI_PORT_TYPE_JOYSTICK         = 0x0C, ///< Joystick
    DMI_PORT_TYPE_KEYBOARD         = 0x0D, ///< Keyboard
    DMI_PORT_TYPE_MOUSE            = 0x0E, ///< Mouse
    DMI_PORT_TYPE_SCSI_SSA         = 0x0F, ///< SSA SCSI
    DMI_PORT_TYPE_USB              = 0x10, ///< USB
    DMI_PORT_TYPE_FIREWIRE         = 0x11, ///< FireWire (IEEE P1394)
    DMI_PORT_TYPE_PCMCIA_I         = 0x12, ///< PCMCIA type I
    DMI_PORT_TYPE_PCMCIA_II        = 0x13, ///< PCMCIA type II
    DMI_PORT_TYPE_PCMCIA_III       = 0x14, ///< PCMCIA type III
    DMI_PORT_TYPE_CARDBUS          = 0x15, ///< CardBus
    DMI_PORT_TYPE_ACCESS_BUS       = 0x16, ///< ACCESS.bus
    DMI_PORT_TYPE_SCSI_2           = 0x17, ///< SCSI-2
    DMI_PORT_TYPE_SCSI_WIDE        = 0x18, ///< Wide SCSI
    DMI_PORT_TYPE_PC_98            = 0x19, ///< PC-98
    DMI_PORT_TYPE_PC_98_HIRESO     = 0x1A, ///< PC-98-Hireso
    DMI_PORT_TYPE_PC_H98           = 0x1B, ///< PC-H98
    DMI_PORT_TYPE_VIDEO            = 0x1C, ///< Video
    DMI_PORT_TYPE_AUDIO            = 0x1D, ///< Audio
    DMI_PORT_TYPE_MODEM            = 0x1E, ///< Modem
    DMI_PORT_TYPE_NETWORK          = 0x1F, ///< Network
    DMI_PORT_TYPE_SATA             = 0x20, ///< SATA
    DMI_PORT_TYPE_SAS              = 0x21, ///< SAS
    DMI_PORT_TYPE_MFDP             = 0x22, ///< MFDP (Multi-Function Display Port)
    DMI_PORT_TYPE_THUNDERBOLT      = 0x23, ///< Thunderbolt
    DMI_PORT_TYPE_8251             = 0xA0, ///< 8251 compatible
    DMI_PORT_TYPE_8251_FIFO        = 0xA1, ///< 8251 FIFO compatible
    DMI_PORT_TYPE_OTHER            = 0xFF, ///< Other
} dmi_port_type_t;

/**
 * @brief Port connector information structure (type 8).
 */
struct dmi_port_connector
{
    /**
     * @brief Internal reference designator, that is, internal to the system
     * enclosure. Example: "J101".
     */
    const char *internal_designator;

    /**
     * @brief Internal connector type.
     */
    dmi_connector_type_t internal_connector;

    /**
     * @brief External reference designation external to the system enclosure.
     * Example: "COM A".
     */
    const char *external_designator;

    /**
     * @brief External connector type.
     */
    dmi_connector_type_t external_connector;

    /**
     * @brief Port type.
     */
    dmi_port_type_t port_type;
};

/**
 * @brief Port connector information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_port_connector_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_connector_type_name(dmi_connector_type_t value);
__dmi_api const char *dmi_port_type_name(dmi_port_type_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_PORT_CONNECTOR_H
