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

bool dmi_pci_addr_decode(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    assert(data != nullptr);
    assert(value != nullptr);

    dmi_unused(field);

    dmi_pci_addr_t *addr = value;

    uint8_t bus_number             = (uint8_t)((data->number >> 16) & 0xFFu);
    uint8_t device_and_func_number = (uint8_t)((data->number >> 24) & 0xFFu);

    addr->segment_group = (uint16_t)(data->number & 0xFFFFu);
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

bool dmi_pci_addr_encode(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    assert(value != nullptr);
    assert(data != nullptr);

    dmi_unused(field);

    const dmi_pci_addr_t *addr = value;

    // Addresses of no bus carry no device and function either
    uintmax_t device_and_func_number = UINT8_MAX;

    if (addr->bus_number != UINT8_MAX) {
        device_and_func_number = ((addr->device_number & 0x1Fu) << 3) |
                                 (addr->function_number & 0x07u);
    }

    data->number = addr->segment_group |
                   ((uintmax_t)addr->bus_number << 16) |
                   (device_and_func_number << 24);

    return true;
}
