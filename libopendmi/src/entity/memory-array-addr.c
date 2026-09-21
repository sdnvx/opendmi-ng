//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <inttypes.h>
#include <opendmi/registry.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/memory-array-addr-internal.h>

const dmi_entity_spec_t dmi_memory_array_addr_spec =
{
    .code = "memory-array-address",
    .name = "Memory array mapped address",
    .type = DMI_TYPE(MEMORY_ARRAY_ADDR),

    .params = {
        .minimum_version = DMI_VERSION(2, 1, 0),
        .required_from   = DMI_VERSION(2, 3, 0),
        .minimum_length  = 0x0F,
        .decoded_length  = sizeof(dmi_memory_array_addr_t)
    },

    .fields = DMI_FIELDS({
        // Addresses are carried in kilobytes, and the ranges which do not fit
        // into four bytes are carried by the extended fields instead
        DMI_FIELD(dmi_memory_array_addr_t, start_addr, dmi_dword_t,
                  .decode = dmi_field_decode_kilobytes,
                  .encode = dmi_field_encode_kilobytes),
        DMI_FIELD(dmi_memory_array_addr_t, end_addr, dmi_dword_t,
                  .decode = dmi_field_decode_kilobytes,
                  .encode = dmi_field_encode_kilobytes),

        DMI_FIELD(dmi_memory_array_addr_t, array_handle,    dmi_word_t),
        DMI_FIELD(dmi_memory_array_addr_t, partition_width, dmi_byte_t),

        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 7, 0)),
        DMI_FIELD_EXTENDED(dmi_memory_array_addr_t, start_addr, dmi_qword_t,
                           .when_raw = 0xFFFFFFFFu),
        DMI_FIELD_EXTENDED(dmi_memory_array_addr_t, end_addr, dmi_qword_t,
                           .when     = dmi_member(dmi_memory_array_addr_t, start_addr),
                           .when_raw = 0xFFFFFFFFu),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_memory_array_addr_t, start_addr, ADDRESS, {
            .code   = "start-addr",
            .name   = "Starting address",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_memory_array_addr_t, end_addr, ADDRESS, {
            .code  = "end-addr",
            .name  = "Ending address",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_memory_array_addr_t, range_size, SIZE, {
            .code   = "range-size",
            .name   = "Range size"
        }),
        DMI_ATTRIBUTE(dmi_memory_array_addr_t, array_handle, HANDLE, {
            .code   = "array-handle",
            .name   = "Memory array handle",
            .targets = dmi_types(DMI_TYPE_MEMORY_ARRAY),
            .link   = dmi_member(dmi_memory_array_addr_t, array)
        }),
        DMI_ATTRIBUTE(dmi_memory_array_addr_t, partition_width, INTEGER, {
            .code   = "partition-width",
            .name   = "Partition width"
        }),
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("memory-array-address.range", dmi_memory_array_addr_lint_range, {
            .name              = "Mapped address range starts before it ends",
            .severity          = DMI_LINT_SEVERITY_ERROR,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        DMI_LINT_RULE("memory-array-address.overlap", dmi_memory_array_addr_lint_overlap, {
            .name              = "Mapped address ranges of the arrays do not overlap",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    }),

    .handlers = {
        .validate = dmi_memory_array_addr_validate,
        .derive   = dmi_memory_array_addr_derive
    }
};
