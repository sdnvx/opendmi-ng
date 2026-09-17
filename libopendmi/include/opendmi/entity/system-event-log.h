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

typedef struct dmi_system_event_log  dmi_system_event_log_t;
typedef union  dmi_system_log_status dmi_system_log_status_t;

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

dmi_packed_union(dmi_system_log_access_method_addr)
{
    dmi_dword_t __value;

    dmi_packed_struct()
    {
        uint16_t index_addr;
        uint16_t data_addr;
    } io;

    uint32_t phys_addr_32bit;

    uint16_t gpnv_handle;
};

dmi_static_assert_value_union(dmi_system_log_access_method_addr);

/**
 * @brief System event log structure (type 15).
 */
struct dmi_system_event_log
{
    dmi_system_log_access_method_t access_method;

    dmi_system_log_status_t status;
};

/**
 * @brief System event log entity specification.
 */
extern const dmi_entity_spec_t dmi_system_event_log_spec;

#endif // !OPENDMI_ENTITY_SYSTEM_EVENT_LOG_H
