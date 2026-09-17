//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SLOT_H
#define OPENDMI_ENTITY_SLOT_H

#include <opendmi/entity.h>
#include <opendmi/entity/common.h>

typedef struct dmi_slot dmi_slot_t;

typedef union dmi_slot_features dmi_slot_features_t;
typedef union dmi_slot_features_ex dmi_slot_features_ex_t;
typedef struct dmi_slot_peer_group dmi_slot_peer_group_t;

/**
 * @brief Slot type identifiers.
 */
typedef enum dmi_slot_type
{
    DMI_SLOT_TYPE_UNSPEC                = 0x00, ///< Unspecified
    DMI_SLOT_TYPE_OTHER                 = 0x01, ///< Other
    DMI_SLOT_TYPE_UNKNOWN               = 0x02, ///< Unknown
    DMI_SLOT_TYPE_ISA                   = 0x03, ///< ISA
    DMI_SLOT_TYPE_MCA                   = 0x04, ///< MCA
    DMI_SLOT_TYPE_EISA                  = 0x05, ///< EISA
    DMI_SLOT_TYPE_PCI                   = 0x06, ///< PCI
    DMI_SLOT_TYPE_PCMCIA                = 0x07, ///< PC card (PCMCIA)
    DMI_SLOT_TYPE_VESA                  = 0x08, ///< VL-VESA
    DMI_SLOT_TYPE_PROPRIETARY           = 0x09, ///< Proprietary
    DMI_SLOT_TYPE_PROCESSOR_CARD        = 0x0A, ///< Processor card slot
    DMI_SLOT_TYPE_MEMORY_CARD           = 0x0B, ///< Memory card slot
    DMI_SLOT_TYPE_IO_RISER_CARD         = 0x0C, ///< I/O riser card slot
    DMI_SLOT_TYPE_NUBUS                 = 0x0D, ///< NuBus
    DMI_SLOT_TYPE_PCI_66                = 0x0E, ///< PCI 66 Mhz
    DMI_SLOT_TYPE_AGP                   = 0x0F, ///< AGP
    DMI_SLOT_TYPE_AGP_2X                = 0x10, ///< AGP 2x
    DMI_SLOT_TYPE_AGP_4X                = 0x11, ///< AGP 4x
    DMI_SLOT_TYPE_PCI_X                 = 0x12, ///< PCI-X
    DMI_SLOT_TYPE_AGP_8X                = 0x13, ///< AGP 8x
    DMI_SLOT_TYPE_M2_S1_DP              = 0x14, ///< M.2 Socket 1-DP (Mechanical key A)
    DMI_SLOT_TYPE_M2_S1_SD              = 0x15, ///< M.2 Socket 1-SD (Mechanical key E)
    DMI_SLOT_TYPE_M2_S2                 = 0x16, ///< M.2 Socket 2 (Mechanical key B)
    DMI_SLOT_TYPE_M2_S3                 = 0x17, ///< M.2 Socket 3 (Mechanical key M)
    DMI_SLOT_TYPE_MXM_I                 = 0x18, ///< MXM Type I
    DMI_SLOT_TYPE_MXM_II                = 0x19, ///< MXM Type II
    DMI_SLOT_TYPE_MXM_III_STD           = 0x1A, ///< MXM Type III (Standard connector)
    DMI_SLOT_TYPE_MXM_III_HE            = 0x1B, ///< MXM Type III (HE connector)
    DMI_SLOT_TYPE_MXM_IV                = 0x1C, ///< MXM Type IV
    DMI_SLOT_TYPE_MXM_30_A              = 0x1D, ///< MXM 3.0 Type A
    DMI_SLOT_TYPE_MXM_30_B              = 0x1E, ///< MXM 3.0 Type B
    DMI_SLOT_TYPE_PCI_E_G2_SFF8639      = 0x1F, ///< PCI Express Gen 2 SFF-8639 (U.2)
    DMI_SLOT_TYPE_PCI_E_G3_SFF8639      = 0x20, ///< PCI Express Gen 3 SFF-8639 (U.2)
    DMI_SLOT_TYPE_PCI_E_MINI_52PIN_BSKO = 0x21, ///< PCI Express Mini 52-pin (CEM spec. 2.0) with bottom-side keep-outs
    DMI_SLOT_TYPE_PCI_E_MINI_52PIN      = 0x22, ///< PCI Express Mini 52-pin (CEM spec. 2.0) without bottom-side keep-outs
    DMI_SLOT_TYPE_PCI_E_MINI_76PIN      = 0x23, ///< PCI Express Mini 76-pin (CEM spec. 2.0)
    DMI_SLOT_TYPE_PCI_E_G4_SFF8639      = 0x24, ///< PCI Express Gen 4 SFF-8639 (U.2)
    DMI_SLOT_TYPE_PCI_E_G5_SFF8639      = 0x25, ///< PCI Express Gen 5 SFF-8639 (U.2)
    DMI_SLOT_TYPE_OCP_NIC_30_SFF        = 0x26, ///< OCP NIC 3.0 Small Form Factor (SFF)
    DMI_SLOT_TYPE_OCP_NIC_30_LFF        = 0x27, ///< OCP NIC 3.0 Large Form Factor (LFF)
    DMI_SLOT_TYPP_OCP_NIC_LEGACY        = 0x28, ///< OCP NIC prior to 3.0
    // Unassigned: 0x29 .. 0x2F
    DMI_SLOT_TYPE_CXL_FLEXBUS_10        = 0x30, ///< CXL FlexBus 1.0
    DMI_SLOT_TYPE_PC_98_C20             = 0xA0, ///< PC-98/C20
    DMI_SLOT_TYPE_PC_98_C24             = 0xA1, ///< PC-98/C24
    DMI_SLOT_TYPE_PC_98_E               = 0xA2, ///< PC-98/E
    DMI_SLOT_TYPE_PC_98_LOCAL_BUS       = 0xA3, ///< PC-98/Local bus
    DMI_SLOT_TYPE_PC_98_CARD            = 0xA4, ///< PC-98/Card
    DMI_SLOT_TYPE_PCI_E                 = 0xA5, ///< PCI Express
    DMI_SLOT_TYPE_PCI_E_X1              = 0xA6, ///< PCI Express x1
    DMI_SLOT_TYPE_PCI_E_X2              = 0xA7, ///< PCI Express x2
    DMI_SLOT_TYPE_PCI_E_X4              = 0xA8, ///< PCI Express x4
    DMI_SLOT_TYPE_PCI_E_X8              = 0xA9, ///< PCI Express x8
    DMI_SLOT_TYPE_PCI_E_X16             = 0xAA, ///< PCI Express x16
    DMI_SLOT_TYPE_PCI_E_G2              = 0xAB, ///< PCI Express Gen 2
    DMI_SLOT_TYPE_PCI_E_G2_X1           = 0xAC, ///< PCI Express Gen 2 x1
    DMI_SLOT_TYPE_PCI_E_G2_X2           = 0xAD, ///< PCI Express Gen 2 x2
    DMI_SLOT_TYPE_PCI_E_G2_X4           = 0xAE, ///< PCI Express Gen 2 x4
    DMI_SLOT_TYPE_PCI_E_G2_X8           = 0xAF, ///< PCI Express Gen 2 x8
    DMI_SLOT_TYPE_PCI_E_G2_X16          = 0xB0, ///< PCI Express Gen 2 x16
    DMI_SLOT_TYPE_PCI_E_G3              = 0xB1, ///< PCI Express Gen 3
    DMI_SLOT_TYPE_PCI_E_G3_X1           = 0xB2, ///< PCI Express Gen 3 x1
    DMI_SLOT_TYPE_PCI_E_G3_X2           = 0xB3, ///< PCI Express Gen 3 x2
    DMI_SLOT_TYPE_PCI_E_G3_X4           = 0xB4, ///< PCI Express Gen 3 x4
    DMI_SLOT_TYPE_PCI_E_G3_X8           = 0xB5, ///< PCI Express Gen 3 x8
    DMI_SLOT_TYPE_PCI_E_G3_X16          = 0xB6, ///< PCI Express Gen 3 x16
    // Unassigned: 0xB7
    DMI_SLOT_TYPE_PCI_E_G4              = 0xB8, ///< PCI Express Gen 4
    DMI_SLOT_TYPE_PCI_E_G4_X1           = 0xB9, ///< PCI Express Gen 4 x1
    DMI_SLOT_TYPE_PCI_E_G4_X2           = 0xBA, ///< PCI Express Gen 4 x2
    DMI_SLOT_TYPE_PCI_E_G4_X4           = 0xBB, ///< PCI Express Gen 4 x4
    DMI_SLOT_TYPE_PCI_E_G4_X8           = 0xBC, ///< PCI Express Gen 4 x8
    DMI_SLOT_TYPE_PCI_E_G4_X16          = 0xBD, ///< PCI Express Gen 4 x16
    DMI_SLOT_TYPE_PCI_E_G5              = 0xBE, ///< PCI Express Gen 5
    DMI_SLOT_TYPE_PCI_E_G5_X1           = 0xBF, ///< PCI Express Gen 5 x1
    DMI_SLOT_TYPE_PCI_E_G5_X2           = 0xC0, ///< PCI Express Gen 5 x2
    DMI_SLOT_TYPE_PCI_E_G5_X4           = 0xC1, ///< PCI Express Gen 5 x4
    DMI_SLOT_TYPE_PCI_E_G5_X8           = 0xC2, ///< PCI Express Gen 5 x8
    DMI_SLOT_TYPE_PCI_E_G5_X16          = 0xC3, ///< PCI Express Gen 5 x16
    DMI_SLOT_TYPE_PCI_E_G6              = 0xC4, ///< PCI Express Gen 6 and beyond
    DMI_SLOT_TYPE_EDSFF_E1              = 0xC5, ///< Enterprise and datacenter 1U E1 form factor slot (EDSFF E1.S, E1.L)
    DMI_SLOT_TYPE_EDSFF_E3              = 0xC6, ///< Enterprise and datacenter 3" E3 form factor slot (EDSFF E3.S, E3.L)
    __DMI_SLOT_TYPE_COUNT
} dmi_slot_type_t;

