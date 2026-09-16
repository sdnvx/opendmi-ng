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

typedef struct dmi_memory_device             dmi_memory_device_t;
typedef struct dmi_memory_device_data        dmi_memory_device_data_t;
typedef union  dmi_memory_device_type_detail dmi_memory_device_type_detail_t;

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
    DMI_MEMORY_DEVICE_TYPE_MRDIMM      = 0x25, ///< MDRIMM
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
        bool __reserved    : 1; ///< Reserved
        bool other         : 1; ///< Other
        bool unknown       : 1; ///< Unknown
        bool fast_paged    : 1; ///< Fast-paged
        bool static_column : 1; ///< Static column
        bool pseudo_static : 1; ///< Pseudo-static
        bool rambus        : 1; ///< RAMBUS
        bool synchronous   : 1; ///< Synchronous
        bool cmos          : 1; ///< CMOS
        bool edo           : 1; ///< EDO
        bool window_dram   : 1; ///< Window DRAM
        bool cache_dram    : 1; ///< Cache DRAM
        bool non_volatile  : 1; ///< Non-volatile
        bool registered    : 1; ///< Registered (buffered)
        bool unbuffered    : 1; ///< Unbuffered (unregistered)
        bool lrdimm        : 1; ///< LRDIMM
    };
};

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
dmi_packed_struct(dmi_memory_device_data)
{
    /**
     * @brief SMBIOS structure header.
     */
    dmi_header_t header;

    /**
     * @brief Handle, or instance number, associated with the physical
     * memory array to which this device belongs.
     *
     * @since SMBIOS 2.1
     */
    dmi_handle_t array_handle;

    /**
     * @brief Handle, or instance number, associated with any error that was
     * previously detected for the device. If the system does not provide the
     * error information structure, the field contains 0xFFFE. Otherwise, the
     * field contains either 0xFFFF (if no error was detected) or the handle
     * of the error-information structure.
     *
     * @since SMBIOS 2.1
     */
    dmi_handle_t error_info_handle;

    /**
     * @brief Total width, in bits, of this memory device, including any check
     * or error-correction bits. If there are no error-correction bits, this
     * value should be equal to data width. If the width is unknown, the field
     * is set to 0xFFFF.
     *
     * @since SMBIOS 2.1
     */
    dmi_word_t total_width;

    /**
     * @brief Data width, in bits, of this memory device. A data width of 0 and
     * a total width of 8 indicates that the device is being used solely to
     * provide 8 error-correction bits. If the width is unknown, the field is
     * set to 0xFFFF.
     *
     * @since SMBIOS 2.1
     */
    dmi_word_t data_width;

    /**
     * @brief Size of the memory device.
     *
     * If the value is 0, no memory device is installed in the socket. If the
     * size is unknown, the field value is 0xFFFF. If the size is 32 GiB-1 MiB
     * or greater, the field value is 0x7FFF and the actual size is stored in
     * the extended size field.
     *
     * The granularity in which the value is specified depends on the setting
     * of the most-significant bit (bit 15). If the bit is 0, the value is
     * specified in megabyte units; if the bit is 1, the value is specified in
     * kilobyte units. For example, the value 0x8100 identifies a 256 KiB
     * memory device and 0x0100 identifies a 256 MiB memory device.
     *
     * @since SMBIOS 2.1
     */
    dmi_word_t size;

    /**
     * @brief Implementation form factor for this memory device.
     *
     * @since SMBIOS 2.1
     */
    dmi_byte_t form_factor;

    /**
     * @brief Identifies when the memory device is one of a set of memory
     * devices that must be populated with all devices of the same type and
     * size, and the set to which this device belongs.
     *
     * A value of 0 indicates that the device is not part of a set. A value of
     * 0xFF indicates that the attribute is unknown.
     *
     * @since SMBIOS 2.1
     */
    dmi_byte_t device_set;

    /**
     * @brief String number of the string that identifies the physically
     * labeled socket or board position where the memory device is located.
     * Example: "SIMM 3".
     *
     * @since SMBIOS 2.1
     */
    dmi_string_t device_locator;

    /**
     * @brief String number of the string that identifies the physically
     * labeled bank where the memory device is located. Example: "Bank 0" or
     * "A".
     *
     * @since SMBIOS 2.1
     */
    dmi_string_t bank_locator;

    /**
     * @brief Type of memory used in this device.
     *
     * @since SMBIOS 2.1
     */
    dmi_byte_t memory_type;

    /**
     * @brief Additional detail on the memory device type.
     *
     * @since SMBIOS 2.1
     */
    dmi_word_t memory_type_detail;

    /**
     * @brief Identifies the maximum capable speed of the device, in
     * megatransfers per second (MT/s). A value of 0x0000 indicated that the
     * speed is unknown. A value of 0xFFFF indicates that the speed is
     * 65,535 MT/s or greater, and the actual speed is stored in the extended
     * speed field.
     *
     * @since SMBIOS 2.3
     */
    dmi_word_t maximum_speed;

    /**
     * @brief String number for the manufacturer of this memory device.
     *
     * @since SMBIOS 2.3
     */
    dmi_string_t vendor;

    /**
     * @brief String number for the serial number of this memory device. This
     * value is set by the manufacturer and normally is not changeable.
     *
     * @since SMBIOS 2.3
     */
    dmi_string_t serial_number;

    /**
     * @brief String number for the asset tag of this memory device.
     *
     * @since SMBIOS 2.3
     */
    dmi_string_t asset_tag;

    /**
     * @brief String number for the part number of this memory device. This
     * value is set by the manufacturer and normally is not changeable.
     *
     * @since SMBIOS 2.3
     */
    dmi_string_t part_number;

    dmi_packed_struct()
    {
        /**
         * @brief Rank (bits 3-0, bits 7-4 are reserved). Zero means unknown
         * rank information.
         * @since SMBIOS 2.6
         */
        uint8_t rank : 4;
    };

    /**
     * @brief Extended size of the memory device (complements the size field
     * at offset 0x0C).
     *
     * @since SMBIOS 2.7
     */
    dmi_dword_t size_ex;

    /**
     * @brief Identifies the configured speed of the memory device, in
     * megatransfers per second (MT/s). The value 0x0000h means that the speed
     * is unknown. The value 0xFFFF means that the speed is 65,535 MT/s or
     * greater, and the actual speed is stored in the extended configured
     * memory speed field.
     *
     * @since SMBIOS 2.7
     */
    dmi_word_t configured_speed;

    /**
     * @brief Minimum operating voltage for this device, in millivolts. If the
     * value is 0, the voltage is unknown.
     *
     * @since SMBIOS 2.8
     */
    dmi_word_t minimum_voltage;

    /**
     * @brief Maximum operating voltage for this device, in millivolts. If the
     * value is 0, the voltage is unknown.
     *
     * @since SMBIOS 2.8
     */
    dmi_word_t maximum_voltage;

    /**
     * @brief Configured voltage for this device, in millivolts. If the value
     * is 0, the voltage is unknown.
     *
     * @since SMBIOS 2.8
     */
    dmi_word_t configured_voltage;

    /**
     * @brief Memory technology type for this memory device.
     * @since SMBIOS 3.2
     */
    dmi_byte_t memory_tech;

    /**
     * @brief The operating modes supported by this memory device.
     * @since SMBIOS 3.2
     */
    dmi_word_t memory_mode_caps;

    /**
     * @brief String number for the firmware version of this memory device.
     * @since SMBIOS 3.2
     */
    dmi_string_t firmware_version;

    /**
     * @brief The two-byte module manufacturer ID found in the SPD of this
     * memory device; LSB first.
     *
     * @since SMBIOS 3.2
     */
    dmi_word_t module_vendor_id;

    /**
     * @brief The two-byte module product ID found in the SPD of this memory
     * device; LSB first.
     *
     * @since SMBIOS 3.2
     */
    dmi_word_t module_product_id;

    /**
     * @brief The two-byte memory subsystem controller manufacturer ID found
     * in the SPD of this memory device; LSB first.
     *
     * @since SMBIOS 3.2
     */
    dmi_word_t controller_vendor_id;

    /**
     * @brief The two-byte memory subsystem controller product ID found in the
     * SPD of this memory device; LSB first.
     *
     * @since SMBIOS 3.2
     */
    dmi_word_t controller_product_id;

    /**
     * @brief Size of the non-volatile portion of the memory device in bytes,
     * if any. If the value is 0, there is no non-volatile portion. If the
     * non-volatile Size is unknown, the field is set to 0xFFFFFFFFFFFFFFFF.
     *
     * @since SMBIOS 3.2
     */
    dmi_qword_t non_volatile_size;

    /**
     * @brief Size of the volatile portion of the memory device in bytes, if
     * any. If the value is 0, there is no volatile portion. If the volatile
     * size is unknown, the field is set to 0xFFFFFFFFFFFFFFFF.
     *
     * @since SMBIOS 3.2
     */
    dmi_qword_t volatile_size;

    /**
     * @brief Size of the cache portion of the memory device in bytes, if any.
     * If the value is 0, there is no cache portion. If the cache size is
     * unknown, the field is set to 0xFFFFFFFFFFFFFFFF.
     *
     * @since SMBIOS 3.2
     */
    dmi_qword_t cache_size;

    /**
     * @brief Size of the logical memory device in bytes. If the size is
     * unknown, the field is set to 0xFFFFFFFFFFFFFFFF.
     *
     * @since SMBIOS 3.2
     */
    dmi_qword_t logical_size;

    /**
     * @brief Extended speed of the memory device (complements the speed field
     * at offset 0x15). Identifies the maximum capable speed of the device, in
     * megatransfers per second (MT/s).
     *
     * @since SMBIOS 3.3
     */
    dmi_dword_t maximum_speed_ex;

    /**
     * @brief Extended configured memory speed of the memory device (complements
     * the configured memory speed field at offset 0x20h). Identifies the configured
     * speed of the memory device, in megatransfers per second (MT/s).
     *
     * @since SMBIOS 3.3
     */
    dmi_dword_t configured_speed_ex;

    /**
     * @brief The two-byte PMIC0 manufacturer ID found in the SPD of this
     * memory device, LSB first.
     *
     * @since SMBIOS 3.7
     */
    dmi_word_t pmic0_vendor_id;

    /**
     * @brief The PMIC 0 Revision Number found in the SPD of this memory
     * device.
     *
     * @since SMBIOS 3.7
     */
    dmi_word_t pmic0_revision;

    /**
     * @brief The two-byte RCD manufacturer ID found in the SPD of this memory
     * device, LSB first.
     *
     * @since SMBIOS 3.7
     */
    dmi_word_t rcd_vendor_id;

    /**
     * @brief The RCD Revision Number found in the SPD of this memory device.
     *
     * @since SMBIOS 3.7
     */
    dmi_word_t rcd_revision;
};

