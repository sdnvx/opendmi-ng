//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_DEVICE_H
#define OPENDMI_ENTITY_MEMORY_DEVICE_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_MEMORY_DEVICE_T
#   define DMI_MEMORY_DEVICE_T
    typedef struct dmi_memory_device dmi_memory_device_t;
#endif // !DMI_MEMORY_DEVICE_T

#ifndef DMI_MEMORY_DEVICE_TYPE_DETAIL_T
#   define DMI_MEMORY_DEVICE_TYPE_DETAIL_T
    typedef union dmi_memory_device_type_detail dmi_memory_device_type_detail_t;
#endif // !DMI_MEMORY_DEVICE_TYPE_DETAIL_T

/**
 * @brief Memory device types.
 */
typedef enum dmi_memory_device_type
{
    DMI_MEMORY_DEVICE_TYPE_UNSPEC      = 0x00, ///< Unspecified
    DMI_MEMORY_DEVICE_TYPE_OTHER       = 0x01, ///< Other
    DMI_MEMORY_DEVICE_TYPE_UNKNOWN     = 0x02, ///< Unknown
    DMI_MEMORY_DEVICE_TYPE_DRAM        = 0x03, ///< DRAM
    DMI_MEMORY_DEVICE_TYPE_EDRAM       = 0x04, ///< EDRAM
    DMI_MEMORY_DEVICE_TYPE_VRAM        = 0x05, ///< VRAM
    DMI_MEMORY_DEVICE_TYPE_SRAM        = 0x06, ///< SRAM
    DMI_MEMORY_DEVICE_TYPE_RAM         = 0x07, ///< RAM
    DMI_MEMORY_DEVICE_TYPE_ROM         = 0x08, ///< ROM
    DMI_MEMORY_DEVICE_TYPE_FLASH       = 0x09, ///< Flash
    DMI_MEMORY_DEVICE_TYPE_EEPROM      = 0x0A, ///< EEPROM
    DMI_MEMORY_DEVICE_TYPE_FEPROM      = 0x0B, ///< FEPROM
    DMI_MEMORY_DEVICE_TYPE_EPROM       = 0x0C, ///< EPROM
    DMI_MEMORY_DEVICE_TYPE_CDRAM       = 0x0D, ///< CDRAM
    DMI_MEMORY_DEVICE_TYPE_3DRAM       = 0x0E, ///< 3DRAM
    DMI_MEMORY_DEVICE_TYPE_SDRAM       = 0x0F, ///< SDRAM
    DMI_MEMORY_DEVICE_TYPE_SGRAM       = 0x10, ///< SGRAM
    DMI_MEMORY_DEVICE_TYPE_RDRAM       = 0x11, ///< RDRAM
    DMI_MEMORY_DEVICE_TYPE_DDR         = 0x12, ///< DDR
    DMI_MEMORY_DEVICE_TYPE_DDR2        = 0x13, ///< DDR2
    DMI_MEMORY_DEVICE_TYPE_DDR2_FBDIMM = 0x14, ///< DDR2 FB-DIMM
    __DMI_MEMORY_DEVICE_TYPE_UNASSIGNED_START = 0x15,
    // Unassigned: 0x15 .. 0x17
    __DMI_MEMORY_DEVICE_TYPE_UNASSIGNED_END   = 0x17,
    DMI_MEMORY_DEVICE_TYPE_DDR3        = 0x18, ///< DDR3
    DMI_MEMORY_DEVICE_TYPE_FBD2        = 0x19, ///< FBD2
    DMI_MEMORY_DEVICE_TYPE_DDR4        = 0x1A, ///< DDR4
    DMI_MEMORY_DEVICE_TYPE_LPDDR       = 0x1B, ///< LPDDR
    DMI_MEMORY_DEVICE_TYPE_LPDDR2      = 0x1C, ///< LPDDR2
    DMI_MEMORY_DEVICE_TYPE_LPDDR3      = 0x1D, ///< LPDDR3
    DMI_MEMORY_DEVICE_TYPE_LPDDR4      = 0x1E, ///< LPDDR4
    DMI_MEMORY_DEVICE_TYPE_LOGICAL_NV  = 0x1F, ///< Logical non-volatile device
    DMI_MEMORY_DEVICE_TYPE_HBM         = 0x20, ///< HBM (High Bandwidth Memory)
    DMI_MEMORY_DEVICE_TYPE_HBM2        = 0x21, ///< HBM2 (High Bandwidth Memory Generation 2)
    DMI_MEMORY_DEVICE_TYPE_DDR5        = 0x22, ///< DDR5
    DMI_MEMORY_DEVICE_TYPE_LPDDR5      = 0x23, ///< LPDDR5
    DMI_MEMORY_DEVICE_TYPE_HBM3        = 0x24, ///< HBM3 (High Bandwidth Memory Generation 3)
    DMI_MEMORY_DEVICE_TYPE_MRDIMM      = 0x25, ///< MRDIMM
    DMI_MEMORY_DEVICE_TYPE_LPDDR6      = 0x26, ///< LPDDR6
    __DMI_MEMORY_DEVICE_TYPE_COUNT
} dmi_memory_device_type_t;