/**
 * @brief Slot data bus width values.
 */
typedef enum dmi_slot_width
{
    DMI_SLOT_WIDTH_UNSPEC  = 0x00, ///< Unspecified
    DMI_SLOT_WIDTH_OTHER   = 0x01, ///< Other
    DMI_SLOT_WIDTH_UNKNOWN = 0x02, ///< Unknown
    DMI_SLOT_WIDTH_8_BIT   = 0x03, ///< 8 bit
    DMI_SLOT_WIDTH_16_BIT  = 0x04, ///< 16 bit
    DMI_SLOT_WIDTH_32_BIT  = 0x05, ///< 32 bit
    DMI_SLOT_WIDTH_64_BIT  = 0x06, ///< 64 bit
    DMI_SLOT_WIDTH_128_BIT = 0x07, ///< 128 bit
    DMI_SLOT_WIDTH_1X      = 0x08, ///< 1x or x1
    DMI_SLOT_WIDTH_2X      = 0x09, ///< 2x or x2
    DMI_SLOT_WIDTH_4X      = 0x0A, ///< 4x or x4
    DMI_SLOT_WIDTH_8X      = 0x0B, ///< 8x or x8
    DMI_SLOT_WIDTH_12X     = 0x0C, ///< 12x or x12
    DMI_SLOT_WIDTH_16X     = 0x0D, ///< 16x or x16
    DMI_SLOT_WIDTH_32X     = 0x0E, ///< 32x or x32
    __DMI_SLOT_WIDTH_COUNT
} dmi_slot_width_t;

