//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SYSTEM_EVENT_LOG_H
#define OPENDMI_ENTITY_SYSTEM_EVENT_LOG_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_system_event_log   dmi_system_event_log_t;
typedef union  dmi_system_log_status   dmi_system_log_status_t;
typedef struct dmi_system_log_io_ports dmi_system_log_io_ports_t;
typedef struct dmi_system_log_type_descriptor dmi_system_log_type_descriptor_t;

typedef enum dmi_system_log_access_method {
    DMI_SYSTEM_LOG_ACCESS_METHOD_INDEXED_IO_8BIT_1_1 = 0x00, // Indexed I/O: 1 8-bit index port, 1 8-bit data port
    DMI_SYSTEM_LOG_ACCESS_METHOD_INDEXED_IO_8BIT_2_1 = 0x01, // Indexed I/O: 2 8-bit index ports, 1 8-bit data port
    DMI_SYSTEM_LOG_ACCESS_METHOD_INDEXED_IO_16BIT    = 0x02, // Indexed I/O: 1 16-bit index port, 1 8-bit data port
    DMI_SYSTEM_LOG_ACCESS_METHOD_MMAP_32BIT_ADDR     = 0x03, // Memory-mapped physical 32-bit address
    DMI_SYSTEM_LOG_ACCESS_METHOD_DATA_FUNCTIONS      = 0x04, // Available through general-purpose non-volatile data functions
    // Reserved: 0x05 .. 0x7H
    DMI_SYSTEM_LOG_ACCESS_METHOD_OEM_START           = 0x80, // 0x80 .. 0xFF Firmware vendor/OEM-specific
    DMI_SYSTEM_LOG_ACCESS_METHOD_OEM_END             = 0xFF, // 0x80 .. 0xFF Firmware vendor/OEM-specific
} dmi_system_log_access_method_t;

typedef enum dmi_system_log_header_fmt {
    DMI_SYSTEM_LOG_HEADER_FMT_NO_HEADER = 0x00, // No header
    DMI_SYSTEM_LOG_HEADER_FMT_TYPE_1    = 0x01, // Type 1 log header
    // Reserved: 0x02 .. 0x7EH
    DMI_SYSTEM_LOG_HEADER_FMT_OEM_START = 0x80, // 0x80 .. 0xFF Firmware OEM-specific format
    DMI_SYSTEM_LOG_HEADER_FMT_OEM_END   = 0xFF, // 0x80 .. 0xFF Firmware OEM-specific format
} dmi_system_log_header_fmt_t;

/**
 * @brief Event log types.
 */
typedef enum dmi_event_log_type
{
    DMI_EVENT_LOG_TYPE_SINGLE_BIT_ECC        = 0x01, ///< Single-bit ECC memory error
    DMI_EVENT_LOG_TYPE_MULTI_BIT_ECC         = 0x02, ///< Multi-bit ECC memory error
    DMI_EVENT_LOG_TYPE_PARITY                = 0x03, ///< Parity memory error
    DMI_EVENT_LOG_TYPE_BUS_TIMEOUT           = 0x04, ///< Bus time-out
    DMI_EVENT_LOG_TYPE_IO_CHANNEL_CHECK      = 0x05, ///< I/O channel check
    DMI_EVENT_LOG_TYPE_SOFTWARE_NMI          = 0x06, ///< Software NMI
    DMI_EVENT_LOG_TYPE_POST_MEMORY_RESIZE    = 0x07, ///< POST memory resize
    DMI_EVENT_LOG_TYPE_POST_ERROR            = 0x08, ///< POST error
    DMI_EVENT_LOG_TYPE_PCI_PARITY            = 0x09, ///< PCI parity error
    DMI_EVENT_LOG_TYPE_PCI_SYSTEM            = 0x0A, ///< PCI system error
    DMI_EVENT_LOG_TYPE_CPU_FAILURE           = 0x0B, ///< CPU failure
    DMI_EVENT_LOG_TYPE_EISA_FAILSAFE_TIMEOUT = 0x0C, ///< EISA FailSafe timer time-out
    DMI_EVENT_LOG_TYPE_MEMORY_LOG_DISABLED   = 0x0D, ///< Correctable memory log disabled
    DMI_EVENT_LOG_TYPE_LOGGING_DISABLED      = 0x0E, ///< Logging disabled for a specific event type
    DMI_EVENT_LOG_TYPE_LIMIT_EXCEEDED        = 0x10, ///< System limit exceeded
    DMI_EVENT_LOG_TYPE_TIMER_EXPIRED         = 0x11, ///< Asynchronous hardware timer expired
    DMI_EVENT_LOG_TYPE_CONFIG_INFO           = 0x12, ///< System configuration information
    DMI_EVENT_LOG_TYPE_HARD_DISK_INFO        = 0x13, ///< Hard disk information
    DMI_EVENT_LOG_TYPE_RECONFIGURED          = 0x14, ///< System reconfigured
    DMI_EVENT_LOG_TYPE_CPU_COMPLEX_ERROR     = 0x15, ///< Uncorrectable CPU-complex error
    DMI_EVENT_LOG_TYPE_LOG_CLEARED           = 0x16, ///< Log area reset or cleared
    DMI_EVENT_LOG_TYPE_SYSTEM_BOOT           = 0x17, ///< System boot
    DMI_EVENT_LOG_TYPE_END_OF_LOG            = 0xFF, ///< End of log

    __DMI_EVENT_LOG_TYPE_UNUSED_START        = 0x18,
    __DMI_EVENT_LOG_TYPE_UNUSED_END          = 0x7F,
    __DMI_EVENT_LOG_TYPE_OEM_START           = 0x80,
    __DMI_EVENT_LOG_TYPE_OEM_END             = 0xFE
} dmi_event_log_type_t;