struct dmi_memory_device
{
    /**
     * @brief Handle, or instance number, associated with the physical
     * memory array to which this device belongs.
     */
    dmi_handle_t array_handle;
    dmi_entity_t *array;
    dmi_handle_t error_info_handle;
    dmi_entity_t *error_info;
    unsigned short total_width;
    unsigned short data_width;
    dmi_size_t size;
    dmi_memory_device_form_factor_t form_factor;
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
    dmi_memory_device_type_detail_t memory_type_detail;
    unsigned long maximum_speed;
    const char *vendor;

    /**
     * @brief Serial number of this memory device. This value is set by the
     * manufacturer and normally is not changeable.
     */
    const char *serial_number;

    /**
     * @brief Asset tag of this memory device.
     */
    const char *asset_tag;

    /**
     * @brief Part number of this memory device. This value is set by the
     * manufacturer and normally is not changeable.
     */
    const char *part_number;

    unsigned short rank;
    unsigned long configured_speed;
    unsigned short minimum_voltage;
    unsigned short maximum_voltage;
    unsigned short configured_voltage;
    dmi_memory_device_tech_t memory_tech;
    uint16_t memory_mode_caps;
    const char *firmware_version;
    uint16_t module_vendor_id;
    uint16_t module_product_id;
    uint16_t controller_vendor_id;
    uint16_t controller_product_id;

