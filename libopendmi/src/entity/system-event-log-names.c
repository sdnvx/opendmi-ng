//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/reader.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/system-event-log-internal.h>

const dmi_name_set_t dmi_system_log_access_method_names =
{
    .code  = "access-method",
    .names = DMI_NAMES({
        {
            .id   = DMI_SYSTEM_LOG_ACCESS_METHOD_INDEXED_IO_8BIT_1_1,
            .code = "indexed-io-8bit-1-1",
            .name = "Indexed I/O: 1 8-bit index port, 1 8-bit data port"
        },
        {
            .id   = DMI_SYSTEM_LOG_ACCESS_METHOD_INDEXED_IO_8BIT_2_1,
            .code = "indexed-io-8bit-2-1",
            .name = "Indexed I/O: 2 8-bit index ports, 1 8-bit data port"
        },
        {
            .id   = DMI_SYSTEM_LOG_ACCESS_METHOD_INDEXED_IO_16BIT,
            .code = "indexed-io-16bit",
            .name = "Indexed I/O: 1 16-bit index port, 1 8-bit data port"
        },
        {
            .id   = DMI_SYSTEM_LOG_ACCESS_METHOD_MMAP_32BIT_ADDR,
            .code = "memory-mapped-32bit-address",
            .name = "Memory-mapped physical 32-bit address"
        },
        {
            .id   = DMI_SYSTEM_LOG_ACCESS_METHOD_DATA_FUNCTIONS,
            .code = "data-functions",
            .name = "General-purpose non-volatile data functions"
        },
        {}
    })
};

const dmi_name_set_t dmi_system_log_status_names =
{
    .code = "system-log-status",
    .names = DMI_NAMES({
        {
            .id   = 0,
            .code = "log-area-valid",
            .name = "Log area valid"
        },
        {
            .id   = 1,
            .code = "log-area-full",
            .name = "Log area full"
        },
        {}
    })
};

const dmi_name_set_t dmi_system_log_header_format_names =
{
    .code   = "system-log-header-format",
    .names  = DMI_NAMES({
        {
            .id   = DMI_SYSTEM_LOG_HEADER_FMT_NO_HEADER,
            .code = "no-header",
            .name = "No header"
        },
        {
            .id   = DMI_SYSTEM_LOG_HEADER_FMT_TYPE_1,
            .code = "type-1",
            .name = "Type 1 log header"
        },
        {}
    }),
    .ranges = DMI_NAME_RANGES({
        {
            .start_id = DMI_SYSTEM_LOG_HEADER_FMT_OEM_START,
            .end_id   = DMI_SYSTEM_LOG_HEADER_FMT_OEM_END,
            .code     = "oem-specific",
            .name     = "OEM-specific"
        },
        {}
    })
};

