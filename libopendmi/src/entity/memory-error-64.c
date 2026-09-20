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

#include <opendmi/entity/memory-error-64.h>

static bool dmi_memory_error_64_decode(dmi_entity_t *entity);

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
    .minimum_version = DMI_VERSION(2, 3, 0),
    .minimum_length  = 0x1F,
    .decoded_length  = sizeof(dmi_memory_error_64_t),
    .attributes      = (const dmi_attribute_t[]){
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
            .name    = "operation",
            .unspec  = dmi_value_ptr(DMI_MEMORY_ERROR_OPERATION_UNSPEC),
            .unknown = dmi_value_ptr(DMI_MEMORY_ERROR_OPERATION_UNKNOWN),
            .values  = &dmi_memory_error_operation_names
        }),
        DMI_ATTRIBUTE(dmi_memory_error_64_t, vendor_syndrome, INTEGER, {
            .code    = "vendor-syndrome",
            .name    = "vendor-syndrome",
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
        DMI_ATTRIBUTE_NULL
    },
    .handlers        = {
        .decode = dmi_memory_error_64_decode,
    }
};

static bool dmi_memory_error_64_decode(dmi_entity_t *entity)
{
    dmi_memory_error_64_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_ERROR_64));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = dmi_entity_stream(entity);

    return
        dmi_stream_decode(stream, dmi_byte_t, &info->type) and
        dmi_stream_decode(stream, dmi_byte_t, &info->granularity) and
        dmi_stream_decode(stream, dmi_byte_t, &info->operation) and
        dmi_stream_decode(stream, dmi_dword_t, &info->vendor_syndrome) and
        dmi_stream_decode(stream, dmi_qword_t, &info->array_addr) and
        dmi_stream_decode(stream, dmi_qword_t, &info->device_addr) and
        dmi_stream_decode(stream, dmi_dword_t, &info->resolution);
}