/**
 * @brief Memory device form factors.
 */
typedef enum dmi_memory_device_form_factor
{
    DMI_MEMORY_DEVICE_FORM_FACTOR_UNSPEC   = 0x00, ///< Unspecified
    DMI_MEMORY_DEVICE_FORM_FACTOR_OTHER    = 0x01, ///< Other
    DMI_MEMORY_DEVICE_FORM_FACTOR_UNKNOWN  = 0x02, ///< Unknown
    DMI_MEMORY_DEVICE_FORM_FACTOR_SIMM     = 0x03, ///< SIMM
    DMI_MEMORY_DEVICE_FORM_FACTOR_SIP      = 0x04, ///< SIP
    DMI_MEMORY_DEVICE_FORM_FACTOR_CHIP     = 0x05, ///< Chip
    DMI_MEMORY_DEVICE_FORM_FACTOR_DIP      = 0x06, ///< DIP
    DMI_MEMORY_DEVICE_FORM_FACTOR_ZIP      = 0x07, ///< ZIP
    DMI_MEMORY_DEVICE_FORM_FACTOR_CARD     = 0x08, ///< Proprietary card
    DMI_MEMORY_DEVICE_FORM_FACTOR_DIMM     = 0x09, ///< DIMM
    DMI_MEMORY_DEVICE_FORM_FACTOR_TSOP     = 0x0A, ///< TSOP
    DMI_MEMORY_DEVICE_FORM_FACTOR_CHIP_ROW = 0x0B, ///< Row of chips
    DMI_MEMORY_DEVICE_FORM_FACTOR_RIMM     = 0x0C, ///< RIMM
    DMI_MEMORY_DEVICE_FORM_FACTOR_SODIMM   = 0x0D, ///< SODIMM
    DMI_MEMORY_DEVICE_FORM_FACTOR_SRIMM    = 0x0E, ///< SRIMM
    DMI_MEMORY_DEVICE_FORM_FACTOR_FBDIMM   = 0x0F, ///< FBDIMM
    DMI_MEMORY_DEVICE_FORM_FACTOR_DIE      = 0x10, ///< Die
    DMI_MEMORY_DEVICE_FORM_FACTOR_CAMM     = 0x11, ///< CAMM
    DMI_MEMORY_DEVICE_FORM_FACTOR_CUDIMM   = 0x12, ///< CUDIMM
    DMI_MEMORY_DEVICE_FORM_FACTOR_CSODIMM  = 0x13, ///< CSODIMM
    __DMI_MEMORY_DEVICE_FORM_FACTOR_COUNT
} dmi_memory_device_form_factor_t;

/**
 * @brief Memory device technologies.
 */