/**
 * @brief Slot usage values.
 */
typedef enum dmi_slot_usage
{
    DMI_SLOT_USAGE_UNSPEC      = 0x00, ///< Unspecified
    DMI_SLOT_USAGE_OTHER       = 0x01, ///< Other
    DMI_SLOT_USAGE_UNKNOWN     = 0x02, ///< Unknown
    DMI_SLOT_USAGE_AVAILABLE   = 0x03, ///< Available
    DMI_SLOT_USAGE_IN_USE      = 0x04, ///< In use
    DMI_SLOT_USAGE_UNAVAILABLE = 0x05, ///< Unavailable
    __DMI_SLOT_USAGE_COUNT
} dmi_slot_usage_t;

/**
 * @brief Slot length values.
 */
typedef enum dmi_slot_length
{
    DMI_SLOT_LENGTH_UNSPEC   = 0x00, ///< Unspecified
    DMI_SLOT_LENGTH_OTHER    = 0x01, ///< Other
    DMI_SLOT_LENGTH_UNKNOWN  = 0x02, ///< Unknown
    DMI_SLOT_LENGTH_SHORT    = 0x03, ///< Short length
    DMI_SLOT_LENGTH_LONG     = 0x04, ///< Long length
    DMI_SLOT_LENGTH_SFF_8200 = 0x05, ///< 2.5" drive form factor
    DMI_SLOT_LENGTH_SFF_8300 = 0x06, ///< 3.5" drive form factor
    __DMI_SLOT_LENGTH_COUNT
} dmi_slot_length_t;