/**
 * @brief Event log variable data format types.
 */
typedef enum dmi_event_log_data_format
{
    DMI_EVENT_LOG_DATA_FORMAT_NONE                = 0x00, ///< No standard format
    DMI_EVENT_LOG_DATA_FORMAT_HANDLE              = 0x01, ///< Handle of structure associated with the event
    DMI_EVENT_LOG_DATA_FORMAT_MULTIPLE_EVENT      = 0x02, ///< Multiple-event counter
    DMI_EVENT_LOG_DATA_FORMAT_MULTIPLE_HANDLE     = 0x03, ///< Handle and multiple-event counter
    DMI_EVENT_LOG_DATA_FORMAT_POST_RESULTS        = 0x04, ///< POST results bitmap
    DMI_EVENT_LOG_DATA_FORMAT_SYSTEM_MGMT         = 0x05, ///< System management type
    DMI_EVENT_LOG_DATA_FORMAT_MULTIPLE_SYSTEM_MGMT = 0x06, ///< Multiple-event system management type

    __DMI_EVENT_LOG_DATA_FORMAT_UNUSED_START      = 0x07,
    __DMI_EVENT_LOG_DATA_FORMAT_UNUSED_END        = 0x7F,
    __DMI_EVENT_LOG_DATA_FORMAT_OEM_START         = 0x80,
    __DMI_EVENT_LOG_DATA_FORMAT_OEM_END           = 0xFF
} dmi_event_log_data_format_t;

/**
 * @brief Supported event log type descriptor.
 */
struct dmi_system_log_type_descriptor
{
    /**
     * @brief Event log type.
     */
    dmi_event_log_type_t type;

    /**
     * @brief Format of variable data of the events.
     */
    dmi_event_log_data_format_t data_format;
};

dmi_packed_union(dmi_system_log_status)
{
    dmi_byte_t __value;

    dmi_packed_struct()
    {
        dmi_byte_t is_log_area_valid : 1;
        dmi_byte_t is_log_area_full  : 1;

        dmi_byte_t __reserved : 6;
    };
};

dmi_static_assert_value_union(dmi_system_log_status);

/**
 * @brief I/O ports of indexed I/O access methods.
 */
struct dmi_system_log_io_ports
{
    /**
     * @brief Index port address.
     */
    uint16_t index_port;

    /**
     * @brief Data port address.
     */
    uint16_t data_port;
};

/**
 * @brief System event log structure (type 15).
 */
struct dmi_system_event_log
{
    /**
     * @brief Length of the log area, including header and data, in bytes.
     */
    uint16_t area_length;

    /**
     * @brief Offset of the log header from the access method address.
     */
    uint16_t header_offset;

    /**
     * @brief Offset of the log data from the access method address.
     */
    uint16_t data_offset;

    dmi_system_log_access_method_t access_method;

    dmi_system_log_status_t status;

    /**
     * @brief Access method address, as stored. Contains the physical address
     * for memory-mapped access method.
     */
    uint32_t access_address;

    /**
     * @brief I/O ports, if the access method is indexed I/O.
     */
    dmi_system_log_io_ports_t access_ports;

    /**
     * @brief General-purpose non-volatile data handle, if the log is
     * available through general-purpose non-volatile data functions.
     */
    uint16_t access_gpnv_handle;

    /**
     * @brief Token, which changes every time the log is updated.
     */
    uint32_t change_token;

    /**
     * @brief Format of the log header.
     *
     * @since SMBIOS 2.1
     */
    dmi_system_log_header_fmt_t header_format;

    /**
     * @brief Number of supported event log type descriptors.
     *
     * @since SMBIOS 2.1
     */
    size_t descriptor_count;

    /**
     * @brief Supported event log type descriptors.
     *
     * @since SMBIOS 2.1
     */
    dmi_system_log_type_descriptor_t *descriptors;
};

/**
 * @brief System event log entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_system_event_log_spec;

#endif // !OPENDMI_ENTITY_SYSTEM_EVENT_LOG_H
