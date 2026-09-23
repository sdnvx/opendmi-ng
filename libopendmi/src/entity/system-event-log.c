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
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x14,
        .decoded_length  = sizeof(dmi_system_event_log_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_system_event_log_t, area_length,    dmi_word_t),
        DMI_FIELD(dmi_system_event_log_t, header_offset,  dmi_word_t),
        DMI_FIELD(dmi_system_event_log_t, data_offset,    dmi_word_t),
        DMI_FIELD(dmi_system_event_log_t, access_method,  dmi_byte_t),
        DMI_FIELD(dmi_system_event_log_t, status,         dmi_byte_t),
        DMI_FIELD(dmi_system_event_log_t, change_token,   dmi_dword_t),
        DMI_FIELD(dmi_system_event_log_t, access_address, dmi_dword_t),

        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 1, 0)),
        DMI_FIELD(dmi_system_event_log_t, header_format, dmi_byte_t),

        // Descriptors are two bytes long, and longer ones are allowed for
        // future extensions, so the next one is found by the declared length
        DMI_FIELD_ARRAY(dmi_system_event_log_t, descriptors, descriptor_count,
            .count_length   = sizeof(dmi_byte_t),
            .stride_length  = sizeof(dmi_byte_t),
            .stride_minimum = 2,
            .fields         = DMI_FIELDS({
                DMI_FIELD(dmi_system_log_type_descriptor_t, type,        dmi_byte_t),
                DMI_FIELD(dmi_system_log_type_descriptor_t, data_format, dmi_byte_t),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
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
            .variants = DMI_VARIANTS({
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
                {}
            })
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
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("system-event-log.area", dmi_system_event_log_lint_area, {
            .name              = "Header and data of the log are within its area",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        DMI_LINT_RULE("system-event-log.descriptors", dmi_system_event_log_lint_descriptors, {
            .name              = "Descriptors of the supported log types are of the expected length",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    }),

    .handlers = {
        .derive  = dmi_system_event_log_derive,
        .cleanup = dmi_system_event_log_cleanup
    }
};