typedef enum dmi_memory_device_tech
{
    DMI_MEMORY_DEVICE_TECH_UNSPEC   = 0x00, ///< Unspecified
    DMI_MEMORY_DEVICE_TECH_OTHER    = 0x01, ///< Other
    DMI_MEMORY_DEVICE_TECH_UNKNOWN  = 0x02, ///< Unknown
    DMI_MEMORY_DEVICE_TECH_DRAM     = 0x03, ///< DRAM
    DMI_MEMORY_DEVICE_TECH_NVDIMM_N = 0x04, ///< NVDIMM-N
    DMI_MEMORY_DEVICE_TECH_NVDIMM_F = 0x05, ///< NVDIMM-F
    DMI_MEMORY_DEVICE_TECH_NVDIMM_P = 0x06, ///< NVDIMM-P
    DMI_MEMORY_DEVICE_TECH_OPTANE   = 0x07, ///< Intel Optane persistent memory
    DMI_MEMORY_DEVICE_TECH_MRDIMM   = 0x08, ///< MRDIMM (deprecated)
    __DMI_MEMORY_DEVICE_TECH_COUNT
} dmi_memory_device_tech_t;

/**
 * @brief Memory device type details.
 */
dmi_packed_union(dmi_memory_device_type_detail)
{
    dmi_word_t __value;

    dmi_packed_struct()
    {
        dmi_word_t __reserved    : 1; ///< Reserved
        dmi_word_t other         : 1; ///< Other
        dmi_word_t unknown       : 1; ///< Unknown
        dmi_word_t fast_paged    : 1; ///< Fast-paged
        dmi_word_t static_column : 1; ///< Static column
        dmi_word_t pseudo_static : 1; ///< Pseudo-static
        dmi_word_t rambus        : 1; ///< RAMBUS
        dmi_word_t synchronous   : 1; ///< Synchronous
        dmi_word_t cmos          : 1; ///< CMOS
        dmi_word_t edo           : 1; ///< EDO
        dmi_word_t window_dram   : 1; ///< Window DRAM
        dmi_word_t cache_dram    : 1; ///< Cache DRAM
        dmi_word_t non_volatile  : 1; ///< Non-volatile
        dmi_word_t registered    : 1; ///< Registered (buffered)
        dmi_word_t unbuffered    : 1; ///< Unbuffered (unregistered)
        dmi_word_t lrdimm        : 1; ///< LRDIMM
    };
};

dmi_static_assert_value_union(dmi_memory_device_type_detail);

/**
 * @brief Memory device structure (type 17).
 *
 * This structure describes a single memory device that is part of a larger
 * physical memory array (type 16) structure. If a system includes memory-device
 * sockets, the SMBIOS implementation includes a memory device structure
 * instance for each slot, whether the socket is currently populated.
 *
 * @since SMBIOS 2.1
 */
struct dmi_memory_device
{
    /**
     * @brief Handle, or instance number, associated with the physical
     * memory array to which this device belongs.
     */
    dmi_handle_t array_handle;

    /**
     * @brief Physical memory array entity, linked by handle, or `nullptr`.
     */
    dmi_entity_t *array;

    /**
     * @brief Handle of the memory error information structure (type 18 or
     * 33) describing the last error detected in the device. 0xFFFE if the
     * firmware provides no error information, `DMI_HANDLE_INVALID` if no
     * error has been detected.
     */
    dmi_handle_t error_info_handle;

    /**
     * @brief Memory error information entity, linked by handle, or
     * `nullptr`.
     */
    dmi_entity_t *error_info;

    /**
     * @brief Total width of the device, in bits, error-correction bits
     * included. `USHRT_MAX` if unknown.
     */
    unsigned short total_width;

    /**
     * @brief Data width of the device, in bits. `USHRT_MAX` if unknown.
     */
    unsigned short data_width;

