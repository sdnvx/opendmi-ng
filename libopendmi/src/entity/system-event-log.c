//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/system-event-log.h>

static bool dmi_system_event_log_decode(dmi_entity_t *entity);
static void dmi_system_event_log_cleanup(dmi_entity_t *entity);

static const dmi_name_set_t dmi_system_log_access_method_names =
{
    .code  = "access-method",
    .names = (dmi_name_t[]){
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
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_system_log_status_names =
{
    .code = "system-log-status",
    .names = (dmi_name_t[]){
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
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_system_log_header_format_names =
{
    .code   = "system-log-header-format",
    .names  = (dmi_name_t[]){
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
        DMI_NAME_NULL
    },
    .ranges = (dmi_name_range_t[]){
        {
            .start_id = DMI_SYSTEM_LOG_HEADER_FMT_OEM_START,
            .end_id   = DMI_SYSTEM_LOG_HEADER_FMT_OEM_END,
            .code     = "oem-specific",
            .name     = "OEM-specific"
        },
        DMI_NAME_RANGE_NULL
    }
};

static const dmi_name_set_t dmi_event_log_type_names =
{
    .code   = "event-log-type",
    .names  = (dmi_name_t[]){
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
        DMI_NAME_NULL
    },
    .ranges = (dmi_name_range_t[]){
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
        DMI_NAME_RANGE_NULL
    }
};

static const dmi_name_set_t dmi_event_log_data_format_names =
{
    .code   = "event-log-data-format",
    .names  = (dmi_name_t[]){
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
        DMI_NAME_NULL
    },
    .ranges = (dmi_name_range_t[]){
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
        DMI_NAME_RANGE_NULL
    }
};

static const dmi_attribute_t dmi_system_log_type_descriptor_attrs[] =
{
    DMI_ATTRIBUTE(dmi_system_log_type_descriptor_t, type, ENUM, {
        .code   = "type",
        .name   = "Type",
        .values = &dmi_event_log_type_names
    }),
    DMI_ATTRIBUTE(dmi_system_log_type_descriptor_t, data_format, ENUM, {
        .code   = "data-format",
        .name   = "Data format",
        .values = &dmi_event_log_data_format_names
    }),
    DMI_ATTRIBUTE_NULL
};

static const dmi_attribute_t dmi_system_log_io_ports_attrs[] =
{
    DMI_ATTRIBUTE(dmi_system_log_io_ports_t, index_port, INTEGER, {
        .code  = "index-port",
        .name  = "Index port",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_system_log_io_ports_t, data_port, INTEGER, {
        .code  = "data-port",
        .name  = "Data port",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE_NULL
};

//
// Access method address of indexed I/O access methods contains I/O ports
//
#define dmi_system_log_io_ports_variant(__method)                            \
    DMI_VARIANT(__method, dmi_system_event_log_t, access_ports, STRUCT, {    \
        .attrs = dmi_system_log_io_ports_attrs                               \
    })

const dmi_entity_spec_t dmi_system_event_log_spec =
{
    .code            = "system-event-log",
    .name            = "System event log",
    .description     = (const char *[]){
        "The presence of this structure within the SMBIOS data returned for a "
        "system indicates that the system supports an event log. An event log "
        "is a fixed-length area within a non-volatile storage element, "
        "starting with a fixed-length (and vendor-specific) header record, "
        "followed by one or more variable-length log records.",
        //
        "An application can implement event-log change notification by "
        "periodically reading the System Event Log structure (by its assigned "
        "handle) and looking for a change in the Log Change Token. This token "
        "uniquely identifies the last time the event log was updated. When it "
        "sees the token changed, the application can retrieve the entire "
        "event log and determine the changes since the last time it read the "
        "event log.",
        //
        nullptr
    },
    .type            = DMI_TYPE(SYSTEM_EVENT_LOG),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x14,
    .decoded_length  = sizeof(dmi_system_event_log_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_system_event_log_t, area_length, INTEGER, {
            .code   = "area-length",
            .name   = "Area length",
            .unit   = DMI_UNIT_BYTE
        }),
        DMI_ATTRIBUTE(dmi_system_event_log_t, header_offset, INTEGER, {
            .code   = "header-offset",
            .name   = "Header start offset",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_system_event_log_t, data_offset, INTEGER, {
            .code   = "data-offset",
            .name   = "Data start offset",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_system_event_log_t, access_method, ENUM, {
            .code   = "access-method",
            .name   = "Access method",
            .values = &dmi_system_log_access_method_names
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_system_event_log_t, access_method, {
            .code     = "access-address",
            .name     = "Access method address",
            .variants = (const dmi_attribute_variant_t[]){
                dmi_system_log_io_ports_variant(DMI_SYSTEM_LOG_ACCESS_METHOD_INDEXED_IO_8BIT_1_1),
                dmi_system_log_io_ports_variant(DMI_SYSTEM_LOG_ACCESS_METHOD_INDEXED_IO_8BIT_2_1),
                dmi_system_log_io_ports_variant(DMI_SYSTEM_LOG_ACCESS_METHOD_INDEXED_IO_16BIT),
                DMI_VARIANT(DMI_SYSTEM_LOG_ACCESS_METHOD_MMAP_32BIT_ADDR, dmi_system_event_log_t,
                            access_address, ADDRESS, {}),
                DMI_VARIANT(DMI_SYSTEM_LOG_ACCESS_METHOD_DATA_FUNCTIONS, dmi_system_event_log_t,
                            access_gpnv_handle, INTEGER, { .flags = DMI_ATTRIBUTE_FLAG_HEX }),
                DMI_VARIANT_DEFAULT(dmi_system_event_log_t, access_address, INTEGER, {
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                DMI_VARIANT_NULL
            }
        }),
        DMI_ATTRIBUTE(dmi_system_event_log_t, status, SET, {
            .code = "status",
            .name = "Status",
            .values = &dmi_system_log_status_names,
        }),
        DMI_ATTRIBUTE(dmi_system_event_log_t, change_token, INTEGER, {
            .code   = "change-token",
            .name   = "Change token",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_system_event_log_t, header_format, ENUM, {
            .code   = "header-format",
            .name   = "Header format",
            .values = &dmi_system_log_header_format_names,
            .level  = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_system_event_log_t, descriptors, descriptor_count, STRUCT, {
            .code   = "descriptors",
            .name   = "Supported log type descriptors",
            .attrs  = dmi_system_log_type_descriptor_attrs,
            .level  = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode  = dmi_system_event_log_decode,
        .cleanup = dmi_system_event_log_cleanup
    }
};

static bool dmi_system_event_log_decode(dmi_entity_t *entity)
{
    dmi_system_event_log_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_EVENT_LOG));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);
    dmi_stream_t *stream   = dmi_entity_stream(entity);

    // SMBIOS 2.0 fields
    dmi_byte_t status_value = 0;

    bool status =
        dmi_stream_decode(stream, dmi_word_t, &info->area_length) and
        dmi_stream_decode(stream, dmi_word_t, &info->header_offset) and
        dmi_stream_decode(stream, dmi_word_t, &info->data_offset) and
        dmi_stream_decode(stream, dmi_byte_t, &info->access_method) and
        dmi_stream_decode(stream, dmi_byte_t, &status_value) and
        dmi_stream_decode(stream, dmi_dword_t, &info->change_token) and
        dmi_stream_decode(stream, dmi_dword_t, &info->access_address);
    if (not status)
        return false;

    info->status.__value = status_value;

    // Access method address is interpreted according to the access method
    info->access_ports.index_port = (uint16_t)(info->access_address & 0xFFFFu);
    info->access_ports.data_port  = (uint16_t)(info->access_address >> 16);
    info->access_gpnv_handle      = (uint16_t)(info->access_address & 0xFFFFu);

    // SMBIOS 2.1 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 1, 0);

    dmi_byte_t descriptor_count  = 0;
    dmi_byte_t descriptor_length = 0;

    status =
        dmi_stream_decode(stream, dmi_byte_t, &info->header_format) and
        dmi_stream_decode(stream, dmi_byte_t, &descriptor_count) and
        dmi_stream_decode(stream, dmi_byte_t, &descriptor_length);
    if (not status)
        return dmi_entity_incomplete(entity);

    // Descriptors are two bytes long, longer ones are allowed for future
    // extensions
    if ((descriptor_count == 0) or (descriptor_length < 2))
        return true;

    info->descriptors = dmi_alloc_array(context, sizeof(*info->descriptors), descriptor_count);
    if (info->descriptors == nullptr)
        return false;

    // Only completely present descriptors are counted
    for (size_t i = 0; i < descriptor_count; i++) {
        dmi_system_log_type_descriptor_t *descriptor = &info->descriptors[i];

        status =
            dmi_stream_has(stream, descriptor_length) and
            dmi_stream_decode(stream, dmi_byte_t, &descriptor->type) and
            dmi_stream_decode(stream, dmi_byte_t, &descriptor->data_format) and
            dmi_stream_skip(stream, descriptor_length - 2u);
        if (not status)
            return dmi_entity_incomplete(entity);

        info->descriptor_count++;
    }

    return true;
}

static void dmi_system_event_log_cleanup(dmi_entity_t *entity)
{
    dmi_system_event_log_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_EVENT_LOG));
    if (info == nullptr)
        return;

    dmi_free(info->descriptors);
}
