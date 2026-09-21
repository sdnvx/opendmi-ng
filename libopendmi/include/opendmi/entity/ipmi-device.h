//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_IPMI_DEVICE_H
#define OPENDMI_ENTITY_IPMI_DEVICE_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/lint.h>

typedef struct dmi_ipmi_device         dmi_ipmi_device_t;
typedef union  dmi_ipmi_device_details dmi_ipmi_device_details_t;

typedef enum dmi_ipmi_interface
{
    DMI_IPMI_INTERFACE_UNKNOWN = 0x00, ///< Unknown
    DMI_IPMI_INTERFACE_KCS     = 0x01, ///< KCS: Keyboard Controller Style
    DMI_IPMI_INTERFACE_SMIC    = 0x02, ///< SMIC: Server Management Interface Chip
    DMI_IPMI_INTERFACE_BT      = 0x03, ///< BT: Block Transfer
    DMI_IPMI_INTERFACE_SSIF    = 0x04, ///< SSIF: SMBus System Interface
} dmi_ipmi_interface_t;

typedef enum dmi_ipmi_addr_type
{
    DMI_IPMI_ADDR_TYPE_MEMORY = 0x0, ///< Memory-mapped
    DMI_IPMI_ADDR_TYPE_IO     = 0x1, ///< I/O space
    DMI_IPMI_ADDR_TYPE_SMBUS  = 0x2  ///< SMBus target address (SSIF interface)
} dmi_ipmi_addr_type_t;

typedef enum dmi_ipmi_intr_trigger
{
    DMI_IPMI_INTR_TRIGGER_UNSPEC = 0x0, ///< Unspecified
    DMI_IPMI_INTR_TRIGGER_EDGE   = 0x1, ///< Edge
    DMI_IPMI_INTR_TRIGGER_LEVEL  = 0x2  ///< Level
} dmi_ipmi_intr_trigger_t;

typedef enum dmi_ipmi_intr_polarity
{
    DMI_IPMI_INTR_POLARITY_UNSPEC = 0x0, ///< Unspecified
    DMI_IPMI_INTR_POLARITY_LOW    = 0x1, ///< Active low
    DMI_IPMI_INTR_POLARITY_HIGH   = 0x2  ///< Active high
} dmi_ipmi_intr_polarity_t;

typedef enum dmi_ipmi_register_spacing
{
    DMI_IPMI_REGISTER_SPACING_1        = 0x0, ///< 1 byte
    DMI_IPMI_REGISTER_SPACING_4        = 0x1, ///< 4 bytes
    DMI_IPMI_REGISTER_SPACING_16       = 0x2, ///< 16 bytes
    DMI_IPMI_REGISTER_SPACING_RESERVED = 0x3, ///< Reserved
} dmi_ipmi_register_spacing_t;

dmi_packed_union(dmi_ipmi_device_details)
{
    /**
     * @brief Raw value.
     */
    uint8_t __value;

    dmi_packed_struct()
    {
        uint8_t is_intr_level_triggered : 1;
        uint8_t is_intr_active_high     : 1;
        uint8_t __reserved_1            : 1; ///< Reserved for future use, set to 0.
        uint8_t is_intr_info_specified  : 1;
        uint8_t base_addr_lsb           : 1;
        uint8_t __reserved_2            : 1; ///< Reserved for future use, set to 0.
        uint8_t register_spacing        : 2;
    };
};

dmi_static_assert_value_union(dmi_ipmi_device_details);

struct dmi_ipmi_device
{
    /**
     * @brief Baseboard Management Controller (BMC) interface type.
     */
    dmi_ipmi_interface_t interface_type;

    /**
     * @brief IPMI specification revision.
     */
    dmi_version_t spec_version;

    /**
     * @brief Target address on the I2C bus of this BMC.
     */
    dmi_i2c_addr_t i2c_target_addr;

    /**
     * @brief Bus ID of the NV storage device. If no storage device exists for
     * this BMC, the field is set to `UINT8_MAX`.
     */
    uint8_t nv_storage_addr;

    /**
     * @brief Base address (either memory-mapped or I/O) of the BMC.
     *
     * In the raw structure, the least-significant bit of the field indicates
     * I/O space, and the actual least-significant bit of the address is stored
     * in the base address modifier. This field contains the actual address
     * with both bits applied, and the address space is reported in
     * @ref base_addr_type. For SSIF interface, the field contains SMBus target
     * address of the BMC.
     */
    dmi_size_t base_addr;

    /**
     * @brief Base address type.
     */
    dmi_ipmi_addr_type_t base_addr_type;

    /**
     * @brief Least-significant bit of the base address, as specified in the
     * base address modifier.
     */
    bool base_addr_lsb;

    /**
     * @brief Interrupt trigger mode.
     */
    dmi_ipmi_intr_trigger_t intr_trigger;

    /**
     * @brief Interrupt polarity.
     */
    dmi_ipmi_intr_polarity_t intr_polarity;

    /**
     * @brief Register spacing in bytes.
     */
    unsigned short register_spacing;

    /**
     * @brief Interrupt number for IPMI System Interface. Zero means that IPMI
     * interrupt is unspecified or unsupported.
     */
    unsigned short intr_number;
};

/**
 * @brief IPMI device information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_ipmi_device_spec;

/**
 * @brief Revision of the IPMI specification is a binary-coded decimal.
 */
extern __dmi_api const dmi_lint_rule_t dmi_ipmi_device_revision_rule;

__BEGIN_DECLS

__dmi_api const char *dmi_ipmi_interface_name(dmi_ipmi_interface_t value);
__dmi_api const char *dmi_ipmi_addr_type_name(dmi_ipmi_addr_type_t value);
__dmi_api const char *dmi_ipmi_intr_trigger_name(dmi_ipmi_intr_trigger_t value);
__dmi_api const char *dmi_ipmi_intr_polarity_name(dmi_ipmi_intr_polarity_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_IPMI_DEVICE_H