    /**
     * @brief Size of the device, in bytes. 0 if the socket is empty,
     * `DMI_SIZE_MAX` if unknown. Sizes of 32 GiB - 1 MiB and above come from
     * the extended size field added in SMBIOS 2.7.
     */
    dmi_size_t size;

    /**
     * @brief Form factor of the device.
     */
    dmi_memory_device_form_factor_t form_factor;

    /**
     * @brief Number of the set of devices which must be populated with
     * devices of the same type and size, 0 if the device belongs to no set,
     * `USHRT_MAX` if unknown.
     */
    unsigned short device_set;

    /**
     * @brief The string that identifies the physically labeled socket or board
     * position where the memory device is located. Example: "SIMM 3".
     */
    const char *device_locator;

    /**
     * @brief The string that identifies the physically labeled bank where the
     * memory device is located. Example: "Bank 0" or "A".
     */
    const char *bank_locator;

    /**
     * @brief Type of memory used in this device.
     */
    dmi_memory_device_type_t memory_type;

    /**
     * @brief Additional details on the memory type.
     */
    dmi_memory_device_type_detail_t memory_type_detail;

    /**
     * @brief Maximum speed the device is capable of, in megatransfers per
     * second (MT/s), 0 if unknown. Speeds of 65535 MT/s and above come from
     * the extended speed field added in SMBIOS 3.3.
     *
     * @since SMBIOS 2.3
     */
    unsigned long maximum_speed;

    /**
     * @brief Manufacturer of the device.
     *
     * @since SMBIOS 2.3
     */
    const char *vendor;

    /**
     * @brief Serial number of this memory device. This value is set by the
     * manufacturer and normally is not changeable.
     *
     * @since SMBIOS 2.3
     */
    const char *serial_number;

    /**
     * @brief Asset tag of this memory device.
     *
     * @since SMBIOS 2.3
     */
    const char *asset_tag;

    /**
     * @brief Part number of this memory device. This value is set by the
     * manufacturer and normally is not changeable.
     *
     * @since SMBIOS 2.3
     */
    const char *part_number;

    /**
     * @brief Physical or package rank of the device, 0 if unknown. Ranks
     * above 15 are defined since SMBIOS 3.10, which widens the field to five
     * bits.
     *
     * @since SMBIOS 2.6
     */
    unsigned short rank;

    /**
     * @brief Whether the device has been disabled because of an error. The
     * error may be described by a 32-bit memory error information structure.
     *
     * @since SMBIOS 3.10
     */
    bool is_disabled;

    /**
     * @brief Whether the device is functioning, but has been left out of the
     * memory map to prevent a non-standard configuration. The device may be
     * described by a 32-bit memory error information structure reporting the
     * OK kind of error.
     *
     * @since SMBIOS 3.10
     */
    bool is_unmapped;

    /**
     * @brief Speed the device is configured to run at, in megatransfers per
     * second (MT/s), 0 if unknown. Speeds of 65535 MT/s and above come from
     * the extended configured speed field added in SMBIOS 3.3.
     *
     * @since SMBIOS 2.7
     */
    unsigned long configured_speed;

    /**
     * @brief Minimum operating voltage of the device, in millivolts, 0 if
     * unknown.
     *
     * @since SMBIOS 2.8
     */
    unsigned short minimum_voltage;

    /**
     * @brief Maximum operating voltage of the device, in millivolts, 0 if
     * unknown.
     *
     * @since SMBIOS 2.8
     */
    unsigned short maximum_voltage;

    /**
     * @brief Voltage the device is configured to run at, in millivolts, 0 if
     * unknown.
     *
     * @since SMBIOS 2.8
     */
    unsigned short configured_voltage;

    /**
     * @brief Memory technology of the device.
     *
     * @since SMBIOS 3.2
     */
    dmi_memory_device_tech_t memory_tech;

    /**
     * @brief Operating modes the device supports, as stored: bit 1 other,
     * bit 2 unknown, bit 3 volatile memory, bit 4 byte-accessible persistent
     * memory, bit 5 block-accessible persistent memory.
     *
     * @since SMBIOS 3.2
     */
    uint16_t memory_mode_caps;

