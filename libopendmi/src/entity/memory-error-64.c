//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/memory-error-64-internal.h>

const dmi_entity_spec_t dmi_memory_error_64_spec =
{
    .code            = "memory-error-64",
    .name            = "64-bit memory error information",
    .description     = (const char *[]){
        "This structure describes an error within a Physical Memory Array "
        "when the error address is above 4GiB (0xFFFFFFFF).",
        //
        nullptr
    },
    .type            = DMI_TYPE(MEMORY_ERROR_64),
    .params = {
        .minimum_version = DMI_VERSION(2, 3, 0),
        .minimum_length  = 0x1F,
        .decoded_length  = sizeof(dmi_memory_error_64_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_memory_error_64_t, type,            BYTE),
        DMI_FIELD(dmi_memory_error_64_t, granularity,     BYTE),
        DMI_FIELD(dmi_memory_error_64_t, operation,       BYTE),
        DMI_FIELD(dmi_memory_error_64_t, vendor_syndrome, DWORD),
        DMI_FIELD(dmi_memory_error_64_t, array_addr,      QWORD),
        DMI_FIELD(dmi_memory_error_64_t, device_addr,     QWORD),
        DMI_FIELD(dmi_memory_error_64_t, resolution,      DWORD),
        {}
    }),

    .attributes      = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_memory_error_64_t, type, ENUM, {
            .code    = "type",
            .name    = "Type",
            .unspec  = dmi_value_ptr(DMI_MEMORY_ERROR_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_MEMORY_ERROR_TYPE_UNKNOWN),
            .values  = &dmi_memory_error_type_names
        }),
        DMI_ATTRIBUTE(dmi_memory_error_64_t, granularity, ENUM, {
            .code    = "granularity",
            .name    = "Granularity",
            .unspec  = dmi_value_ptr(DMI_MEMORY_ERROR_GRANULARITY_UNSPEC),
            .unknown = dmi_value_ptr(DMI_MEMORY_ERROR_GRANULARITY_UNKNOWN),
            .values  = &dmi_memory_error_granularity_names
        }),
        DMI_ATTRIBUTE(dmi_memory_error_64_t, operation, ENUM, {
            .code    = "operation",
            .name    = "Operation",
            .unspec  = dmi_value_ptr(DMI_MEMORY_ERROR_OPERATION_UNSPEC),
            .unknown = dmi_value_ptr(DMI_MEMORY_ERROR_OPERATION_UNKNOWN),
            .values  = &dmi_memory_error_operation_names
        }),
        DMI_ATTRIBUTE(dmi_memory_error_64_t, vendor_syndrome, INTEGER, {
            .code    = "vendor-syndrome",
            .name    = "Vendor syndrome",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_memory_error_64_t, array_addr, ADDRESS, {
            .code    = "array-addr",
            .name    = "Array-relative address",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_memory_error_64_t, device_addr, ADDRESS, {
            .code    = "device-addr",
            .name    = "Device-relative address",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_memory_error_64_t, resolution, SIZE, {
            .code    = "resolution",
            .name    = "Resolution"
        }),
        {}
    }),
};
