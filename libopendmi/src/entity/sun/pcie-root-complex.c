//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/sun.h>
#include <opendmi/entity/sun/pcie-root-complex.h>

static bool dmi_sun_pcie_root_complex_decode(dmi_entity_t *entity);

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
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x08,
    .decoded_length  = sizeof(dmi_sun_pcie_root_complex_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_sun_pcie_root_complex_t, baseboard_handle, HANDLE, {
            .code  = "baseboard-handle",
            .name  = "Baseboard handle"
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
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode  = dmi_sun_pcie_root_complex_decode
    }
};

static bool dmi_sun_pcie_root_complex_decode(dmi_entity_t *entity)
{
    dmi_sun_pcie_root_complex_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SUN_PCIE_ROOT_COMPLEX));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = dmi_entity_stream(entity);
    dmi_word_t bdf = 0;

    bool status =
        dmi_stream_decode(stream, dmi_word_t, &info->baseboard_handle) and
        dmi_stream_decode(stream, dmi_word_t, &bdf);
    if (not status)
        return false;

    // Bus, device and function numbers are packed as in PCI routing IDs
    info->bus_number      = (uint8_t)(bdf >> 8);
    info->device_number   = (uint8_t)((bdf >> 3) & 0x1F);
    info->function_number = (uint8_t)(bdf & 0x07);

    return true;
}
