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
typedef enum dmi_type_id
{
    DMI_TYPE_ID_INVALID                 = -1,
    DMI_TYPE_ID_ANY                     = -1,
    DMI_TYPE_ID_FIRMWARE                = 0,   ///< Platform firmware information
    DMI_TYPE_ID_SYSTEM                  = 1,   ///< System information
    DMI_TYPE_ID_BASEBOARD               = 2,   ///< Baseboard or module information
    DMI_TYPE_ID_CHASSIS                 = 3,   ///< System enclosure or chassis
    DMI_TYPE_ID_PROCESSOR               = 4,   ///< Processor information
    DMI_TYPE_ID_MEMORY_CONTROLLER       = 5,   ///< Memory controller information (obsolete)
    DMI_TYPE_ID_MEMORY_MODULE           = 6,   ///< Memory module information (obsolete)
    DMI_TYPE_ID_CACHE                   = 7,   ///< Cache information
    DMI_TYPE_ID_PORT_CONNECTOR          = 8,   ///< Port connector information
    DMI_TYPE_ID_SYSTEM_SLOTS            = 9,   ///< System slots
    DMI_TYPE_ID_ONBOARD_DEVICE          = 10,  ///< Onboard devices information
    DMI_TYPE_ID_OEM_STRINGS             = 11,  ///< OEM strings
    DMI_TYPE_ID_SYSTEM_CONFIG_OPTIONS   = 12,  ///< System configuration options
    DMI_TYPE_ID_FIRMWARE_LANGUAGE       = 13,  ///< Firmware language information
    DMI_TYPE_ID_GROUP_ASSOC             = 14,  ///< Group associations
    DMI_TYPE_ID_SYSTEM_EVENT_LOG        = 15,  ///< System event log
    DMI_TYPE_ID_MEMORY_ARRAY            = 16,  ///< Physical memory array
    DMI_TYPE_ID_MEMORY_DEVICE           = 17,  ///< Memory device
    DMI_TYPE_ID_MEMORY_ERROR_32         = 18,  ///< 32-bit memory error information
    DMI_TYPE_ID_MEMORY_ARRAY_ADDR       = 19,  ///< Memory array mapped address
    DMI_TYPE_ID_MEMORY_DEVICE_ADDR      = 20,  ///< Memory device mapped address
    DMI_TYPE_ID_POINTING_DEVICE         = 21,  ///< Built-in pointing device
    DMI_TYPE_ID_PORTABLE_BATTERY        = 22,  ///< Portable battery
    DMI_TYPE_ID_SYSTEM_RESET            = 23,  ///< System reset
    DMI_TYPE_ID_HARDWARE_SECURITY       = 24,  ///< Hardware security
    DMI_TYPE_ID_POWER_CONTROLS          = 25,  ///< System power controls
    DMI_TYPE_ID_VOLTAGE_PROBE           = 26,  ///< Voltage probe
    DMI_TYPE_ID_COOLING_DEVICE          = 27,  ///< Cooling device
    DMI_TYPE_ID_TEMPERATURE_PROBE       = 28,  ///< Temperature probe
    DMI_TYPE_ID_CURRENT_PROBE           = 29,  ///< Electrical current probe
    DMI_TYPE_ID_OOB_REMOTE_ACCESS       = 30,  ///< Out-of-band remote access
    DMI_TYPE_ID_BIS_ENTRY_POINT         = 31,  ///< Boot Integrity Services (BIS) entry point
    DMI_TYPE_ID_SYSTEM_BOOT             = 32,  ///< System boot information
    DMI_TYPE_ID_MEMORY_ERROR_64         = 33,  ///< 64-bit memory error information
    DMI_TYPE_ID_MGMT_DEVICE             = 34,  ///< Management device
    DMI_TYPE_ID_MGMT_DEVICE_COMPONENT   = 35,  ///< Management device component
    DMI_TYPE_ID_MGMT_DEVICE_THRESHOLD   = 36,  ///< Management device threshold data
    DMI_TYPE_ID_MEMORY_CHANNEL          = 37,  ///< Memory channel
    DMI_TYPE_ID_IPMI_DEVICE             = 38,  ///< IPMI device information
    DMI_TYPE_ID_POWER_SUPPLY            = 39,  ///< System power supply
    DMI_TYPE_ID_ADDITIONAL_INFO         = 40,  ///< Additional information
    DMI_TYPE_ID_ONBOARD_DEVICE_EX       = 41,  ///< Onboard devices extended information
    DMI_TYPE_ID_MGMT_CONTROLLER_HOST_IF = 42,  ///< Management controller host interface
    DMI_TYPE_ID_TPM_DEVICE              = 43,  ///< TPM device
    DMI_TYPE_ID_PROCESSOR_EX            = 44,  ///< Processor additional information
    DMI_TYPE_ID_FIRMWARE_INVENTORY      = 45,  ///< Firmware inventory information
    DMI_TYPE_ID_STRING_PROPERTY         = 46,  ///< String property
    DMI_TYPE_ID_INACTIVE                = 126, ///< Inactive
    DMI_TYPE_ID_END_OF_TABLE            = 127, ///< End of table
    __DMI_TYPE_ID_OEM_START             = 128,
    __DMI_TYPE_ID_COUNT
} dmi_type_id_t;