/**
 * @brief Slot height values.
 */
typedef enum dmi_slot_height
{
    DMI_SLOT_HEIGHT_UNSPEC      = 0x00, ///< Unspecified or not applicable
    DMI_SLOT_HEIGHT_OTHER       = 0x01, ///< Other
    DMI_SLOT_HEIGHT_UNKNOWN     = 0x02, ///< Unknown
    DMI_SLOT_HEIGHT_FULL        = 0x03, ///< Full height
    DMI_SLOT_HEIGHT_LOW_PROFILE = 0x04, ///< Low-profile
    __DMI_SLOT_HEIGHT_COUNT
} dmi_slot_height_t;

/**
 * @brief System slot characteristics.
 */
dmi_packed_union(dmi_slot_features)
{
    /**
     * @brief Raw value.
     */
    dmi_byte_t __value;

    dmi_packed_struct()
    {
        /**
         * @brief Characteristics unknown.
         */
        dmi_byte_t is_unknown : 1;

        /**
         * @brief Provides 5.0 volts.
         */
        dmi_byte_t has_5v0_support : 1;

        /**
         * @brief Provides 3.3 volts.
         */
        dmi_byte_t has_3v3_support : 1;

        /**
         * @brief Slot’s opening is shared with another slot (e.g., PCI/EISA
         * shared slot).
         */
        dmi_byte_t has_shared_opening : 1;

        /**
         * @brief PC Card slot supports PC Card-16.
         */
        dmi_byte_t has_pccard_16_support : 1;

        /**
         * @brief PC Card slot supports CardBus.
         */
        dmi_byte_t has_cardbus_support : 1;

        /**
         * @brief PC Card slot supports Zoom Video.
         */
        dmi_byte_t has_zoom_video_supoort : 1;

        /**
         * @brief PC Card slot supports Modem Ring Resume.
         */
        dmi_byte_t has_modem_ring_resume_support : 1;
    };
};

dmi_static_assert_value_union(dmi_slot_features);

/**
 * @brief System slot extended characteristics.
 */
dmi_packed_union(dmi_slot_features_ex)
{
    /**
     * @brief Raw value.
     */
    dmi_byte_t __value;

    dmi_packed_struct()
    {
        /**
         * @brief PCI slot supports Power Management Event (PME#) signal.
         */
        dmi_byte_t has_pme_support : 1;

        /**
         * @brief Slot supports hot-plug devices.
         */
        dmi_byte_t has_hotplug_support : 1;

        /**
         * @brief PCI slot supports SMBus signal.
         */
        dmi_byte_t has_smbus_support : 1;

        /**
         * @brief PCIe slot supports bifurcation. This slot can partition its
         * lanes into two or more PCIe devices plugged into the slot.
         *
         * @note This field does not indicate complete details on what levels
         * of bifurcation are supported by the slot, but only that the slot
         * supports some level of bifurcation.
         */
        dmi_byte_t has_bifurcation_support : 1;

        /**
         * @brief Slot supports async/surprise removal, such as removal without
         * prior notification to the operating system, device driver, or
         * applications.
         */
        dmi_byte_t has_async_removal_support : 1;

        /**
         * @brief Flexbus slot, CXL 1.0 capable.
         */
        dmi_byte_t is_cxl_10_capable : 1;

        /**
         * @brief Flexbus slot, CXL 2.0 capable.
         */
        dmi_byte_t is_cxl_20_capable : 1;

        /**
         * @brief Flexbus slot, CXL 3.0 capable.
         */
        dmi_byte_t is_cxl_30_capable : 1;
    };
};

