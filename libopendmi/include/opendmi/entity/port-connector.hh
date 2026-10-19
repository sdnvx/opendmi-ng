//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_PORT_CONNECTOR_HH
#define OPENDMI_ENTITY_PORT_CONNECTOR_HH

#pragma once

#include <type_traits>
#include <string>

namespace dmi {
    namespace capi {
#       include <opendmi/entity/port-connector.h>
    }

    using connector_type_t = capi::dmi_connector_type_t;
    using port_type_t      = capi::dmi_port_type_t;

    enum class connector_type : std::underlying_type_t<connector_type_t>
    {
        none             = capi::DMI_CONNECTOR_TYPE_NONE,             ///< None
        cn_36            = capi::DMI_CONNECTOR_TYPE_CN_36,            ///< Centronics
        hpcn_36          = capi::DMI_CONNECTOR_TYPE_HPCN_36,          ///< Mini-Centronics
        proprietary      = capi::DMI_CONNECTOR_TYPE_PROPRIETARY,      ///< Proprietary
        db_25_m          = capi::DMI_CONNECTOR_TYPE_DB_25_M,          ///< DB-25 male
        db_25_f          = capi::DMI_CONNECTOR_TYPE_DB_25_F,          ///< DB-25 female
        db_15_m          = capi::DMI_CONNECTOR_TYPE_DB_15_M,          ///< DB-15 male
        db_15_f          = capi::DMI_CONNECTOR_TYPE_DB_15_F,          ///< DB-15 female
        db_9_m           = capi::DMI_CONNECTOR_TYPE_DB_9_M,           ///< DB-9 male
        db_9_f           = capi::DMI_CONNECTOR_TYPE_DB_9_F,           ///< DB-9 female
        rj_11            = capi::DMI_CONNECTOR_TYPE_RJ_11,            ///< RJ-11
        rj_45            = capi::DMI_CONNECTOR_TYPE_RJ_45,            ///< RJ-45
        mini_scsi_50pin  = capi::DMI_CONNECTOR_TYPE_MINI_SCSI_50PIN,  ///< 50-pin MiniSCSI
        mini_din         = capi::DMI_CONNECTOR_TYPE_MINI_DIN,         ///< Mini-DIN
        micro_din        = capi::DMI_CONNECTOR_TYPE_MICRO_DIN,        ///< Micro-DIN
        ps2              = capi::DMI_CONNECTOR_TYPE_PS2,              ///< PS/2
        infrared         = capi::DMI_CONNECTOR_TYPE_INFRARED,         ///< Infrared
        hp_hil           = capi::DMI_CONNECTOR_TYPE_HP_HIL,           ///< HP-HIL
        access_bus       = capi::DMI_CONNECTOR_TYPE_ACCESS_BUS,       ///< Access Bus (USB)
        ssa_scsi         = capi::DMI_CONNECTOR_TYPE_SSA_SCSI,         ///< SSA SCSI
        circular_din_8_m = capi::DMI_CONNECTOR_TYPE_CIRCULAR_DIN_8_M, ///< Circular DIN-8 male
        circular_din_8_f = capi::DMI_CONNECTOR_TYPE_CIRCULAR_DIN_8_F, ///< Circular DIN-8 female
        onboard_ide      = capi::DMI_CONNECTOR_TYPE_ONBOARD_IDE,      ///< On Board IDE
        onboard_floppy   = capi::DMI_CONNECTOR_TYPE_ONBOARD_FLOPPY,   ///< On Board Floppy
        dil_9pin         = capi::DMI_CONNECTOR_TYPE_DIL_9PIN,         ///< 9-pin Dual Inline (pin 10 cut)
        dil_25pin        = capi::DMI_CONNECTOR_TYPE_DIL_25PIN,        ///< 25-pin Dual Inline (pin 26 cut)
        dil_50pin        = capi::DMI_CONNECTOR_TYPE_DIL_50PIN,        ///< 50-pin Dual Inline
        dil_68_pin       = capi::DMI_CONNECTOR_TYPE_DIL_68PIN,        ///< 68-pin Dual Inline
        onboard_sound    = capi::DMI_CONNECTOR_TYPE_ONBOARD_SOUND,    ///< On Board Sound Input from CD-ROM
        hpcn_14          = capi::DMI_CONNECTOR_TYPE_HPCN_14,          ///< Mini-Centronics Type-14
        hpcn_26          = capi::DMI_CONNECTOR_TYPE_HPCN_26,          ///< Mini-Centronics Type-26
        mini_jack        = capi::DMI_CONNECTOR_TYPE_MINI_JACK,        ///< Mini-jack (headphones)
        bnc              = capi::DMI_CONNECTOR_TYPE_BNC,              ///< BNC
        ieee_1394        = capi::DMI_CONNECTOR_TYPE_IEEE_1394,        ///< IEEE 1394
        sas_sata         = capi::DMI_CONNECTOR_TYPE_SAS_SATA,         ///< SAS/SATA Plug Receptacle
        usb_c            = capi::DMI_CONNECTOR_TYPE_USB_C,            ///< USB Type-C Receptacle
        pc_98            = capi::DMI_CONNECTOR_TYPE_PC_98,            ///< PC-98
        pc_98_hireso     = capi::DMI_CONNECTOR_TYPE_PC_98_HIRESO,     ///< PC-98 Hireso
        pc_h98           = capi::DMI_CONNECTOR_TYPE_PC_H98,           ///< PC-H98
        pc_98_note       = capi::DMI_CONNECTOR_TYPE_PC_98_NOTE,       ///< PC-98 Note
        pc_98_full       = capi::DMI_CONNECTOR_TYPE_PC_98_FULL,       ///< PC-98 Full
        other            = capi::DMI_CONNECTOR_TYPE_OTHER,            ///< Other
    };