#define DMI_TYPE_ID(x) ((dmi_type_id_t)(DMI_TYPE_ID_ ## x))

#define DMI_TYPE_ID_MAX UINT8_MAX

typedef struct dmi_type dmi_type_t;

/**
 * @brief Structure type, which tells what the information a structure is
 * decoded into is.
 *
 * Structures of a type are decoded into one C type, whatever layout they
 * have: the specifications of the layouts of a type, e.g. the ones of the
 * generations of a vendor, refer to it. Structures of different types may
 * share a type number, e.g. the ones vendors define for the same OEM type
 * number, and a structure type of a vendor may be found at another type
 * number on the platforms of another vendor, so a type is told by its object
 * rather than by its number: structure types are the constants declared
 * here and by the extension modules, e.g. `dmi_type_system`, which
 * `DMI_TYPE()` names.
 */
struct dmi_type
{
    /**
     * @brief Type number the specification of the type gives the structures.
     */
    dmi_type_id_t id;
};

/**
 * @brief Structure type of the given name, e.g. `DMI_TYPE(system)` for
 * `&dmi_type_system`.
 */
#define DMI_TYPE(x) (&dmi_type_ ## x)

/**
 * @brief Structure type standing for any type, where a type is optional.
 */
#define DMI_TYPE_ANY ((const dmi_type_t *)nullptr)

__BEGIN_DECLS

/** @brief Platform firmware information */
extern __dmi_api const dmi_type_t dmi_type_firmware;

/** @brief System information */
extern __dmi_api const dmi_type_t dmi_type_system;

/** @brief Baseboard or module information */
extern __dmi_api const dmi_type_t dmi_type_baseboard;

/** @brief System enclosure or chassis */
extern __dmi_api const dmi_type_t dmi_type_chassis;

/** @brief Processor information */
extern __dmi_api const dmi_type_t dmi_type_processor;

/** @brief Memory controller information (obsolete) */
extern __dmi_api const dmi_type_t dmi_type_memory_controller;

/** @brief Memory module information (obsolete) */
extern __dmi_api const dmi_type_t dmi_type_memory_module;

/** @brief Cache information */
extern __dmi_api const dmi_type_t dmi_type_cache;

/** @brief Port connector information */
extern __dmi_api const dmi_type_t dmi_type_port_connector;

/** @brief System slots */
extern __dmi_api const dmi_type_t dmi_type_system_slots;

/** @brief Onboard devices information */
extern __dmi_api const dmi_type_t dmi_type_onboard_device;

/** @brief OEM strings */
extern __dmi_api const dmi_type_t dmi_type_oem_strings;

/** @brief System configuration options */
extern __dmi_api const dmi_type_t dmi_type_system_config_options;