dmi_static_assert_value_union(dmi_slot_features_ex);

/**
 * @brief System slots structure (type 9).
 *
 * The information in this structure defines the attributes of a system slot.
 * One structure is provided for each slot in the system.
 */
struct dmi_slot
{
    /**
     * @brief Reference designator.
     */
    const char *designator;

    /**
     * @brief Slot type.
     */
    dmi_slot_type_t type;

    /**
     * @brief Data bus width.
     */
    dmi_slot_width_t bus_width;

    /**
     * @brief Current usage.
     */
    dmi_slot_usage_t usage;

    /**
     * @brief Slot length.
     */
    dmi_slot_length_t length;

    /**
     * @brief Slot identifier.
     * @todo Implement bus-specific identifier decoding (PCI, MCA, etc).
     */
    uint16_t ident;

    /**
     * @brief Slot characteristics.
     */
    dmi_slot_features_t features;

    /**
     * @brief Extended slot characteristics.
     */
    dmi_slot_features_ex_t features_ex;

    /**
     * @brief Base device address.
     */
    dmi_pci_addr_t base_address;

    /**
     * @brief Indicate electrical bus width of base Segment/Bus/Device/Function.
     */
    dmi_slot_width_t base_bus_width;

    /**
     * @brief Number of peer Segment/Bus/Device/Function/Width groups that
     * follow. Zero if no peer groups.
     */
    size_t peer_group_count;

    /**
     * @brief Peer Segment/Bus/Device/Function/Width present in the slot. This
     * field is @c nullptr if there are no peer groups (`peer_group_count` == 0).
     */
    dmi_slot_peer_group_t *peer_groups;

    /**
     * @brief Slot informatiom.
     *
     * The contents of this field depend on what is contained in the Slot Type
     * field. For Slot Type of `0xC4` this field must contain the numeric value
     * of the PCI Express Generation, such as Gen6 would be `0x06`. For other
     * PCI Express slot types, this field may be used but it is not required.
     * If not used, it should be set to `0x00`.
     *
     * For all other Slot Types, this field should be set to `0x00`.
     */
    uint8_t information;

    /**
     * @brief Slot physical width.
     *
     * This field indicates the physical width of the slot whereas Slot Data Bus
     * Width (offset 06h) indicates the electrical width of the slot.
     */
    dmi_slot_width_t physical_width;

    /**
     * @brief Slot pitch.
     *
     * This  field contains a numeric value that indicates the pitch of the slot
     * in 1/100 millimeter units. The pitch is defined by each slot/card
     * specification, but typically describes add-in card to add-in card pitch.
     * For example, if the pitch for the slot is 12.5 mm, the value 1250 would
     * be used.
     *
     * For EDSFF slots, the pitch is defined in SFF-TA-1006 table 7.1, SFF-TA-1007
     * table 7.1 (add-in card to add-in card pitch), and SFF-TA-1008 table 6-1
     * (SSD to SSD pitch).
     *
     * A value of 0 implies that the slot pitch is not given or is unknown.
     */
    uint16_t pitch;

    /**
     * @brief Slot height. This field indicates the maximum supported card height
     * for the slot.
     */
    dmi_slot_height_t height;
};

/**
 * @brief Slot peer group.
 */
struct dmi_slot_peer_group
{
    /**
     * @brief Address.
     */
    dmi_pci_addr_t address;

    /**
     * @brief Data bus width.
     */
    dmi_slot_width_t bus_width;
};

/**
 * @brief System slots entity specification.
 */
extern const dmi_entity_spec_t dmi_slot_spec;

__BEGIN_DECLS

const char *dmi_slot_type_name(dmi_slot_type_t value);
const char *dmi_slot_width_name(dmi_slot_width_t value);
const char *dmi_slot_usage_name(dmi_slot_usage_t value);
const char *dmi_slot_length_name(dmi_slot_length_t value);
const char *dmi_slot_height_name(dmi_slot_height_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_SLOT_H