    /**
     * @brief Size of the non-volatile portion of the memory device in bytes,
     * if any. If the value is 0, there is no non-volatile portion. If the
     * non-volatile Size is unknown, the field is set to `UINT64_MAX`.
     */
    dmi_size_t non_volatile_size;

    /**
     * @brief Size of the volatile portion of the memory device in bytes, if
     * any. If the value is 0, there is no volatile portion. If the volatile
     * size is unknown, the field is set to `UINT64_MAX`.
     */
    dmi_size_t volatile_size;

    /**
     * @brief Size of the cache portion of the memory device in bytes, if any.
     * If the value is 0, there is no cache portion. If the cache size is
     * unknown, the field is set to `UINT64_MAX`.
     */
    dmi_size_t cache_size;

    /**
     * @brief Size of the logical memory device in bytes. If the size is
     * unknown, the field is set to `UINT64_MAX`.
     */
    dmi_size_t logical_size;

    /**
     * @brief The two-byte PMIC0 manufacturer ID found in the SPD of this
     * memory device, LSB first.
     */
    uint16_t pmic0_vendor_id;

    /**
     * @brief The PMIC 0 Revision Number found in the SPD of this memory
     * device.
     */
    uint16_t pmic0_revision;

    /**
     * @brief The two-byte RCD manufacturer ID found in the SPD of this memory
     * device.
     */
    uint16_t rcd_vendor_id;

    /**
     * @brief The RCD Revision Number found in the SPD of this memory device.
     */
    uint16_t rcd_revision;

    /**
     * @brief Reference to the structure that identifies the memory channel
     * to which this device is connected.
     */
    dmi_entity_t *channel;
};

/**
 * @brief Memory device entity specification.
 */
extern const dmi_entity_spec_t dmi_memory_device_spec;

__BEGIN_DECLS

const char *dmi_memory_device_type_name(dmi_memory_device_type_t value);
const char *dmi_memory_device_form_factor_name(dmi_memory_device_form_factor_t value);
const char *dmi_memory_device_tech_name(dmi_memory_device_tech_t value);

/**
 * @internal
 */
dmi_size_t dmi_memory_device_size(uint16_t value);

/**
 * @internal
 */
dmi_size_t dmi_memory_device_size_ex(uint32_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_MEMORY_DEVICE_H
