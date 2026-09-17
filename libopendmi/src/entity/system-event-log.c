//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/system-event-log.h>

static bool dmi_system_event_log_decode(dmi_entity_t *entity);

static const dmi_name_set_t dmi_system_log_access_method_names =
{
    .code  = "access-methods",
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
    .code = "status",
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
        DMI_ATTRIBUTE(dmi_system_event_log_t, access_method, ENUM, {
            .code   = "access-method",
            .name   = "Access Method",
            .values = &dmi_system_log_access_method_names
        }),
        DMI_ATTRIBUTE(dmi_system_event_log_t, status, SET, {
            .code = "status",
            .name = "Status",
            .values = &dmi_system_log_status_names,
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_system_event_log_decode
    }
};

static bool dmi_system_event_log_decode(dmi_entity_t *entity)
{
    dmi_system_event_log_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_EVENT_LOG));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    // SMBIOS 2.0 fields: log area length, header and data offsets, change
    // token and access method address are not decoded yet
    dmi_byte_t status_value = 0;

    bool status =
        dmi_stream_skip(stream, 3 * sizeof(dmi_word_t)) and
        dmi_stream_decode(stream, dmi_byte_t, &info->access_method) and
        dmi_stream_decode(stream, dmi_byte_t, &status_value) and
        dmi_stream_skip(stream, 2 * sizeof(dmi_dword_t));
    if (not status)
        return false;

    info->status.__value = status_value;

    // SMBIOS 2.1 fields: log header format and type descriptors are not
    // decoded yet
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 1, 0);

    if (not dmi_stream_skip(stream, 3 * sizeof(dmi_byte_t)))
        return dmi_entity_incomplete(entity);

    return true;
}
