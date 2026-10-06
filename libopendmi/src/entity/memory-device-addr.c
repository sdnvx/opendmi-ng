//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <inttypes.h>
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/registry.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>
#include <opendmi/entity/memory-array-addr.h>

#include "memory-device-addr-internal.h"

const dmi_entity_spec_t dmi_memory_device_addr_spec =
{
    .code            = "memory-device-address",
    .name            = "Memory device mapped address",
    .description     = (const char *[]){
        "This structure maps memory address space usually to a device-level "
        "granularity. One structure is present for each contiguous address "
        "range described.",
        //
        "Note: A Memory Device Mapped Address structure is provided only if "
        "a Memory Device has a mapped address; there is no provision within "
        "this structure to map a zero-length address space.",
        //
        nullptr
    },
    .type            = DMI_TYPE(memory_device_addr),
    .params = {
        .minimum_version = DMI_VERSION(2, 1, 0),
        .minimum_length  = 0x13,
        .decoded_length  = sizeof(dmi_memory_device_addr_t)
    },

    .fields = DMI_FIELDS({
        // Addresses are carried in kilobytes, and the ranges which do not fit
        // into four bytes are carried by the extended fields instead. The end
        // is the last kilobyte of the range, and is decoded into its last
        // byte, which is what the extended field carries
        DMI_FIELD(dmi_memory_device_addr_t, start_address, dmi_dword_t,
                  .decode = dmi_field_decode_kilobytes,
                  .encode = dmi_field_encode_kilobytes),
        DMI_FIELD(dmi_memory_device_addr_t, end_address, dmi_dword_t,
                  .decode = dmi_field_decode_kilobytes_last,
                  .encode = dmi_field_encode_kilobytes),

        DMI_FIELD(dmi_memory_device_addr_t, device_handle,     dmi_word_t),
        DMI_FIELD(dmi_memory_device_addr_t, array_address_handle, dmi_word_t),

        DMI_FIELD(dmi_memory_device_addr_t, partition_pos,    dmi_byte_t, .unknown_raw = 0xFFu),
        DMI_FIELD(dmi_memory_device_addr_t, interleave_pos,   dmi_byte_t, .unknown_raw = 0xFFu),
        DMI_FIELD(dmi_memory_device_addr_t, interleave_depth, dmi_byte_t, .unknown_raw = 0xFFu),

        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 7, 0)),
        DMI_FIELD_EXTENDED(dmi_memory_device_addr_t, start_address, dmi_qword_t,
                           .when_raw = 0xFFFFFFFFu),
        DMI_FIELD_EXTENDED(dmi_memory_device_addr_t, end_address, dmi_qword_t,
                           .when     = dmi_member(dmi_memory_device_addr_t, start_address),
                           .when_raw = 0xFFFFFFFFu),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_memory_device_addr_t, start_address, ADDRESS, {
            .code    = "start-address",
            .name    = "Starting address",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_memory_device_addr_t, end_address, ADDRESS, {
            .code    = "end-address",
            .name    = "Ending address",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_memory_device_addr_t, range_size, SIZE, {
            .code    = "range-size",
            .name    = "Range size"
        }),
        DMI_ATTRIBUTE(dmi_memory_device_addr_t, device_handle, HANDLE, {
            .code    = "device-handle",
            .name    = "Device handle",
            .targets = dmi_types(DMI_TYPE(memory_device)),
            .link    = dmi_member(dmi_memory_device_addr_t, device)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_addr_t, array_address_handle, HANDLE, {
            .code    = "array-address-handle",
            .name    = "Array mapped address handle",
            .targets = dmi_types(DMI_TYPE(memory_array_addr)),
            .link    = dmi_member(dmi_memory_device_addr_t, array_address)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_addr_t, partition_pos, INTEGER, {
            .code    = "partition-pos",
            .name    = "Partition row position",
            .unknown = dmi_value_ptr((unsigned short)USHRT_MAX)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_addr_t, interleave_pos, INTEGER, {
            .code    = "interleave-pos",
            .name    = "Interleave position",
            .unknown = dmi_value_ptr((unsigned short)USHRT_MAX)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_addr_t, interleave_depth, INTEGER, {
            .code    = "interleave-depth",
            .name    = "Interleave data depth",
            .unknown = dmi_value_ptr((unsigned short)USHRT_MAX)
        }),
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("memory-device-address.range", dmi_memory_device_addr_lint_range, {
            .name              = "Mapped address range starts before it ends",
            .reader_severity   = DMI_LINT_SEVERITY_ERROR,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        DMI_LINT_RULE("memory-device-address.bounds", dmi_memory_device_addr_lint_bounds, {
            .name              = "Mapped address range fits the one of its array",
            .reader_severity   = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    }),

    .handlers = {
        .validate = dmi_memory_device_addr_validate,
        .derive   = dmi_memory_device_addr_derive
    }
};