    /**
     * @brief Firmware version of the device.
     *
     * @since SMBIOS 3.2
     */
    const char *firmware_version;

    /**
     * @brief JEDEC JEP106 manufacturer ID of the module, as read from its
     * SPD, 0 if unknown.
     *
     * @since SMBIOS 3.2
     */
    uint16_t module_vendor_id;

    /**
     * @brief Product ID of the module, as read from its SPD, 0 if unknown.
     *
     * @since SMBIOS 3.2
     */
    uint16_t module_product_id;

    /**
     * @brief JEDEC JEP106 manufacturer ID of the memory subsystem controller,
     * as read from the SPD, 0 if unknown.
     *
     * @since SMBIOS 3.2
     */
    uint16_t controller_vendor_id;

    /**
     * @brief Product ID of the memory subsystem controller, as read from the
     * SPD, 0 if unknown.
     *
     * @since SMBIOS 3.2
     */
    uint16_t controller_product_id;

    /**
     * @brief Size of the non-volatile portion of the memory device in bytes,
     * if any. If the value is 0, there is no non-volatile portion. If the
     * non-volatile Size is unknown, the field is set to `DMI_SIZE_MAX`.
     *
     * @since SMBIOS 3.2
     */
    dmi_size_t non_volatile_size;

    /**
     * @brief Size of the volatile portion of the memory device in bytes, if
     * any. If the value is 0, there is no volatile portion. If the volatile
     * size is unknown, the field is set to `DMI_SIZE_MAX`.
     *
     * @since SMBIOS 3.2
     */
    dmi_size_t volatile_size;

    /**
     * @brief Size of the cache portion of the memory device in bytes, if any.
     * If the value is 0, there is no cache portion. If the cache size is
     * unknown, the field is set to `DMI_SIZE_MAX`.
     *
     * @since SMBIOS 3.2
     */
    dmi_size_t cache_size;

    /**
     * @brief Size of the logical memory device in bytes. If the size is
     * unknown, the field is set to `DMI_SIZE_MAX`.
     *
     * @since SMBIOS 3.2
     */
    dmi_size_t logical_size;

    /**
     * @brief The two-byte PMIC0 manufacturer ID found in the SPD of this
     * memory device, LSB first. 0 if unknown.
     *
     * @since SMBIOS 3.7
     */
    uint16_t pmic0_vendor_id;

    /**
     * @brief The PMIC 0 Revision Number found in the SPD of this memory
     * device. 0xFF00 if unknown.
     *
     * @since SMBIOS 3.7
     */
    uint16_t pmic0_revision;

    /**
     * @brief The two-byte RCD manufacturer ID found in the SPD of this memory
     * device, LSB first. 0 if unknown.
     *
     * @since SMBIOS 3.7
     */
    uint16_t rcd_vendor_id;

    /**
     * @brief The RCD Revision Number found in the SPD of this memory device.
     * 0xFF00 if unknown.
     *
     * @since SMBIOS 3.7
     */
    uint16_t rcd_revision;

    /**
     * @brief Reference to the structure that identifies the memory channel
     * to which this device is connected, set when the memory channel
     * structures (type 37) are linked, `nullptr` if no channel lists the
     * device.
     */
    dmi_entity_t *channel;
};

/**
 * @brief Memory device entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_memory_device_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_memory_device_type_name(dmi_memory_device_type_t value);
__dmi_api const char *dmi_memory_device_form_factor_name(dmi_memory_device_form_factor_t value);
__dmi_api const char *dmi_memory_device_tech_name(dmi_memory_device_tech_t value);

/**
 * @internal
 */
__dmi_api dmi_size_t dmi_memory_device_size(uint16_t value);

/**
 * @internal
 */
__dmi_api dmi_size_t dmi_memory_device_size_ex(uint32_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_MEMORY_DEVICE_H
