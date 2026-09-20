//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/port-connector.h>

static bool dmi_port_connector_decode(dmi_entity_t *entity);

static const dmi_name_set_t dmi_connector_type_names =
{
    .code  = "port-connector-type",
    .names = (dmi_name_t[]){
        DMI_NAME_NONE(DMI_CONNECTOR_TYPE_NONE),
        {
            .id   = DMI_CONNECTOR_TYPE_CN_36,
            .code = "cn-36",
            .name = "Centronics"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_HPCN_36,
            .code = "hpcn-36",
            .name = "Mini-Centronics"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_PROPRIETARY,
            .code = "proprietary",
            .name = "Proprietary"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_DB_25_M,
            .code = "db-25-m",
            .name = "DB-25 male"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_DB_25_F,
            .code = "db-25-f",
            .name = "DB-25 female"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_DB_15_M,
            .code = "db-15-m",
            .name = "DB-15 male"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_DB_15_F,
            .code = "db-15-f",
            .name = "DB-15 female"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_DB_9_M,
            .code = "db-9-m",
            .name = "DB-9 male"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_DB_9_F,
            .code = "db-9-f",
            .name = "DB-9 female"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_RJ_11,
            .code = "rj-11",
            .name = "RJ-11"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_RJ_45,
            .code = "rj-45",
            .name = "RJ-45"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_MINI_SCSI_50PIN,
            .code = "mini-scsi-50pin",
            .name = "50-pin MiniSCSI"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_MINI_DIN,
            .code = "mini-din",
            .name = "Mini-DIN"
        },
        {
            .id = DMI_CONNECTOR_TYPE_MICRO_DIN,
            .code = "micro-din",
            .name = "Micro-DIN"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_PS2,
            .code = "ps-2",
            .name = "PS/2"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_INFRARED,
            .code = "infrared",
            .name = "Infrared"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_HP_HIL,
            .code = "hp-hil",
            .name = "HP-HIL"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_ACCESS_BUS,
            .code = "access-bus",
            .name = "Access Bus (USB)"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_SSA_SCSI,
            .code = "ssa-scsi",
            .name = "SSA SCSI"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_CIRCULAR_DIN_8_M,
            .code = "circular-din-8-m",
            .name = "Circular DIN-8 male"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_CIRCULAR_DIN_8_F,
            .code = "circular-din-8-f",
            .name = "Circular DIN-8 female"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_ONBOARD_IDE,
            .code = "onboard-ide",
            .name = "On Board IDE"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_ONBOARD_FLOPPY,
            .code = "onboard-floppy",
            .name = "On Board Floppy"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_DIL_9PIN,
            .code = "dil-9pin",
            .name = "9-pin Dual Inline (pin 10 cut)"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_DIL_25PIN,
            .code = "dil-25-pin",
            .name = "25-pin Dual Inline (pin 26 cut)"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_DIL_50PIN,
            .code = "dil-50pin",
            .name = "50-pin Dual Inline"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_DIL_68PIN,
            .code = "dil-68pin",
            .name = "68-pin Dual Inline"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_ONBOARD_SOUND,
            .code = "onboard-sound",
            .name = "On Board Sound Input from CD-ROM"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_HPCN_14,
            .code = "hpcn-14",
            .name = "Mini-Centronics Type-14"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_HPCN_26,
            .code = "hpcn-26",
            .name = "Mini-Centronics Type-26"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_MINI_JACK,
            .code = "mini-jack",
            .name = "Mini-jack (headphones)"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_BNC,
            .code = "bnc",
            .name = "BNC"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_IEEE_1394,
            .code = "ieee-1394",
            .name = "IEEE 1394"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_SAS_SATA,
            .code = "sas-sata",
            .name = "SAS/SATA Plug Receptacle"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_USB_C,
            .code = "usb-c",
            .name = "USB Type-C Receptacle"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_PC_98,
            .code = "pc-98",
            .name = "PC-98"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_PC_98_HIRESO,
            .code = "pc-98-hireso",
            .name = "PC-98 Hireso"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_PC_H98,
            .code = "pc-h98",
            .name = "PC-H98"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_PC_98_NOTE,
            .code = "pc-98-note",
            .name = "PC-98 Note"
        },
        {
            .id   = DMI_CONNECTOR_TYPE_PC_98_FULL,
            .code = "pc-98-full",
            .name = "PC-98 Full"
        },
        DMI_NAME_OTHER(DMI_CONNECTOR_TYPE_OTHER),
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_port_type_names =
{
    .code  = "port-type",
    .names = (dmi_name_t[]){
        DMI_NAME_NONE(DMI_PORT_TYPE_NONE),
        {
            .id   = DMI_PORT_TYPE_PARALLEL_XT_AT,
            .code = "parallel-xt-at",
            .name = "Parallel XT/AT compatible"
        },
        {
            .id   = DMI_PORT_TYPE_PARALLEL_PS2,
            .code = "parallel-ps2",
            .name = "Parallel PS/2"
        },
        {
            .id   = DMI_PORT_TYPE_PARALLEL_ECP,
            .code = "parallel-ecp",
            .name = "Parallel ECP"
        },
        {
            .id   = DMI_PORT_TYPE_PARALLEL_EPP,
            .code = "parallel-epp",
            .name = "Parallel EPP"
        },
        {
            .id   = DMI_PORT_TYPE_PARALLEL_ECP_EPP,
            .code = "parallel-ecp-epp",
            .name = "Parallel ECP/EPP"
        },
        {
            .id   = DMI_PORT_TYPE_SERIAL_XT_AT,
            .code = "serial-xt-at",
            .name = "Serial XT/AT compatible"
        },
        {
            .id   = DMI_PORT_TYPE_SERIAL_16450,
            .code = "serial-16450",
            .name = "Serial 16450 compatible"
        },
        {
            .id   = DMI_PORT_TYPE_SERIAL_16550,
            .code = "serial-16550",
            .name = "Serial 16550 compatible"
        },
        {
            .id   = DMI_PORT_TYPE_SERIAL_16550A,
            .code = "serial-16550a",
            .name = "Serial 16550A compatible"
        },
        {
            .id   = DMI_PORT_TYPE_SCSI,
            .code = "scsi",
            .name = "SCSI"
        },
        {
            .id   = DMI_PORT_TYPE_MIDI,
            .code = "midi",
            .name = "MIDI"
        },
        {
            .id   = DMI_PORT_TYPE_JOYSTICK,
            .code = "joystick",
            .name = "Joystick"
        },
        {
            .id   = DMI_PORT_TYPE_KEYBOARD,
            .code = "keyboard",
            .name = "Keyboard"
        },
        {
            .id   = DMI_PORT_TYPE_MOUSE,
            .code = "mouse",
            .name = "Mouse"
        },
        {
            .id   = DMI_PORT_TYPE_SCSI_SSA,
            .code = "ssa-scsi",
            .name = "SSA SCSI"
        },
        {
            .id   = DMI_PORT_TYPE_USB,
            .code = "usb",
            .name = "USB"
        },
        {
            .id   = DMI_PORT_TYPE_FIREWIRE,
            .code = "firewire",
            .name = "FireWire (IEEE P1394)"
        },
        {
            .id   = DMI_PORT_TYPE_PCMCIA_I,
            .code = "pcmcia-i",
            .name = "PCMCIA type I"
        },
        {
            .id   = DMI_PORT_TYPE_PCMCIA_II,
            .code = "pcmcia-ii",
            .name = "PCMCIA type II"
        },
        {
            .id   = DMI_PORT_TYPE_PCMCIA_III,
            .code = "pcmcia-iii",
            .name = "PCMCIA type III"
        },
        {
            .id   = DMI_PORT_TYPE_CARDBUS,
            .code = "cardbus",
            .name = "CardBus"
        },
        {
            .id   = DMI_PORT_TYPE_ACCESS_BUS,
            .code = "access-bus",
            .name = "ACCESS.bus"
        },
        {
            .id   = DMI_PORT_TYPE_SCSI_2,
            .code = "scsi-2",
            .name = "SCSI-2"
        },
        {
            .id   = DMI_PORT_TYPE_SCSI_WIDE,
            .code = "wide-scsi",
            .name = "Wide SCSI"
        },
        {
            .id   = DMI_PORT_TYPE_PC_98,
            .code = "pc-98",
            .name = "PC-98"
        },
        {
            .id   = DMI_PORT_TYPE_PC_98_HIRESO,
            .code = "pc-98-hireso",
            .name = "PC-98-Hireso"
        },
        {
            .id   = DMI_PORT_TYPE_PC_H98,
            .code = "pc-h98",
            .name = "PC-H98"
        },
        {
            .id   = DMI_PORT_TYPE_VIDEO,
            .code = "video",
            .name = "Video"
        },
        {
            .id   = DMI_PORT_TYPE_AUDIO,
            .code = "audio",
            .name = "Audio"
        },
        {
            .id   = DMI_PORT_TYPE_MODEM,
            .code = "modem",
            .name = "Modem"
        },
        {
            .id   = DMI_PORT_TYPE_NETWORK,
            .code = "network",
            .name = "Network"
        },
        {
            .id   = DMI_PORT_TYPE_SATA,
            .code = "sata",
            .name = "SATA"
        },
        {
            .id   = DMI_PORT_TYPE_SAS,
            .code = "sas",
            .name = "SAS"
        },
        {
            .id   = DMI_PORT_TYPE_MFDP,
            .code = "mfdp",
            .name = "MFDP (Multi-Function Display Port)"
        },
        {
            .id   = DMI_PORT_TYPE_THUNDERBOLT,
            .code = "thunderbolt",
            .name = "Thunderbolt"
        },
        {
            .id   = DMI_PORT_TYPE_8251,
            .code = "8251",
            .name = "8251 compatible"
        },
        {
            .id   = DMI_PORT_TYPE_8251_FIFO,
            .code = "8251-fifo",
            .name = "8251 FIFO compatible"
        },
        DMI_NAME_OTHER(DMI_PORT_TYPE_OTHER),
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_port_connector_spec =
{
    .code            = "port-connector",
    .name            = "Port connector information",
    .description     = (const char *[]){
        "The information in this structure defines the attributes of a system "
        "port connector (for example, parallel, serial, keyboard, or mouse "
        "ports). The port’s type and connector information are provided. One "
        "structure is present for each port provided by the system.",
        //
        nullptr
    },
    .type            = DMI_TYPE(PORT_CONNECTOR),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x09,
    .decoded_length  = sizeof(dmi_port_connector_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_port_connector_t, internal_designator, STRING, {
            .code   = "internal-designator",
            .name   = "Internal reference designator"
        }),
        DMI_ATTRIBUTE(dmi_port_connector_t, internal_connector, ENUM, {
            .code   = "internal-connector",
            .name   = "Internal connector type",
            .values = &dmi_connector_type_names
        }),
        DMI_ATTRIBUTE(dmi_port_connector_t, external_designator, STRING, {
            .code   = "external-designator",
            .name   = "External reference designator"
        }),
        DMI_ATTRIBUTE(dmi_port_connector_t, external_connector, ENUM, {
            .code   = "external-connector",
            .name   = "External connector type",
            .values = &dmi_connector_type_names
        }),
        DMI_ATTRIBUTE(dmi_port_connector_t, port_type, ENUM, {
            .code   = "port-type",
            .name   = "Port type",
            .values = &dmi_port_type_names
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers   = {
        .decode = dmi_port_connector_decode
    }
};

const char *dmi_connector_type_name(dmi_connector_type_t value)
{
    return dmi_name_lookup(&dmi_connector_type_names, (int)value);
}

const char *dmi_port_type_name(dmi_port_type_t value)
{
    return dmi_name_lookup(&dmi_port_type_names, (int)value);
}

static bool dmi_port_connector_decode(dmi_entity_t *entity)
{
    dmi_port_connector_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(PORT_CONNECTOR));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = dmi_entity_stream(entity);

    return
        dmi_stream_decode_str(stream, &info->internal_designator) and
        dmi_stream_decode(stream, dmi_byte_t, &info->internal_connector) and
        dmi_stream_decode_str(stream, &info->external_designator) and
        dmi_stream_decode(stream, dmi_byte_t, &info->external_connector) and
        dmi_stream_decode(stream, dmi_byte_t, &info->port_type);
}