    enum class port_type : std::underlying_type_t<port_type_t>
    {
        none             = capi::DMI_PORT_TYPE_NONE,             ///< None
        parallel_xt_at   = capi::DMI_PORT_TYPE_PARALLEL_XT_AT,   ///< Parallel XT/AT compatible
        parallel_ps2     = capi::DMI_PORT_TYPE_PARALLEL_PS2,     ///< Parallel PS/2
        parallel_ecp     = capi::DMI_PORT_TYPE_PARALLEL_ECP,     ///< Parallel ECP
        parallel_epp     = capi::DMI_PORT_TYPE_PARALLEL_EPP,     ///< Parallel EPP
        parallel_ecp_epp = capi::DMI_PORT_TYPE_PARALLEL_ECP_EPP, ///< Parallel ECP/EPP
        serial_xt_at     = capi::DMI_PORT_TYPE_SERIAL_XT_AT,     ///< Serial XT/AT compatible
        serial_c16450    = capi::DMI_PORT_TYPE_SERIAL_C16450,    ///< Serial 16450 compatible
        serial_c16550    = capi::DMI_PORT_TYPE_SERIAL_C16550,    ///< Serial 16550 compatible
        serial_c16550a   = capi::DMI_PORT_TYPE_SERIAL_C16550A,   ///< Serial 16550A compatible
        scsi             = capi::DMI_PORT_TYPE_SCSI,             ///< SCSI
        midi             = capi::DMI_PORT_TYPE_MIDI,             ///< MIDI
        joystick         = capi::DMI_PORT_TYPE_JOYSTICK,         ///< Joystick
        keyboard         = capi::DMI_PORT_TYPE_KEYBOARD,         ///< Keyboard
        mouse            = capi::DMI_PORT_TYPE_MOUSE,            ///< Mouse
        scsi_ssa         = capi::DMI_PORT_TYPE_SCSI_SSA,         ///< SSA SCSI
        usb              = capi::DMI_PORT_TYPE_USB,              ///< USB
        firewire         = capi::DMI_PORT_TYPE_FIREWIRE,         ///< FireWire (IEEE P1394)
        pcmcia_i         = capi::DMI_PORT_TYPE_PCMCIA_I,         ///< PCMCIA type I
        pcmcia_ii        = capi::DMI_PORT_TYPE_PCMCIA_II,        ///< PCMCIA type II
        pcmcia_iii       = capi::DMI_PORT_TYPE_PCMCIA_III,       ///< PCMCIA type III
        cardbus          = capi::DMI_PORT_TYPE_CARDBUS,          ///< CardBus
        access_bus       = capi::DMI_PORT_TYPE_ACCESS_BUS,       ///< ACCESS.bus
        scsi_2           = capi::DMI_PORT_TYPE_SCSI_2,           ///< SCSI-2
        scsi_wide        = capi::DMI_PORT_TYPE_SCSI_WIDE,        ///< Wide SCSI
        pc_98            = capi::DMI_PORT_TYPE_PC_98,            ///< PC-98
        pc_98_hireso     = capi::DMI_PORT_TYPE_PC_98_HIRESO,     ///< PC-98-Hireso
        pc_h98           = capi::DMI_PORT_TYPE_PC_H98,           ///< PC-H98
        video            = capi::DMI_PORT_TYPE_VIDEO,            ///< Video
        audio            = capi::DMI_PORT_TYPE_AUDIO,            ///< Audio
        modem            = capi::DMI_PORT_TYPE_MODEM,            ///< Modem
        network          = capi::DMI_PORT_TYPE_NETWORK,          ///< Network
        sata             = capi::DMI_PORT_TYPE_SATA,             ///< SATA
        sas              = capi::DMI_PORT_TYPE_SAS,              ///< SAS
        mfdp             = capi::DMI_PORT_TYPE_MFDP,             ///< MFDP (Multi-Function Display Port)
        thunderbolt      = capi::DMI_PORT_TYPE_THUNDERBOLT,      ///< Thunderbolt
        c8251            = capi::DMI_PORT_TYPE_C8251,            ///< 8251 compatible
        c8251_fifo       = capi::DMI_PORT_TYPE_C8251_FIFO,       ///< 8251 FIFO compatible
        other            = capi::DMI_PORT_TYPE_OTHER,            ///< Other
    };

    class port_connector
    {
    public:
        const std::string& internal_designator;
        const connector_type& internal_connector;
        const std::string& external_designator;
        const connector_type& external_connector;
        const port_type& type;

        port_connector(const capi::dmi_port_connector_t *info)
            : internal_designator(m_internal_designator)
            , internal_connector(m_internal_connector)
            , external_designator(m_external_designator)
            , external_connector(m_external_connector)
            , type(m_type)
        {
            m_internal_designator = info->internal_designator;
            m_internal_connector  = static_cast<connector_type>(info->internal_connector);
            m_external_designator = info->external_designator;
            m_external_connector  = static_cast<connector_type>(info->external_connector);
            m_type                = static_cast<port_type>(info->port_type);
        }

    private:
        std::string m_internal_designator;
        connector_type m_internal_connector;
        std::string m_external_designator;
        connector_type m_external_connector;
        port_type m_type;
    };
}

#endif // !OPENDMI_ENTITY_PORT_CONNECTOR_HH