const dmi_name_set_t dmi_event_log_type_names =
{
    .code   = "event-log-type",
    .names  = DMI_NAMES({
        {
            .id   = DMI_EVENT_LOG_TYPE_SINGLE_BIT_ECC,
            .code = "single-bit-ecc",
            .name = "Single-bit ECC memory error"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_MULTI_BIT_ECC,
            .code = "multi-bit-ecc",
            .name = "Multi-bit ECC memory error"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_PARITY,
            .code = "parity",
            .name = "Parity memory error"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_BUS_TIMEOUT,
            .code = "bus-timeout",
            .name = "Bus time-out"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_IO_CHANNEL_CHECK,
            .code = "io-channel-check",
            .name = "I/O channel check"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_SOFTWARE_NMI,
            .code = "software-nmi",
            .name = "Software NMI"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_POST_MEMORY_RESIZE,
            .code = "post-memory-resize",
            .name = "POST memory resize"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_POST_ERROR,
            .code = "post-error",
            .name = "POST error"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_PCI_PARITY,
            .code = "pci-parity",
            .name = "PCI parity error"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_PCI_SYSTEM,
            .code = "pci-system",
            .name = "PCI system error"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_CPU_FAILURE,
            .code = "cpu-failure",
            .name = "CPU failure"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_EISA_FAILSAFE_TIMEOUT,
            .code = "eisa-failsafe-timeout",
            .name = "EISA FailSafe timer time-out"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_MEMORY_LOG_DISABLED,
            .code = "memory-log-disabled",
            .name = "Correctable memory log disabled"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_LOGGING_DISABLED,
            .code = "logging-disabled",
            .name = "Logging disabled for a specific event type"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_LIMIT_EXCEEDED,
            .code = "limit-exceeded",
            .name = "System limit exceeded"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_TIMER_EXPIRED,
            .code = "timer-expired",
            .name = "Asynchronous hardware timer expired"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_CONFIG_INFO,
            .code = "config-info",
            .name = "System configuration information"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_HARD_DISK_INFO,
            .code = "hard-disk-info",
            .name = "Hard disk information"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_RECONFIGURED,
            .code = "reconfigured",
            .name = "System reconfigured"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_CPU_COMPLEX_ERROR,
            .code = "cpu-complex-error",
            .name = "Uncorrectable CPU-complex error"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_LOG_CLEARED,
            .code = "log-cleared",
            .name = "Log area reset or cleared"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_SYSTEM_BOOT,
            .code = "system-boot",
            .name = "System boot"
        },
        {
            .id   = DMI_EVENT_LOG_TYPE_END_OF_LOG,
            .code = "end-of-log",
            .name = "End of log"
        },
        {}
    }),
    .ranges = DMI_NAME_RANGES({
        {
            .start_id = __DMI_EVENT_LOG_TYPE_UNUSED_START,
            .end_id   = __DMI_EVENT_LOG_TYPE_UNUSED_END,
            .code     = "unused",
            .name     = "Unused"
        },
        {
            .start_id = __DMI_EVENT_LOG_TYPE_OEM_START,
            .end_id   = __DMI_EVENT_LOG_TYPE_OEM_END,
            .code     = "oem-specific",
            .name     = "System/OEM-specific"
        },
        {}
    })
};

const dmi_name_set_t dmi_event_log_data_format_names =
{
    .code   = "event-log-data-format",
    .names  = DMI_NAMES({
        {
            .id   = DMI_EVENT_LOG_DATA_FORMAT_NONE,
            .code = "none",
            .name = "None"
        },
        {
            .id   = DMI_EVENT_LOG_DATA_FORMAT_HANDLE,
            .code = "handle",
            .name = "Handle"
        },
        {
            .id   = DMI_EVENT_LOG_DATA_FORMAT_MULTIPLE_EVENT,
            .code = "multiple-event",
            .name = "Multiple-event"
        },
        {
            .id   = DMI_EVENT_LOG_DATA_FORMAT_MULTIPLE_HANDLE,
            .code = "multiple-event-handle",
            .name = "Multiple-event handle"
        },
        {
            .id   = DMI_EVENT_LOG_DATA_FORMAT_POST_RESULTS,
            .code = "post-results",
            .name = "POST results bitmap"
        },
        {
            .id   = DMI_EVENT_LOG_DATA_FORMAT_SYSTEM_MGMT,
            .code = "system-management",
            .name = "System management type"
        },
        {
            .id   = DMI_EVENT_LOG_DATA_FORMAT_MULTIPLE_SYSTEM_MGMT,
            .code = "multiple-event-system-management",
            .name = "Multiple-event system management type"
        },
        {}
    }),
    .ranges = DMI_NAME_RANGES({
        {
            .start_id = __DMI_EVENT_LOG_DATA_FORMAT_UNUSED_START,
            .end_id   = __DMI_EVENT_LOG_DATA_FORMAT_UNUSED_END,
            .code     = "unused",
            .name     = "Unused"
        },
        {
            .start_id = __DMI_EVENT_LOG_DATA_FORMAT_OEM_START,
            .end_id   = __DMI_EVENT_LOG_DATA_FORMAT_OEM_END,
            .code     = "oem-specific",
            .name     = "OEM-specific"
        },
        {}
    })
};
