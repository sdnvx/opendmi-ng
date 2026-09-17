//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>

#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/common.h>

const dmi_name_set_t dmi_status_names =
{
    .code  = "statuses",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_STATUS_UNSPEC),
        DMI_NAME_OTHER(DMI_STATUS_OTHER),
        DMI_NAME_UNKNOWN(DMI_STATUS_UNKNOWN),
        {
            .id   = DMI_STATUS_OK,
            .code = "ok",
            .name = "OK"
        },
        {
            .id   = DMI_STATUS_NON_CRITICAL,
            .code = "non-critical",
            .name = "Non-critical"
        },
        {
            .id   = DMI_STATUS_CRITICAL,
            .code = "critical",
            .name = "Critical"
        },
        {
            .id   = DMI_STATUS_NON_RECOVERABLE,
            .code = "non-recoverable",
            .name = "Non-recoverable"
        },
        DMI_NAME_NULL
    }
};

const dmi_name_set_t dmi_error_correct_type_names =
{
    .code  = "error-correction-types",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_ERROR_CORRECT_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_ERROR_CORRECT_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_ERROR_CORRECT_TYPE_UNKNOWN),
        DMI_NAME_NONE(DMI_ERROR_CORRECT_TYPE_NONE),
        {
            .id   = DMI_ERROR_CORRECT_TYPE_PARITY,
            .code = "parity",
            .name = "Parity"
        },
        {
            .id   = DMI_ERROR_CORRECT_TYPE_SINGLE_BIT,
            .code = "single-bit",
            .name = "Single-bit ECC"
        },
        {
            .id   = DMI_ERROR_CORRECT_TYPE_MULTI_BIT,
            .code = "multi-bit",
            .name = "Multi-bit ECC"
        },
        {
            .id   = DMI_ERROR_CORRECT_TYPE_CRC,
            .code = "crc",
            .name = "CRC"
        },
        DMI_NAME_NULL
    }
};

const dmi_attribute_t dmi_pci_addr_attrs[] =
{
    DMI_ATTRIBUTE(dmi_pci_addr_t, segment_group, INTEGER, {
        .code   = "segment-group",
        .name   = "Segment group",
        .unspec = dmi_value_ptr((uint16_t)UINT16_MAX),
        .flags  = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_pci_addr_t, bus_number, INTEGER, {
        .code   = "bus-number",
        .name   = "Bus number",
        .unspec = dmi_value_ptr((uint8_t)UINT8_MAX),
        .flags  = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_pci_addr_t, device_number, INTEGER, {
        .code   = "device-number",
        .name   = "Device number",
        .unspec = dmi_value_ptr((uint8_t)UINT8_MAX),
        .flags  = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_pci_addr_t, function_number, INTEGER, {
        .code   = "function-number",
        .name   = "Function number",
        .unspec = dmi_value_ptr((uint8_t)UINT8_MAX),
        .flags  = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE_NULL
};

const char *dmi_status_name(dmi_status_t value)
{
    return dmi_name_lookup(&dmi_status_names, (int)value);
}

const char *dmi_error_correct_type_name(dmi_error_correct_type_t value)
{
    return dmi_name_lookup(&dmi_error_correct_type_names, (int)value);
}

bool dmi_pci_addr_decode(dmi_stream_t *stream, dmi_pci_addr_t *addr)
{
    assert(stream != nullptr);
    assert(addr != nullptr);

    uint16_t segment_group;
    uint8_t bus_number, device_and_func_number;

    bool status =
        dmi_stream_decode(stream, dmi_word_t, &segment_group) and
        dmi_stream_decode(stream, dmi_byte_t, &bus_number) and
        dmi_stream_decode(stream, dmi_byte_t, &device_and_func_number);

    if (not status)
        return false;

    addr->segment_group = segment_group;
    addr->bus_number    = bus_number;

    if (bus_number != UINT8_MAX) {
        addr->device_number   = (device_and_func_number >> 3) & 0x1Fu;
        addr->function_number = device_and_func_number & 0x07u;
    } else {
        addr->device_number   = UINT8_MAX;
        addr->function_number = UINT8_MAX;
    }

    return true;
}
