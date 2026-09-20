//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_TYPES_H
#define OPENDMI_TYPES_H

#pragma once

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <limits.h>

#if defined(_WIN32)
#   include <BaseTsd.h>
    typedef SSIZE_T ssize_t;
#endif

#include <opendmi/defs.h>

#define DMI_HANDLE_INVALID     ((dmi_handle_t)0xFFFFu)
#define DMI_HANDLE_UNSUPPORTED ((dmi_handle_t)0xFFFEu)
#define DMI_HANDLE_TEST        ((dmi_handle_t)0x1234u)

/**
 * @brief Byte type for raw data and pointer arithmetics.
 */
typedef uint8_t dmi_data_t;
#define dmi_data(x) ((const dmi_data_t *)(x))

/**
 * @brief Binary data of variable length, referenced in place.
 */
typedef struct dmi_binary
{
    /**
     * @brief Pointer to the data, @c nullptr if the data is empty.
     */
    const dmi_data_t *data;

    /**
     * @brief Data length in bytes.
     */
    size_t length;
} dmi_binary_t;

/**
 * @brief SMBIOS BYTE type.
 */
typedef uint8_t dmi_byte_t;

/**
 * @brief SMBIOS WORD type.
 * @note Raw values are always little-endian.
 */
typedef uint16_t dmi_word_t;

/**
 * @brief SMBIOS DWORD type.
 * @note Raw values are always little-endian.
 */
typedef uint32_t dmi_dword_t;

/**
 * @brief SMBIOS QWORD value type.
 * @note Raw values are always little-endian.
 */
typedef uint64_t dmi_qword_t;

/**
 * @brief SMBIOS structure handle, a unique 16-bit number in the range 0 to
 * 0xFFFE (for version 2.0) or 0 to 0xFEFF (for version 2.1 and later). The
 * handle numbers are not required to be contiguous. For version 2.1 and
 * later, handle values in the range 0xFF00 to 0xFFFF are reserved for use
 * by DMI/SMBIOS specification.
 *
 * The UEFI Platform Initialization Specification reserves handle number
 * 0xFFFE for its EFI_SMBIOS_PROTOCOL.Add() function to mean "assign an unused
 * handle number automatically." This number is not used for any other purpose
 * by the SMBIOS specification.
 */
typedef dmi_word_t dmi_handle_t;

/**
 * @brief DMI string number.
 */
typedef dmi_byte_t dmi_string_t;

/**
 * @brief DMI size type.
 */
typedef uint64_t dmi_size_t;

/**
 * @brief Maximum value of @ref dmi_size_t, used to represent unknown sizes.
 */
#define DMI_SIZE_MAX ((dmi_size_t)UINT64_MAX)

/**
 * @brief I2C address (7- or 10-bit).
 */
typedef uint16_t dmi_i2c_addr_t;

/**
 * @brief SMBIOS structure types identifiers. Types 0 through 127 (7Fh) are
 * reserved for and defined by this specification. Types 128 through 256 (0x80
 * to 0xFF) are available for system- and OEM-specific information.
 */
