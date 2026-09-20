//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/slot.h>

static bool dmi_slot_decode(dmi_entity_t *entity);
static void dmi_slot_cleanup(dmi_entity_t *entity);

static const dmi_name_set_t dmi_slot_type_names =
{
    .code  = "slot-type",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_SLOT_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_SLOT_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_SLOT_TYPE_UNKNOWN),
        {
            .id   = DMI_SLOT_TYPE_ISA,
            .code = "isa",
            .name = "ISA"
        },
        {
            .id   = DMI_SLOT_TYPE_MCA,
            .code = "mca",
            .name = "MCA"
        },
        {
            .id   = DMI_SLOT_TYPE_EISA,
            .code = "eisa",
            .name = "EISA"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI,
            .code = "pci",
            .name = "PCI"
        },
        {
            .id   = DMI_SLOT_TYPE_PCMCIA,
            .code = "pcmcia",
            .name = "PC card (PCMCIA)"
        },
        {
            .id   = DMI_SLOT_TYPE_VESA,
            .code = "vesa",
            .name = "VL-VESA"
        },
        {
            .id   = DMI_SLOT_TYPE_PROPRIETARY,
            .code = "proprietary",
            .name = "Proprietary"
        },
        {
            .id   = DMI_SLOT_TYPE_PROCESSOR_CARD,
            .code = "processor-card",
            .name = "Processor card slot"
        },
        {
            .id   = DMI_SLOT_TYPE_MEMORY_CARD,
            .code = "memory-card",
            .name = "Memory card slot"
        },
        {
            .id   = DMI_SLOT_TYPE_IO_RISER_CARD,
            .code = "io-riser-card",
            .name = "I/O riser card slot"
        },
        {
            .id   = DMI_SLOT_TYPE_NUBUS,
            .code = "nubus",
            .name = "NuBus"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_66,
            .code = "pci-66",
            .name = "PCI 66 Mhz"
        },
        {
            .id   = DMI_SLOT_TYPE_AGP,
            .code = "agp",
            .name = "AGP"
        },
        {
            .id   = DMI_SLOT_TYPE_AGP_2X,
            .code = "agp-2x",
            .name = "AGP 2x"
        },
        {
            .id   = DMI_SLOT_TYPE_AGP_4X,
            .code = "agp-4x",
            .name = "AGP 4x"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_X,
            .code = "pci-x",
            .name = "PCI-X"
        },
        {
            .id   = DMI_SLOT_TYPE_AGP_8X,
            .code = "agp-8x",
            .name = "AGP 8x"
        },
        {
            .id   = DMI_SLOT_TYPE_M2_S1_DP,
            .code = "m2-s1-dp",
            .name = "M.2 Socket 1-DP (Mechanical key A)"
        },
        {
            .id   = DMI_SLOT_TYPE_M2_S1_SD,
            .code = "m2-s1-sd",
            .name = "M.2 Socket 1-SD (Mechanical key E)"
        },
        {
            .id   = DMI_SLOT_TYPE_M2_S2,
            .code = "m2-s2",
            .name = "M.2 Socket 2 (Mechanical key B)"
        },
        {
            .id   = DMI_SLOT_TYPE_M2_S3,
            .code = "m2-s3",
            .name = "M.2 Socket 3 (Mechanical key M)"
        },
        {
            .id   = DMI_SLOT_TYPE_MXM_I,
            .code = "mxm-i",
            .name = "MXM Type I"
        },
        {
            .id   = DMI_SLOT_TYPE_MXM_II,
            .code = "mxm-iii",
            .name = "MXM Type II"
        },
        {
            .id   = DMI_SLOT_TYPE_MXM_III_STD,
            .code = "mxm-iii-std",
            .name = "MXM Type III (Standard connector)"
        },
        {
            .id   = DMI_SLOT_TYPE_MXM_III_HE,
            .code = "mxm-iii-he",
            .name = "MXM Type III (HE connector)"
        },
        {
            .id   = DMI_SLOT_TYPE_MXM_IV,
            .code = "mxm-iv",
            .name = "MXM Type IV"
        },
        {
            .id   = DMI_SLOT_TYPE_MXM_30_A,
            .code = "mxm-3.0-a",
            .name = "MXM 3.0 Type A"
        },
        {
            .id   = DMI_SLOT_TYPE_MXM_30_B,
            .code = "mxm-3.0-b",
            .name = "MXM 3.0 Type B"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G2_SFF8639,
            .code = "pci-e-g2-sff8639",
            .name = "PCI Express Gen 2 SFF-8639 (U.2)"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G3_SFF8639,
            .code = "pci-e-g3-sff8639",
            .name = "PCI Express Gen 3 SFF-8639 (U.2)"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_MINI_52PIN_BSKO,
            .code = "pci-e-mini-52pin-bsko",
            .name = "PCI Express Mini 52-pin (CEM spec. 2.0) with bottom-side keep-outs"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_MINI_52PIN,
            .code = "pci-e-mini-52pin",
            .name = "PCI Express Mini 52-pin (CEM spec. 2.0) without bottom-side keep-outs"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_MINI_76PIN,
            .code = "pci-e-mini-76pin",
            .name = "PCI Express Mini 76-pin (CEM spec. 2.0)"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G4_SFF8639,
            .code = "pci-e-g4-sff8639",
            .name = "PCI Express Gen 4 SFF-8639 (U.2)"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G5_SFF8639,
            .code = "pci-e-g5-sff8639",
            .name = "PCI Express Gen 5 SFF-8639 (U.2)"
        },
        {
            .id   = DMI_SLOT_TYPE_OCP_NIC_30_SFF,
            .code = "ocp-nic-3.0-sff",
            .name = "OCP NIC 3.0 Small Form Factor (SFF)"
        },
        {
            .id   = DMI_SLOT_TYPE_OCP_NIC_30_LFF,
            .code = "ocp-nic-3.0-lff",
            .name = "OCP NIC 3.0 Large Form Factor (LFF)"
        },
        {
            .id   = DMI_SLOT_TYPP_OCP_NIC_LEGACY,
            .code = "ocp-nic-legacy",
            .name = "OCP NIC prior to 3.0"
        },
        {
            .id   = DMI_SLOT_TYPE_CXL_FLEXBUS_10,
            .code = "cxl-flexbus-1.0",
            .name = "CXL FlexBus 1.0"
        },
        {
            .id   = DMI_SLOT_TYPE_PC_98_C20,
            .code = "pc-98-c20",
            .name = "PC-98/C20"
        },
        {
            .id   = DMI_SLOT_TYPE_PC_98_C24,
            .code = "pc-98-c24",
            .name = "PC-98/C24"
        },
        {
            .id   = DMI_SLOT_TYPE_PC_98_E,
            .code = "pc-98-e",
            .name = "PC-98/E"
        },
        {
            .id   = DMI_SLOT_TYPE_PC_98_LOCAL_BUS,
            .code = "pc-98-local-bus",
            .name = "PC-98/Local bus"
        },
        {
            .id   = DMI_SLOT_TYPE_PC_98_CARD,
            .code = "pc-98-card",
            .name = "PC-98/Card"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E,
            .code = "pci-e",
            .name = "PCI Express"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_X1,
            .code = "pci-e-x1",
            .name = "PCI Express x1"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_X2,
            .code = "pci-e-x2",
            .name = "PCI Express x2"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_X4,
            .code = "pci-e-x4",
            .name = "PCI Express x4"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_X8,
            .code = "pci-e-x8",
            .name = "PCI Express x8"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_X16,
            .code = "pci-e-x16",
            .name = "PCI Express x16"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G2,
            .code = "pci-e-g2",
            .name = "PCI Express Gen 2"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G2_X1,
            .code = "pci-e-g2-x1",
            .name = "PCI Express Gen 2 x1"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G2_X2,
            .code = "pci-e-g2-x2",
            .name = "PCI Express Gen 2 x2"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G2_X4,
            .code = "pci-e-g2-x4",
            .name = "PCI Express Gen 2 x4"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G2_X8,
            .code = "pci-e-g2-x8",
            .name = "PCI Express Gen 2 x8"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G2_X16,
            .code = "pci-e-g2-x16",
            .name = "PCI Express Gen 2 x16"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G3,
            .code = "pci-e-g3",
            .name = "PCI Express Gen 3"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G3_X1,
            .code = "pci-e-g3-x1",
            .name = "PCI Express Gen 3 x1"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G3_X2,
            .code = "pci-e-g3-x2",
            .name = "PCI Express Gen 3 x2"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G3_X4,
            .code = "pci-e-g3-x4",
            .name = "PCI Express Gen 3 x4"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G3_X8,
            .code = "pci-e-g3-x8",
            .name = "PCI Express Gen 3 x8"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G3_X16,
            .code = "pci-e-g3-x16",
            .name = "PCI Express Gen 3 x16"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G4,
            .code = "pci-e-g4",
            .name = "PCI Express Gen 4"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G4_X1,
            .code = "pci-e-g4-x1",
            .name = "PCI Express Gen 4 x1"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G4_X2,
            .code = "pci-e-g4-x2",
            .name = "PCI Express Gen 4 x2"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G4_X4,
            .code = "pci-e-g4-x4",
            .name = "PCI Express Gen 4 x4"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G4_X8,
            .code = "pci-e-g4-x8",
            .name = "PCI Express Gen 4 x8"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G4_X16,
            .code = "pci-e-g4-x16",
            .name = "PCI Express Gen 4 x16"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G5,
            .code = "pci-e-g5",
            .name = "PCI Express Gen 5"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G5_X1,
            .code = "pci-e-g5-x1",
            .name = "PCI Express Gen 5 x1"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G5_X2,
            .code = "pci-e-g5-x2",
            .name = "PCI Express Gen 5 x2"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G5_X4,
            .code = "pci-e-g5-x4",
            .name = "PCI Express Gen 5 x4"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G5_X8,
            .code = "pci-e-g5-x8",
            .name = "PCI Express Gen 5 x8"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G5_X16,
            .code = "pci-e-g5-x16",
            .name = "PCI Express Gen 5 x16"
        },
        {
            .id   = DMI_SLOT_TYPE_PCI_E_G6,
            .code = "pci-e-g6",
            .name = "PCI Express Gen 6 and beyond"
        },
        {
            .id   = DMI_SLOT_TYPE_EDSFF_E1,
            .code = "edsff-e1",
            .name = "Enterprise and datacenter 1U E1 form factor slot (EDSFF E1.S, E1.L)"
        },
        {
            .id   = DMI_SLOT_TYPE_EDSFF_E3,
            .code = "edsff-e3",
            .name = "Enterprise and datacenter 3\"} E3 form factor slot (EDSFF E3.S, E3.L)"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_slot_width_names =
{
    .code  = "slot-width",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_SLOT_WIDTH_UNSPEC),
        DMI_NAME_OTHER(DMI_SLOT_WIDTH_OTHER),
        DMI_NAME_UNKNOWN(DMI_SLOT_WIDTH_UNKNOWN),
        {
            .id   = DMI_SLOT_WIDTH_8_BIT,
            .code = "8-bit",
            .name = "8 bit"
        },
        {
            .id   = DMI_SLOT_WIDTH_16_BIT,
            .code = "16-bit",
            .name = "16 bit"
        },
        {
            .id   = DMI_SLOT_WIDTH_32_BIT,
            .code = "32-bit",
            .name = "32 bit"
        },
        {
            .id   = DMI_SLOT_WIDTH_64_BIT,
            .code = "64-bit",
            .name = "64 bit"
        },
        {
            .id   = DMI_SLOT_WIDTH_128_BIT,
            .code = "128-bit",
            .name = "128 bit"
        },
        {
            .id   = DMI_SLOT_WIDTH_1X,
            .code = "1x",
            .name = "1x or x1"
        },
        {
            .id   = DMI_SLOT_WIDTH_2X,
            .code = "2x",
            .name = "2x or x2"
        },
        {
            .id   = DMI_SLOT_WIDTH_4X,
            .code = "4x",
            .name = "4x or x4"
        },
        {
            .id   = DMI_SLOT_WIDTH_8X,
            .code = "8x",
            .name = "8x or x8"
        },
        {
            .id   = DMI_SLOT_WIDTH_12X,
            .code = "12x",
            .name = "12x or x12"
        },
        {
            .id   = DMI_SLOT_WIDTH_16X,
            .code = "16x",
            .name = "16x or x16"
        },
        {
            .id   = DMI_SLOT_WIDTH_32X,
            .code = "32x",
            .name = "32x or x32"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_slot_length_names =
{
    .code  = "slot-length",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_SLOT_LENGTH_UNSPEC),
        DMI_NAME_OTHER(DMI_SLOT_LENGTH_OTHER),
        DMI_NAME_UNKNOWN(DMI_SLOT_LENGTH_UNKNOWN),
        {
            .id   = DMI_SLOT_LENGTH_SHORT,
            .code = "short",
            .name = "Short length"
        },
        {
            .id   = DMI_SLOT_LENGTH_LONG,
            .code = "long",
            .name = "Long length"
        },
        {
            .id   = DMI_SLOT_LENGTH_SFF_8200,
            .code = "sff-8200",
            .name = "2.5\" drive form factor"
        },
        {
            .id   = DMI_SLOT_LENGTH_SFF_8300,
            .code = "sff-8300",
            .name = "3.5\" drive form factor"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_slot_feature_names =
{
    .code  = "slot-feature",
    .names = (dmi_name_t[]){
        DMI_NAME_UNKNOWN(0),
        {
            .id   = 1,
            .code = "has-5v0-support",
            .name = "Provides 5.0 volts"
        },
        {
            .id   = 2,
            .code = "has-3v3-support",
            .name = "Provides 3.3 volts"
        },
        {
            .id   = 3,
            .code = "has-shared-opening",
            .name = "Slot’s opening is shared with another slot"
        },
        {
            .id   = 4,
            .code = "has-pccard-16-support",
            .name = "PC Card slot supports PC Card-16"
        },
        {
            .id   = 5,
            .code = "has-cardbus-support",
            .name = "PC Card slot supports CardBus"
        },
        {
            .id   = 6,
            .code = "has-zoom-video-support",
            .name = "PC Card slot supports Zoom Video"
        },
        {
            .id   = 7,
            .code = "has-modem-ring-resume-support",
            .name = "PC Card slot supports Modem Ring Resume"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_slot_feature_ex_names =
{
    .code  = "slot-features-ex",
    .names = (dmi_name_t[]){
        {
            .id   = 0,
            .code = "has-pme-support",
            .name = "PCI slot supports PME# signal"
        },
        {
            .id   = 1,
            .code = "has-hotplug-support",
            .name = "Slot supports hot-plug devices"
        },
        {
            .id   = 2,
            .code = "has-smbus-support",
            .name = "PCI slot supports SMBus signal"
        },
        {
            .id   = 3,
            .code = "has-bifurcation-support",
            .name = "PCIe slot supports bifurcation"
        },
        {
            .id = 4,
            .code = "has-async-removal-support",
            .name = "Slot supports async/surprise removal"
        },
        {
            .id   = 5,
            .code = "is-cxl-10-capable",
            .name = "Flexbus slot, CXL 1.0 capable"
        },
        {
            .id   = 6,
            .code = "is-cxl-20-capable",
            .name = "Flexbus slot, CXL 2.0 capable"
        },
        {
            .id   = 7,
            .code = "is-cxl-30-capable",
            .name = "Flexbus slot, CXL 3.0 capable"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_slot_height_names =
{
    .code  = "slot-height",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_SLOT_HEIGHT_UNSPEC),
        DMI_NAME_OTHER(DMI_SLOT_HEIGHT_OTHER),
        DMI_NAME_UNKNOWN(DMI_SLOT_HEIGHT_UNKNOWN),
        {
            .id   = DMI_SLOT_HEIGHT_FULL,
            .code = "full",
            .name = "Full height"
        },
        {
            .id   = DMI_SLOT_HEIGHT_LOW_PROFILE,
            .code = "low-profile",
            .name = "Low-profile"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_slot_usage_names =
{
    .code  = "slot-usage",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_SLOT_USAGE_UNSPEC),
        DMI_NAME_OTHER(DMI_SLOT_USAGE_OTHER),
        DMI_NAME_UNKNOWN(DMI_SLOT_USAGE_UNKNOWN),
        {
            .id   = DMI_SLOT_USAGE_AVAILABLE,
            .code = "available",
            .name = "Available"
        },
        {
            .id   = DMI_SLOT_USAGE_IN_USE,
            .code = "in-use",
            .name = "In use"
        },
        {
            .id   = DMI_SLOT_USAGE_UNAVAILABLE,
            .code = "unavailable",
            .name = "Unavailable"
        },
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_slot_spec =
{
    .code            = "slot",
    .name            = "System slots",
    .description     = (const char *[]){
        "The information in this structure defines the attributes of a system "
        "slot. One structure is provided for each slot in the system.",
        //
        nullptr
    },
    .type            = DMI_TYPE(SYSTEM_SLOTS),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x0C,
    .decoded_length  = sizeof(dmi_slot_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_slot_t, designator, STRING, {
            .code = "designator",
            .name = "Designator"
        }),
        DMI_ATTRIBUTE(dmi_slot_t, type, ENUM, {
            .code    = "type",
            .name    = "Type",
            .unspec  = dmi_value_ptr(DMI_SLOT_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_SLOT_TYPE_UNKNOWN),
            .values  = &dmi_slot_type_names
        }),
        DMI_ATTRIBUTE(dmi_slot_t, bus_width, ENUM, {
            .code    = "bus-width",
            .name    = "Data bus width",
            .unspec  = dmi_value_ptr(DMI_SLOT_WIDTH_UNSPEC),
            .unknown = dmi_value_ptr(DMI_SLOT_WIDTH_UNKNOWN),
            .values  = &dmi_slot_width_names
        }),
        DMI_ATTRIBUTE(dmi_slot_t, usage, ENUM, {
            .code    = "usage",
            .name    = "Current usage",
            .unspec  = dmi_value_ptr(DMI_SLOT_USAGE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_SLOT_USAGE_UNKNOWN),
            .values  = &dmi_slot_usage_names
        }),
        DMI_ATTRIBUTE(dmi_slot_t, length, ENUM, {
            .code    = "length",
            .name    = "Slot length",
            .unspec  = dmi_value_ptr(DMI_SLOT_LENGTH_UNSPEC),
            .unknown = dmi_value_ptr(DMI_SLOT_LENGTH_UNKNOWN),
            .values  = &dmi_slot_length_names
        }),
        DMI_ATTRIBUTE(dmi_slot_t, ident, INTEGER, {
            .code    = "ident",
            .name    = "Identifier",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_slot_t, features, SET, {
            .code    = "features",
            .name    = "Characteristics",
            .values  = &dmi_slot_feature_names
        }),
        DMI_ATTRIBUTE(dmi_slot_t, features_ex, SET, {
            .code    = "features-ex",
            .name    = "Extended characteristics",
            .values  = &dmi_slot_feature_ex_names
        }),
        DMI_ATTRIBUTE(dmi_slot_t, base_address, STRUCT, {
            .code    = "base-address",
            .name    = "Base device address",
            .attrs   = dmi_pci_addr_attrs
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_slot_t, peer_groups, peer_group_count, STRUCT, {
            .code = "peer-groups",
            .name = "Peer groups",
            .attrs = (const dmi_attribute_t[]){
                DMI_ATTRIBUTE(dmi_slot_peer_group_t, address, STRUCT, {
                    .code  = "address",
                    .name  = "Peer address",
                    .attrs = dmi_pci_addr_attrs
                }),
                DMI_ATTRIBUTE(dmi_slot_peer_group_t, bus_width, ENUM, {
                    .code    = "bus-width",
                    .name    = "Data bus width",
                    .unspec  = dmi_value_ptr(DMI_SLOT_WIDTH_UNSPEC),
                    .unknown = dmi_value_ptr(DMI_SLOT_WIDTH_UNKNOWN),
                    .values  = &dmi_slot_width_names
                }),
                DMI_ATTRIBUTE_NULL
            }
        }),
        DMI_ATTRIBUTE(dmi_slot_t, information, INTEGER, {
            .code    = "information",
            .name    = "Information",
            .unspec  = dmi_value_ptr((uint8_t)0)
        }),
        DMI_ATTRIBUTE(dmi_slot_t, physical_width, ENUM, {
            .code    = "physical-width",
            .name    = "Physical width",
            .unspec  = dmi_value_ptr(DMI_SLOT_WIDTH_UNSPEC),
            .unknown = dmi_value_ptr(DMI_SLOT_WIDTH_UNKNOWN),
            .values  = &dmi_slot_width_names
        }),
        DMI_ATTRIBUTE(dmi_slot_t, pitch, DECIMAL, {
            .code    = "pitch",
            .name    = "Pitch",
            .scale   = 2,
            .unit    = DMI_UNIT_MILLIMETER,
            .unspec  = dmi_value_ptr((uint16_t)0)
        }),
        DMI_ATTRIBUTE(dmi_slot_t, height, ENUM, {
            .code    = "height",
            .name    = "Height",
            .unspec  = dmi_value_ptr(DMI_SLOT_HEIGHT_UNSPEC),
            .unknown = dmi_value_ptr(DMI_SLOT_HEIGHT_UNKNOWN),
            .values  = &dmi_slot_height_names
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode  = dmi_slot_decode,
        .cleanup = dmi_slot_cleanup
    }
};

const char *dmi_slot_type_name(dmi_slot_type_t value)
{
    return dmi_name_lookup(&dmi_slot_type_names, (int)value);
}

const char *dmi_slot_width_name(dmi_slot_width_t value)
{
    return dmi_name_lookup(&dmi_slot_width_names, (int)value);
}

const char *dmi_slot_usage_name(dmi_slot_usage_t value)
{
    return dmi_name_lookup(&dmi_slot_usage_names, (int)value);
}

const char *dmi_slot_length_name(dmi_slot_length_t value)
{
    return dmi_name_lookup(&dmi_slot_length_names, (int)value);
}

const char *dmi_slot_height_name(dmi_slot_height_t value)
{
    return dmi_name_lookup(&dmi_slot_height_names, (int)value);
}

static bool dmi_slot_decode(dmi_entity_t *entity)
{
    bool status;
    dmi_slot_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_SLOTS));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);
    dmi_stream_t  *stream  = dmi_entity_stream(entity);

    status =
        dmi_stream_decode_str(stream, &info->designator) &&
        dmi_stream_decode(stream, dmi_byte_t, &info->type) &&
        dmi_stream_decode(stream, dmi_byte_t, &info->bus_width) &&
        dmi_stream_decode(stream, dmi_byte_t, &info->usage) &&
        dmi_stream_decode(stream, dmi_byte_t, &info->length) &&
        dmi_stream_decode(stream, dmi_word_t, &info->ident) &&
        dmi_stream_decode(stream, dmi_byte_t, &info->features);
    if (not status)
        return false;

    // SMBIOS 2.1 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 1, 0);

    if (not dmi_stream_decode(stream, dmi_byte_t, &info->features_ex))
        return dmi_entity_incomplete(entity);

    // SMBIOS 2.6 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 6, 0);

    if (not dmi_pci_addr_decode(stream, &info->base_address))
        return dmi_entity_incomplete(entity);

    // SMBIOS 3.2 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(3, 2, 0);

    dmi_byte_t peer_group_count = 0;

    status =
        dmi_stream_decode(stream, dmi_byte_t, &info->base_bus_width) &&
        dmi_stream_decode(stream, dmi_byte_t, &peer_group_count);
    if (not status)
        return dmi_entity_incomplete(entity);

    if (peer_group_count > 0) {
        info->peer_groups = dmi_alloc_array(context, sizeof(dmi_slot_peer_group_t), peer_group_count);
        if (info->peer_groups == nullptr)
            return false;
    }

    // Only completely present peer groups are counted
    for (size_t i = 0; i < peer_group_count; i++) {
        dmi_slot_peer_group_t *peer_group = &info->peer_groups[i];

        status =
            dmi_pci_addr_decode(stream, &peer_group->address) &&
            dmi_stream_decode(stream, dmi_byte_t, &peer_group->bus_width);
        if (not status)
            return dmi_entity_incomplete(entity);

        info->peer_group_count++;
    }

    // SMBIOS 3.4 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(3, 4, 0);

    status =
        dmi_stream_decode(stream, dmi_byte_t, &info->information) &&
        dmi_stream_decode(stream, dmi_byte_t, &info->physical_width) &&
        dmi_stream_decode(stream, dmi_word_t, &info->pitch);
    if (not status)
        return dmi_entity_incomplete(entity);

    // SMBIOS 3.5 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(3, 5, 0);

    if (not dmi_stream_decode(stream, dmi_byte_t, &info->height))
        return dmi_entity_incomplete(entity);

    return true;
}

static void dmi_slot_cleanup(dmi_entity_t *entity)
{
    dmi_slot_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_SLOTS));
    if (info == nullptr)
        return;

    dmi_free(info->peer_groups);
}
