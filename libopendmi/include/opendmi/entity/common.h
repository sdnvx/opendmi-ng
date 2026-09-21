//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_COMMON_H
#define OPENDMI_ENTITY_COMMON_H

#pragma once

#include <opendmi/attribute.h>
#include <opendmi/field.h>
#include <opendmi/stream.h>
#include <opendmi/utils/name.h>

typedef struct dmi_pci_addr dmi_pci_addr_t;

/**
 * @brief Status types.
 */
typedef enum dmi_status
{
    DMI_STATUS_UNSPEC          = 0x00, ///< Unspecified
    DMI_STATUS_OTHER           = 0x01, ///< Other
    DMI_STATUS_UNKNOWN         = 0x02, ///< Unknown
    DMI_STATUS_OK              = 0x03, ///< OK
    DMI_STATUS_NON_CRITICAL    = 0x04, ///< Non-critical
    DMI_STATUS_CRITICAL        = 0x05, ///< Critical
    DMI_STATUS_NON_RECOVERABLE = 0x06, ///< Non-recoverable
    __DMI_STATUS_COUNT
} dmi_status_t;

/**
 * @brief Error correction types.
 */
typedef enum dmi_error_correct_type
{
    DMI_ERROR_CORRECT_TYPE_UNSPEC      = 0x00, ///< Unspecified
    DMI_ERROR_CORRECT_TYPE_OTHER       = 0x01, ///< Other
    DMI_ERROR_CORRECT_TYPE_UNKNOWN     = 0x02, ///< Unknown
    DMI_ERROR_CORRECT_TYPE_NONE        = 0x03, ///< None
    DMI_ERROR_CORRECT_TYPE_PARITY      = 0x04, ///< Parity
    DMI_ERROR_CORRECT_TYPE_SINGLE_BIT  = 0x05, ///< Single-bit ECC
    DMI_ERROR_CORRECT_TYPE_MULTI_BIT   = 0x06, ///< Multi-bit ECC
    DMI_ERROR_CORRECT_TYPE_CRC         = 0x07, ///< CRC
    __DMI_ERROR_CORRECT_TYPE_COUNT
} dmi_error_correct_type_t;

/**
 * @brief PCI class identifier.
 */
typedef uint8_t dmi_pci_class_t;

/**
 * @brief PCI slot number.
 */
typedef uint16_t dmi_pci_slot_t;

/**
 * @brief PCI vendor identifier.
 */
typedef uint16_t dmi_pci_vendor_id_t;

/**
 * @brief PCI device identifier.
 */
typedef uint16_t dmi_pci_device_id_t;

/**
 * @brief PCI address.
 */
struct dmi_pci_addr
{
    /**
     * @brief Segment group. The value is 0 for a single-segment topology.
     */
    uint16_t segment_group;

    /**
     * @brief Bus number. Set to `UINT8_MAX` if not applicable.
     */
    uint8_t bus_number;

    /**
     * @brief Device number. Set to `UINT8_MAX` if not applicable.
     */
    uint8_t device_number;

    /**
     * @brief Function number. Set to `UINT8_MAX` if not applicable.
     */
    uint8_t function_number;
};

extern __dmi_api const dmi_name_set_t dmi_status_names;
extern __dmi_api const dmi_name_set_t dmi_error_correct_type_names;

extern __dmi_api const dmi_attribute_t dmi_pci_addr_attrs[];

__BEGIN_DECLS

__dmi_api const char *dmi_status_name(dmi_status_t value);
__dmi_api const char *dmi_error_correct_type_name(dmi_error_correct_type_t value);

/**
 * @internal
 * @brief Decode a PCI address, which a field carries as a double word of the
 * segment group, the bus, and the device and function packed into one byte.
 *
 * Addresses of no bus carry no device and function either.
 */
__dmi_api bool dmi_pci_addr_decode(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Encode a PCI address, which undoes `dmi_pci_addr_decode()`.
 */
__dmi_api bool dmi_pci_addr_encode(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

__END_DECLS

#endif // !OPENDMI_ENTITY_COMMON_H
