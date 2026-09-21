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
