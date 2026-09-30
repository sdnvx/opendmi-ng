//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include <opendmi/entity/hpe/dimm-vendor.h>

const dmi_entity_spec_t dmi_hpe_dimm_vendor_spec =
{
    .type        = DMI_TYPE(HPE_DIMM_VENDOR),
    .code        = "hpe-dimm-vendor",
    .name        = "HP/HPE DIMM vendor information",
    .description = (const char *[]){
        "Tells the actual manufacturer of the module of a memory device "
        "(type 17), whose standard fields hold the ones of HPE.",
        //
        nullptr
    },
    .params = {
        .generations    = { .minimum = DMI_HPE_GEN9 },
        .minimum_length = 0x08,
        .decoded_length = sizeof(dmi_hpe_dimm_vendor_t)
    },

    // Manufacture date is in binary-coded decimal
    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_hpe_dimm_vendor_t, device_handle, dmi_word_t),
        DMI_FIELD_STRING(dmi_hpe_dimm_vendor_t, manufacturer),
        DMI_FIELD_STRING(dmi_hpe_dimm_vendor_t, part_number),

        DMI_FIELD_GROUP(),
        DMI_FIELD_STRING(dmi_hpe_dimm_vendor_t, serial_number),

        DMI_FIELD_GROUP(),
        DMI_FIELD_BCD(dmi_hpe_dimm_vendor_t, manufacture_year, dmi_byte_t),
        DMI_FIELD_BCD(dmi_hpe_dimm_vendor_t, manufacture_week, dmi_byte_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_dimm_vendor_t, device_handle, HANDLE, {
            .code    = "device-handle",
            .name    = "Memory device handle",
            .targets = dmi_types(DMI_TYPE_MEMORY_DEVICE)
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_vendor_t, manufacturer, STRING, {
            .code = "manufacturer",
            .name = "DIMM manufacturer"
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_vendor_t, part_number, STRING, {
            .code = "part-number",
            .name = "DIMM manufacturer part number"
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_vendor_t, serial_number, STRING, {
            .code  = "serial-number",
            .name  = "DIMM vendor serial number",
            .flags = DMI_ATTRIBUTE_FLAG_PRIVATE
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_vendor_t, manufacture_year, INTEGER, {
            .code   = "manufacture-year",
            .name   = "Manufacture year",
            .unspec = dmi_value_ptr((uint8_t)0)
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_vendor_t, manufacture_week, INTEGER, {
            .code   = "manufacture-week",
            .name   = "Manufacture week",
            .unspec = dmi_value_ptr((uint8_t)0)
        }),
        {}
    })
};
