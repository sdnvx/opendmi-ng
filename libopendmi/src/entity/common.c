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
    {}
};