typedef enum dmi_type
{
    DMI_TYPE_INVALID                 = -1,
    DMI_TYPE_ANY                     = -1,
    DMI_TYPE_FIRMWARE                = 0,   ///< Platform firmware information
    DMI_TYPE_SYSTEM                  = 1,   ///< System information
    DMI_TYPE_BASEBOARD               = 2,   ///< Baseboard or module information
    DMI_TYPE_CHASSIS                 = 3,   ///< System enclosure or chassis
    DMI_TYPE_PROCESSOR               = 4,   ///< Processor information
    DMI_TYPE_MEMORY_CONTROLLER       = 5,   ///< Memory controller information (obsolette)
    DMI_TYPE_MEMORY_MODULE           = 6,   ///< Memory module information (obsolette)
    DMI_TYPE_CACHE                   = 7,   ///< Cache information
    DMI_TYPE_PORT_CONNECTOR          = 8,   ///< Port connector information
    DMI_TYPE_SYSTEM_SLOTS            = 9,   ///< System slots
    DMI_TYPE_ONBOARD_DEVICE          = 10,  ///< Onboard devices information
    DMI_TYPE_OEM_STRINGS             = 11,  ///< OEM strings
    DMI_TYPE_SYSTEM_CONFIG_OPTIONS   = 12,  ///< System configuration options
    DMI_TYPE_FIRMWARE_LANGUAGE       = 13,  ///< Firmware language information
    DMI_TYPE_GROUP_ASSOC             = 14,  ///< Group associations
    DMI_TYPE_SYSTEM_EVENT_LOG        = 15,  ///< System event log
    DMI_TYPE_MEMORY_ARRAY            = 16,  ///< Physical memory array
    DMI_TYPE_MEMORY_DEVICE           = 17,  ///< Memory device
    DMI_TYPE_MEMORY_ERROR_32         = 18,  ///< 32-bit memory error information
    DMI_TYPE_MEMORY_ARRAY_ADDR       = 19,  ///< Memory array mapped address
    DMI_TYPE_MEMORY_DEVICE_ADDR      = 20,  ///< Memory device mapped address
    DMI_TYPE_POINTING_DEVICE         = 21,  ///< Built-in pointing device
    DMI_TYPE_PORTABLE_BATTERY        = 22,  ///< Portable battery
    DMI_TYPE_SYSTEM_RESET            = 23,  ///< System reset
    DMI_TYPE_HARDWARE_SECURITY       = 24,  ///< Hardware security
    DMI_TYPE_POWER_CONTROLS          = 25,  ///< System power controls
    DMI_TYPE_VOLTAGE_PROBE           = 26,  ///< Voltage probe
    DMI_TYPE_COOLING_DEVICE          = 27,  ///< Cooling device
    DMI_TYPE_TEMPERATURE_PROBE       = 28,  ///< Temperature probe
    DMI_TYPE_CURRENT_PROBE           = 29,  ///< Electrical current probe
    DMI_TYPE_OOB_REMOTE_ACCESS       = 30,  ///< Out-of-band remote access
    DMI_TYPE_BIS_ENTRY_POINT         = 31,  ///< Boot Integrity Services (BIS) entry point
    DMI_TYPE_SYSTEM_BOOT             = 32,  ///< System boot information
    DMI_TYPE_MEMORY_ERROR_64         = 33,  ///< 64-bit memory error information
    DMI_TYPE_MGMT_DEVICE             = 34,  ///< Management device
    DMI_TYPE_MGMT_DEVICE_COMPONENT   = 35,  ///< Management device component
    DMI_TYPE_MGMT_DEVICE_THRESHOLD   = 36,  ///< Management device threshold data
    DMI_TYPE_MEMORY_CHANNEL          = 37,  ///< Memory channel
    DMI_TYPE_IPMI_DEVICE             = 38,  ///< IPMI device information
    DMI_TYPE_POWER_SUPPLY            = 39,  ///< System power supply
    DMI_TYPE_ADDITIONAL_INFO         = 40,  ///< Additional information
    DMI_TYPE_ONBOARD_DEVICE_EX       = 41,  ///< Onboard devices extended information
    DMI_TYPE_MGMT_CONTROLLER_HOST_IF = 42,  ///< Management controller host interface
    DMI_TYPE_TPM_DEVICE              = 43,  ///< TPM device
    DMI_TYPE_PROCESSOR_EX            = 44,  ///< Processor additional information
    DMI_TYPE_FIRMWARE_INVENTORY      = 45,  ///< Firmware inventory information
    DMI_TYPE_STRING_PROPERTY         = 46,  ///< String property
    DMI_TYPE_INACTIVE                = 126, ///< Inactive
    DMI_TYPE_END_OF_TABLE            = 127, ///< End of table
    __DMI_TYPE_OEM_START             = 128,
    __DMI_TYPE_COUNT
} dmi_type_t;

#define DMI_TYPE(x) ((dmi_type_t)(DMI_TYPE_ ## x))

#define DMI_TYPE_MAX UINT8_MAX

typedef struct dmi_context dmi_context_t;
typedef struct dmi_entity  dmi_entity_t;

#endif // !OPENDMI_TYPES_H