/** @brief Firmware language information */
extern __dmi_api const dmi_type_t dmi_type_firmware_language;

/** @brief Group associations */
extern __dmi_api const dmi_type_t dmi_type_group_assoc;

/** @brief System event log */
extern __dmi_api const dmi_type_t dmi_type_system_event_log;

/** @brief Physical memory array */
extern __dmi_api const dmi_type_t dmi_type_memory_array;

/** @brief Memory device */
extern __dmi_api const dmi_type_t dmi_type_memory_device;

/** @brief 32-bit memory error information */
extern __dmi_api const dmi_type_t dmi_type_memory_error_32;

/** @brief Memory array mapped address */
extern __dmi_api const dmi_type_t dmi_type_memory_array_addr;

/** @brief Memory device mapped address */
extern __dmi_api const dmi_type_t dmi_type_memory_device_addr;

/** @brief Built-in pointing device */
extern __dmi_api const dmi_type_t dmi_type_pointing_device;

/** @brief Portable battery */
extern __dmi_api const dmi_type_t dmi_type_portable_battery;

/** @brief System reset */
extern __dmi_api const dmi_type_t dmi_type_system_reset;

/** @brief Hardware security */
extern __dmi_api const dmi_type_t dmi_type_hardware_security;

/** @brief System power controls */
extern __dmi_api const dmi_type_t dmi_type_power_controls;

/** @brief Voltage probe */
extern __dmi_api const dmi_type_t dmi_type_voltage_probe;

/** @brief Cooling device */
extern __dmi_api const dmi_type_t dmi_type_cooling_device;

/** @brief Temperature probe */
extern __dmi_api const dmi_type_t dmi_type_temperature_probe;

/** @brief Electrical current probe */
extern __dmi_api const dmi_type_t dmi_type_current_probe;

/** @brief Out-of-band remote access */
extern __dmi_api const dmi_type_t dmi_type_oob_remote_access;

/** @brief Boot Integrity Services (BIS) entry point */
extern __dmi_api const dmi_type_t dmi_type_bis_entry_point;

/** @brief System boot information */
extern __dmi_api const dmi_type_t dmi_type_system_boot;

/** @brief 64-bit memory error information */
extern __dmi_api const dmi_type_t dmi_type_memory_error_64;

/** @brief Management device */
extern __dmi_api const dmi_type_t dmi_type_mgmt_device;

/** @brief Management device component */
extern __dmi_api const dmi_type_t dmi_type_mgmt_device_component;

/** @brief Management device threshold data */
extern __dmi_api const dmi_type_t dmi_type_mgmt_device_threshold;

/** @brief Memory channel */
extern __dmi_api const dmi_type_t dmi_type_memory_channel;

/** @brief IPMI device information */
extern __dmi_api const dmi_type_t dmi_type_ipmi_device;

/** @brief System power supply */
extern __dmi_api const dmi_type_t dmi_type_power_supply;

/** @brief Additional information */
extern __dmi_api const dmi_type_t dmi_type_additional_info;

/** @brief Onboard devices extended information */
extern __dmi_api const dmi_type_t dmi_type_onboard_device_ex;

/** @brief Management controller host interface */
extern __dmi_api const dmi_type_t dmi_type_mgmt_controller_host_if;

/** @brief TPM device */
extern __dmi_api const dmi_type_t dmi_type_tpm_device;

/** @brief Processor additional information */
extern __dmi_api const dmi_type_t dmi_type_processor_ex;

/** @brief Firmware inventory information */
extern __dmi_api const dmi_type_t dmi_type_firmware_inventory;

/** @brief String property */
extern __dmi_api const dmi_type_t dmi_type_string_property;

/** @brief Inactive */
extern __dmi_api const dmi_type_t dmi_type_inactive;

/** @brief End of table */
extern __dmi_api const dmi_type_t dmi_type_end_of_table;

__END_DECLS

typedef struct dmi_context dmi_context_t;
typedef struct dmi_entity  dmi_entity_t;

#endif // !OPENDMI_TYPES_H
