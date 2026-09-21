//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/sun.h>

#include <opendmi/entity/sun/pcie-root-complex-internal.h>

const dmi_entity_spec_t dmi_sun_pcie_root_complex_spec =
{
    .type            = DMI_TYPE(SUN_PCIE_ROOT_COMPLEX),
    .code            = "sun-pcie-root-complex",
    .name            = "Sun PCI-express root complex information",
    .description     = (const char *[]){
        "Describes the PCI address of a PCI-express root complex or root "
        "port and the baseboard containing it.",
        //
        nullptr
    },
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x08,
        .decoded_length  = sizeof(dmi_sun_pcie_root_complex_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_sun_pcie_root_complex_t, baseboard_handle, WORD),

        // Bus, device and function numbers are packed as in PCI routing IDs
        DMI_FIELD_BITS(dmi_sun_pcie_root_complex_t, function_number, 3),
        DMI_FIELD_BITS(dmi_sun_pcie_root_complex_t, device_number,   5),
        DMI_FIELD_BITS(dmi_sun_pcie_root_complex_t, bus_number,      8),
        DMI_FIELD_PAD(WORD),
        {}
    }),

    .attributes      = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_sun_pcie_root_complex_t, baseboard_handle, HANDLE, {
            .code  = "baseboard-handle",
            .name  = "Baseboard handle",
            .targets = dmi_types(DMI_TYPE_BASEBOARD),
        }),
        DMI_ATTRIBUTE(dmi_sun_pcie_root_complex_t, bus_number, INTEGER, {
            .code  = "bus-number",
            .name  = "Bus number",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_sun_pcie_root_complex_t, device_number, INTEGER, {
            .code  = "device-number",
            .name  = "Device number",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_sun_pcie_root_complex_t, function_number, INTEGER, {
            .code  = "function-number",
            .name  = "Function number"
        }),
        {}
    })
};
